/*
 * Copyright 2008 Kyoto University and Honda Motor Co.,Ltd.
 * All rights reserved.
 * HARK was developed by researchers in Okuno Laboratory
 * at the Kyoto University and Honda Research Institute Japan Co.,Ltd.
 */

#include <iostream>
#include <BufferedNode.h>
#include <Buffer.h>
#include <Vector.h>

#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h> 
#include <sys/ioctl.h>
#include <errno.h>

#include <pthread.h>
#include "HarkParamsObjects.h"

int module_send_reject_flag_HarkParamsDynReconf;
int module_sd_connect_flag_HarkParamsDynReconf;
int init_send_flag_HarkParamsDynReconf;

struct T_ConnectionData_HarkParamsDynReconf
{
  int fd1;
  int fd2;
  struct sockaddr_in caddr;
  socklen_t len;
};


using namespace std;
using namespace FD;

void *module_sd_connect_HarkParamsDynReconf(void *param)
{
  int rval;
  char buf[0]; 
  T_ConnectionData_HarkParamsDynReconf* pConnection = (T_ConnectionData_HarkParamsDynReconf*)param;

  while(module_sd_connect_flag_HarkParamsDynReconf){  
    printf("module_sd_connect : check\n");
    rval = recv(pConnection->fd2,buf,sizeof(buf),MSG_DONTWAIT);
    if (rval == -1 && errno == EAGAIN && init_send_flag_HarkParamsDynReconf == 1){
      module_send_reject_flag_HarkParamsDynReconf = 0;
    }else{
      printf("module_sd_connect : waiting\n");
      module_send_reject_flag_HarkParamsDynReconf = 1;
      pConnection->len = sizeof(pConnection->caddr);

      fd_set readfds;
      struct timeval timeout;
      FD_ZERO(&readfds);
      FD_SET(pConnection->fd1, &readfds);
      int n = pConnection->fd1 + 1;
      timeout.tv_sec = 2;
      timeout.tv_usec = 0;
      int re = select( n , &readfds , NULL , NULL , &timeout );

      if ( FD_ISSET( pConnection->fd1 , &readfds ) ){
	if ((pConnection->fd2 = accept(pConnection->fd1, (struct sockaddr *)&pConnection->caddr, &pConnection->len)) < 0) {
	  throw new NodeException(NULL, string("accept"), __FILE__, __LINE__);
	}
	int val = 1;
	ioctl(pConnection->fd2, FIONBIO, (char *)&val);    
	module_send_reject_flag_HarkParamsDynReconf = 0;
	init_send_flag_HarkParamsDynReconf = 1;
	printf("module_sd_connect : connected\n");
      }
    }
    usleep(2000000);
  }
  printf("module_sd_connect : thread terminated\n");
  pthread_exit(0);
}

class HarkParamsDynReconf;

DECLARE_NODE(HarkParamsDynReconf);
/*Node
 *
 * @name HarkParamsDynReconf
 * @category HARK:MISC
 * @description HarkParams dynamic reconfigure node. DO NOT PUT THIS NODE MORE THAN TWO IN THE SAME NETWORK FILE (since single process can generate only one dynamic reconfigure node).
 *
 * @parameter_name PORT
 * @parameter_type int
 * @parameter_value 9999
 * @parameter_description Port number of audio input server. 
 *
 * @parameter_name ENABLE_DEBUG
 * @parameter_type bool
 * @parameter_value false
 * @parameter_list true:false
 * @parameter_description print debug message of this module in case of true.
 *
 * @output_name LocalizeMUSIC
 * @output_type Vector<ObjectRef>
 * @output_description HarkParams of LocalizeMUSIC (ParamsLocalizeMUSIC)
 *
 * @output_name SourceTracker
 * @output_type Vector<ObjectRef>
 * @output_description HarkParams of SourceTracker (ParamsSourceTracker)
 *
 * @output_name HRLE
 * @output_type Vector<ObjectRef>
 * @output_description HarkParams of HRLE (ParamsHRLE)
 *
END*/

class HarkParamsDynReconf : public BufferedNode {
  int localizemusicID;
  int sourcetrackerID;
  int hrleID;

  int port;
  bool enable_debug;

  T_ConnectionData_HarkParamsDynReconf oConnectionData;

  struct sockaddr_in saddr;
  int    ret;
  char   buf[1024];
  int bufint[3];
  pthread_t thread;

public:
  HarkParamsDynReconf(string nodeName, ParameterSet params)
    : BufferedNode(nodeName, params)
  {
    localizemusicID    = addOutput("LocalizeMUSIC");
    sourcetrackerID    = addOutput("SourceTracker");
    hrleID             = addOutput("HRLE");

    port = dereference_cast<int>(parameters.get("PORT"));
    enable_debug = dereference_cast<bool>(parameters.get("ENABLE_DEBUG"));

    if ((oConnectionData.fd1 = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
      throw new NodeException(NULL, string("socket"), __FILE__, __LINE__);
    }

    bzero((char *)&saddr, sizeof(saddr));

    saddr.sin_family = AF_INET;
    saddr.sin_addr.s_addr = INADDR_ANY;
    saddr.sin_port = htons(port);
    
    int yes = 1;
    if (setsockopt(oConnectionData.fd1, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1) {
      perror("setsockopt");
      exit(1);
    }

    if (bind(oConnectionData.fd1, (struct sockaddr *)&saddr, sizeof(saddr)) < 0){
      throw new NodeException(NULL, string("bind"), __FILE__, __LINE__);
    }
    
    if (listen(oConnectionData.fd1, 1) < 0) {
      throw new NodeException(NULL, string("listen"), __FILE__, __LINE__);
    }

#if 0
    oConnectionData.len = sizeof(oConnectionData.caddr);
    if ((oConnectionData.fd2 = accept(oConnectionData.fd1, (struct sockaddr *)&oConnectionData.caddr, &oConnectionData.len)) < 0) {
      throw new NodeException(NULL, string("accept"), __FILE__, __LINE__);
    }
    // Non blocking mode
    int val = 1;
    ioctl(oConnectionData.fd2, FIONBIO, (char *)&val);    
#endif

    init_send_flag_HarkParamsDynReconf = 0;
    module_send_reject_flag_HarkParamsDynReconf = 1;
    inOrder = true;  
  }

  ~HarkParamsDynReconf()
  {
#if 1
    module_sd_connect_flag_HarkParamsDynReconf = 0;
    pthread_join(thread, NULL);
#endif
    close(oConnectionData.fd1);
    close(oConnectionData.fd2);
  }

  virtual void initialize()
  {
    cout << getName() << " initialized..." << endl;
    this->BufferedNode::initialize();
#if 1
    module_sd_connect_flag_HarkParamsDynReconf = 1;  
    pthread_create(&thread,NULL,module_sd_connect_HarkParamsDynReconf,(void*)&oConnectionData);
#endif
  }

  void calculate(int output_id, int count, Buffer &out)
  {
    
    // output settings
    
    Vector<ObjectRef>& out_LocalizeMUSIC = *new Vector<ObjectRef>;
    (*(outputs[localizemusicID].buffer))[count] = &out_LocalizeMUSIC;

    Vector<ObjectRef>& out_SourceTracker = *new Vector<ObjectRef>;
    (*(outputs[sourcetrackerID].buffer))[count] = &out_SourceTracker;

    Vector<ObjectRef>& out_HRLE = *new Vector<ObjectRef>;
    (*(outputs[hrleID].buffer))[count] = &out_HRLE;

    // socket comm.

    int vsize = 12;
    float buff[vsize];
    
    //if(module_send_reject_flag_HarkParamsDynReconf == 1) return;
   
    int ret = recv(oConnectionData.fd2, &buff, vsize * sizeof(float), 0);
    
    if(ret < 1){
      // return;
      // if(errno == EAGAIN){
      // 	return;
      // }else{
      // 	// perror("recv");
      // }
    }else{

      RCPtr<ParamsLocalizeMUSIC> value_LocalizeMUSIC(new ParamsLocalizeMUSIC());
      RCPtr<ParamsSourceTracker> value_SourceTracker(new ParamsSourceTracker());
      RCPtr<ParamsHRLE> value_HRLE(new ParamsHRLE());

      if(enable_debug) cout << "<Received[" << ret << "] : ";
      for (int i = 0; i < vsize; i++)
	if(enable_debug) cout << buff[i] << ", ";
      if(enable_debug) cout << ">" << endl;

      value_LocalizeMUSIC->num_source            = (int)buff[0];
      value_LocalizeMUSIC->min_deg               = (int)buff[1];
      value_LocalizeMUSIC->max_deg               = (int)buff[2];
      value_LocalizeMUSIC->lower_bound_frequency = (int)buff[3];
      value_LocalizeMUSIC->upper_bound_frequency = (int)buff[4];
      value_SourceTracker->thresh                = (float)buff[5];
      value_SourceTracker->pause_length          = (float)buff[6];
      value_SourceTracker->min_src_interval      = (float)buff[7];
      value_SourceTracker->min_tfindex_interval  = (float)buff[8];
      value_SourceTracker->compare_mode          = (Source::CompareMode)buff[9];
      value_HRLE->lx                             = (float)buff[10];
      value_HRLE->time_constant                  = (int)buff[11];
      
      out_LocalizeMUSIC.push_back(value_LocalizeMUSIC);
      out_SourceTracker.push_back(value_SourceTracker);
      out_HRLE.push_back(value_HRLE);

    }    

  }
};

