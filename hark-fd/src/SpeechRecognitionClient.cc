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

using namespace std;
using namespace FD;

class SpeechRecognitionClient;

DECLARE_NODE(SpeechRecognitionClient)
/*Node
 *
 * @name SpeechRecognitionClient
 * @category HARK:ASRIF
 * @description Connect to Julius/Julian via TCP/IP, and send source information, feature vectors, and mask vectors.
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


class SpeechRecognitionClient : public BufferedNode {
  int featuresID;
  int masksID;
  int sourcesID;
  int outputID;
  map<int, int> socket_map;

  bool mft_enabled;
  bool socket_enabled;
  string hostname;
  unsigned short port;
  int CLOSE_socket_id;    //Save socket_id for close socket at destructor.

public:
  SpeechRecognitionClient(string nodeName, ParameterSet params)
    : BufferedNode(nodeName, params)
  {
    featuresID = addInput("FEATURES");
    masksID = addInput("MASKS");
    sourcesID = addInput("SOURCES");
    outputID = addOutput("OUTPUT");
    CLOSE_socket_id = -1;

    mft_enabled = dereference_cast<bool>(parameters.get("MFM_ENABLED"));
    socket_enabled = dereference_cast<bool>(parameters.get("SOCKET_ENABLED"));
    hostname = object_cast<String>(parameters.get("HOST"));
    port = dereference_cast<int>(parameters.get("PORT"));
        
    inOrder = true;
  }
    

  ~SpeechRecognitionClient()
  {
    if( CLOSE_socket_id != -1 ) {
      close( CLOSE_socket_id );
    }
  }

  void calculate(int output_id, int count, Buffer &out)
  {
    ObjectRef features_ptr = getInput(featuresID, count);
    const Map<int,ObjectRef>& features =
	  object_cast<Map<int,ObjectRef> >(features_ptr);
    ObjectRef masks_ptr = getInput(masksID, count);
    const Map<int,ObjectRef>& masks =
	  object_cast<Map<int,ObjectRef> >(masks_ptr);
    RCPtr<Vector<ObjectRef> > sources_ptr = getInput(sourcesID, count);
	const Vector<ObjectRef> &sources = *sources_ptr;

	if(!socket_enabled){
	  out[count] = sources_ptr;
	  return;
	}

    for (int i = 0; i < sources.size(); i++) {
      RCPtr<Source> src = sources[i];

      map<int, int>::iterator socket_it = socket_map.find(src->id);
      int socket_id = -1;
      if (socket_it == socket_map.end()) {
        socket_id = connectToServer();
        if (socket_id != -1) {
          SourceInfo source_info;
          timeval tv;
          source_info.source_id = src->id;
          source_info.azimuth = 180 / M_PI * atan2f(src->x[1], src->x[0]);
          source_info.elevation = 180 / M_PI * asin(src->x[2]);
          gettimeofday(&tv, NULL);
          source_info.tv_sec = tv.tv_sec;
          source_info.tv_usec = tv.tv_usec;
            
          CLOSE_socket_id = socket_id;

          socket_map[src->id] = socket_id;
          int write_size = sizeof(SourceInfo);
          write(socket_id, &write_size, sizeof(int));
          //write(socket_id, &src->id, write_size);
          write(socket_id, &source_info, write_size);
          cout << "source_id " << src->id << " is being recognized.\n";
        }
      }
      else {
        socket_id = socket_it->second;
      }
      
      Map<int,ObjectRef>::const_iterator feature_it = features.find(src->id);
      Map<int,ObjectRef>::const_iterator mask_it = masks.find(src->id);
      if (feature_it != features.end() && mask_it != masks.end()) {
        const Vector<float>& feature =
          object_cast<Vector<float> >(feature_it->second);
        const Vector<float>& mask =
          object_cast<Vector<float> >(mask_it->second);
        int bytes_per_sample = feature.size() * sizeof(float);
        write(socket_id, &bytes_per_sample, sizeof(int));
        write(socket_id, &feature[0], bytes_per_sample);
        write(socket_id, &bytes_per_sample, sizeof(int));

        if (mft_enabled) {
          write(socket_id, &mask[0], bytes_per_sample);
        }
        else {
          vector<float> mask_reliable(mask.size());
          for (int i = 0; i < mask_reliable.size(); i++) {
            mask_reliable[i] = 1.0f;
          }
          write(socket_id, &mask_reliable[0], bytes_per_sample);
        }
      }
    }

/*** next for loop not pass to "else" , not send zero data ***
 *** so , save socket id and send zero after for loop (masunari)***/
int ZERO_send_socket_id = 0; /*** masunari ***/
        
    for (map<int, int>::iterator it = socket_map.begin(); it != socket_map.end();) {
      Map<int,ObjectRef>::const_iterator feature_it = features.find(it->first);
      Map<int,ObjectRef>::const_iterator mask_it = masks.find(it->first);
      if (feature_it != features.end() && mask_it != masks.end()) {
        ++it;
	ZERO_send_socket_id = it->second;
      }
      else {
        timeval tv;
        gettimeofday(&tv, NULL);
        printf("SpeechRecognitionClient: end %d %d %d.%06d\n",
               count, it->first, tv.tv_sec, tv.tv_usec);

        int zero = 0;
        write(it->second, &zero, sizeof(int));

        close(it->second);
        socket_map.erase(it++);
      }
    }

/*** masunari start ***/
    if( ZERO_send_socket_id != 0 ) {
      /******* 
	timeval tv;
        gettimeofday(&tv, NULL);
        printf("SpeechRecognitionClient: end %d %d.%06d\n",
               count, tv.tv_sec, tv.tv_usec);
	*******/

        int zero = 0;
        write(ZERO_send_socket_id, &zero, sizeof(int));

        //close(ZERO_send_socket_id);
        //
    }
/*** masunari end ***/


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
      close(socket_id);
      return -1;
    }

    return socket_id;
  }

  IN_ORDER_NODE_SPEEDUP(SpeechRecognitionClient)

};
