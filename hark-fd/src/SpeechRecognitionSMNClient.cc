/*
 * Copyright 2008 Kyoto University and Honda Motor Co.,Ltd.
 * All rights reserved.
 * HARK was developed by researchers in Okuno Laboratory
 * at the Kyoto University and Honda Research Institute Japan Co.,Ltd.
 */

#include <cstring>
#include "BufferedNode.h"
#include "Buffer.h"
#include "Vector.h"
#include "Map.h"
#include <sstream>
#include <iomanip>
#include "Source.h"
#include "mfcnet.h"

#include <sys/time.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netdb.h>
#include <assert.h>

using namespace std;
using namespace FD;

class SpeechRecognitionSMNClient;

DECLARE_NODE(SpeechRecognitionSMNClient)
/*Node
 *
 * @name SpeechRecognitionSMNClient
 * @category HARK:ASRIF
 * @description Connect to Julius/Julian via TCP/IP, and send source information, feature vectors after processing SMN, and mask vectors.
 *
 * @input_name FEATURES
 * @input_type Map<int,ObjectRef>
 * @input_description Feature vectors. The key is source ID, and the value is a feature vector (Vector<float>).
 *
 * @input_name MASKS
 * @input_type Map<int,ObjectRef>
 * @input_description Mask vectors (Vector<float>).
 *
 * @input_name SOURCES
 * @input_type Vector<ObjectRef>
 * @input_description Source locations with ID. Each element of the vector is a source location with ID specified by "Source".
 *
 * @output_name OUTPUT
 * @output_type Vector<ObjectRef>
 * @output_description The same as SOURCES.
 *
 * @parameter_name MFM_ENABLED
 * @parameter_type bool
 * @parameter_list true:false
 * @parameter_value true
 * @parameter_description MFM is enbaled if true. [default: true]
 *
 * @parameter_name HOST
 * @parameter_type string
 * @parameter_value 127.0.0.1
 * @parameter_description Hostname or IP of Julius/Julian server. [default: 127.0.0.1]
 *
 * @parameter_name PORT
 * @parameter_type int
 * @parameter_value 5530
 * @parameter_description Port number of Julius/Julian server. [default: 5530]
 *
 * @parameter_name SOCKET_ENABLED
 * @parameter_type bool
 * @parameter_list true:false
 * @parameter_value true
 * @parameter_description send data via socket if true. [default: true]
 *
END*/


class SpeechRecognitionSMNClient : public BufferedNode {
  int featuresID;
  int masksID;
  int sourcesID;
  int outputID;

  typedef RCPtr<Vector<float> > FVectorPtr;

  struct asr_param {
    SourceInfo src_info;
    RCPtr<Vector<FVectorPtr> > feature_list;
    RCPtr<Vector<FVectorPtr> > mask_list;
    FVectorPtr spec_mean;
  };

  map<int, int> socket_map;
  map<int, asr_param> param_map;

  bool mft_enabled;
  bool socket_enabled;
  string hostname;
  unsigned short port;
    
public:
  SpeechRecognitionSMNClient(string nodeName, ParameterSet params)
    : BufferedNode(nodeName, params)
  {
    featuresID = addInput("FEATURES");
    masksID = addInput("MASKS");
    sourcesID = addInput("SOURCES");
    outputID = addOutput("OUTPUT");

    mft_enabled = dereference_cast<bool>(parameters.get("MFM_ENABLED"));
    socket_enabled = dereference_cast<bool>(parameters.get("SOCKET_ENABLED"));
    hostname = object_cast<String>(parameters.get("HOST"));
    port = dereference_cast<int>(parameters.get("PORT"));
        
    inOrder = true;
  }

  ~SpeechRecognitionSMNClient()
  {
    for (map<int, asr_param>::iterator it = param_map.begin();
         it != param_map.end();) {
      sendASRParam(it->second);
      param_map.erase(it++);
    }
  }
  
  void calculate(int output_id, int count, Buffer &out)
  {
    ObjectRef features_ptr = getInput(featuresID, count);
    const Map<int, ObjectRef>& features =
      object_cast<Map<int, ObjectRef> >(features_ptr);
    ObjectRef masks_ptr = getInput(masksID, count);
    const Map<int, ObjectRef>& masks =
      object_cast<Map<int, ObjectRef> >(masks_ptr);
    RCPtr<Vector<ObjectRef> > sources_ptr = getInput(sourcesID, count);
    const Vector<ObjectRef>& sources = *sources_ptr;

    if(!socket_enabled){
      out[count] = sources_ptr;
      return;
    }

    for (int i = 0; i < sources.size(); i++) {
      RCPtr<Source> src = sources[i];

      Map<int, ObjectRef>::const_iterator
        feature_it = features.find(src->id);
      Map<int, ObjectRef>::const_iterator
        mask_it = masks.find(src->id);
      assert(feature_it != features.end() && mask_it != masks.end());

      if (param_map.find(src->id) == param_map.end()) {
        asr_param& new_param = param_map[src->id];
        new_param.src_info.source_id = src->id;
        new_param.src_info.azimuth =
          180 / M_PI * atan2f(src->x[1], src->x[0]);
        new_param.src_info.elevation = 180 / M_PI * asin(src->x[2]);
        timeval tv;
        gettimeofday(&tv, NULL);
        new_param.src_info.tv_sec = tv.tv_sec;
        new_param.src_info.tv_usec = tv.tv_usec;

        new_param.feature_list =
          new Vector<FVectorPtr>;
        new_param.mask_list =
          new Vector<FVectorPtr>;
        new_param.spec_mean =
          new Vector<float>(object_cast<Vector<float> >(feature_it->second).size() / 2);
      }
      asr_param& param = param_map[src->id];
            
      param.feature_list->push_back(feature_it->second);
      param.mask_list->push_back(mask_it->second);
      Vector<float>& feature = object_cast<Vector<float> >(feature_it->second);
      for (int i = 0; i < feature.size() / 2; i++) {
        (*param.spec_mean)[i] += object_cast<Vector<float> >(feature_it->second)[i];
      }
    }

    for (map<int, asr_param>::iterator it = param_map.begin();
         it != param_map.end();) {
      if (features.find(it->first) == features.end()) {
        sendASRParam(it->second);
        param_map.erase(it++);
      }
      else {
        ++it;
      }
    }
        
    out[count] = sources_ptr;
  }

  int connectToServer()
  {
    int socket_id;
    hostent* connect_host;
    if ((connect_host = gethostbyname(hostname.c_str())) == NULL) {
      //cerr << "bad hostname!\n";
      return -1;
    }

    sockaddr_in desthost;
    //unsigned short port = 5530;
    bzero((char*)&desthost, sizeof(desthost));
    desthost.sin_family = AF_INET;
    desthost.sin_port = htons(port);
    bcopy(connect_host->h_addr, (char*)&desthost.sin_addr, connect_host->h_length);

    if ((socket_id = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
      //cerr << "Socket for client failed.\n";
      return -1;
    }

    if (connect(socket_id, (sockaddr*)&desthost, sizeof(desthost)) == -1) {
      //cerr << "Connect failed.\n";
      return -1;
    }

    return socket_id;
  }

  void sendASRParam(asr_param& param)
  {
    int socket_id = connectToServer();

    int feature_dim = (*param.feature_list)[0]->size();
    int static_feature_dim = param.spec_mean->size();
    for (int i = 0; i < param.spec_mean->size(); i++) {
      (*param.spec_mean)[i] /= (float)param.feature_list->size();
    }
    int size = sizeof(param.src_info);
    write(socket_id, &size, sizeof(size));
    write(socket_id, &param.src_info, size);

    int bytes_per_sample = feature_dim * sizeof(float);
    for (int t = 0; t < param.feature_list->size(); t++) {
      Vector<float>& feature = *(*param.feature_list)[t];
      for (int i = 0; i < static_feature_dim; i++) {
        feature[i] -= (*param.spec_mean)[i];
      }
      write(socket_id, &bytes_per_sample, sizeof(int));
      write(socket_id, &(*(*param.feature_list)[t])[0], bytes_per_sample);
      write(socket_id, &bytes_per_sample, sizeof(int));

      if (mft_enabled) {
        write(socket_id, &(*(*param.mask_list)[t])[0], bytes_per_sample);
      }
      else {
        vector<float> mask_reliable(feature_dim);
        for (int i = 0; i < mask_reliable.size(); i++) {
          mask_reliable[i] = 1.0f;
        }
        write(socket_id, &mask_reliable[0], bytes_per_sample);
      }
    }
    int zero = 0;
    write(socket_id, &zero, sizeof(int));

    close(socket_id);
  }

  IN_ORDER_NODE_SPEEDUP(SpeechRecognitionSMNClient)

};
