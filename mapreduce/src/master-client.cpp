#include <iostream>
#include <string>
#include <grpc++/grpc++.h>
#include <glog/logging.h>
#include <glog/raw_logging.h>
#include "master-client-utilities.h"
#include "my_fs.h"
int next_client = 0;
int main(int argc, char** argv) {
  /*
  * 0 = program self
  * 1 = input file
  */
  if(argc != 2){
    cout << "Invlid arguments " << endl;
  }
  // upload input file to blob
  string str(argv[1]);
  //string workers(argv[2]);
  size_t found = str.find_last_of("/");
  string blob_filename;
  if(found == string::npos) {
    blob_filename = str;
  }else{
    blob_filename = str.substr(found+1);
  }
  
  LOG(INFO) << "Master is uploading input file: " <<  str << " to blob " << blob_filename;
  upload(str, blob_filename);
  // split input file into N chunks
  LOG(INFO) << "Master is splitting blob file: " << blob_filename;
  int num_chunk = split(blob_filename, 1024);
  LOG(INFO) << "Master splitted blob file: " << blob_filename << " into " << num_chunk << " chunk";
  // create M clients, where M is the number of worker nodes, ask from zookeeper

  vector<WorkerStruct> vct;
  create_client_handles(&vct);
  WorkerStruct temp = vct[0];
  LOG(INFO) << "StartMapper: " << blob_filename << ".1"; 
  temp.handle->StartMapper(blob_filename + ".1");
  // start N pthreads, each thread selects a client based on round robin, and then calls cli.startmapper();
  /*for(int i = 0; i < num_chunk; i++){

  }*/
  // wait all N pthreds to finish, and start reducers
  
  //MasterClient cli(grpc::CreateChannel("map-reduce-node-1:50051", grpc::InsecureChannelCredentials()));
  //std::string input_filename("world.txt");
  //std::string output_filename = cli.StartMapper(input_filename);
  //std::cout << "Worker received: " << output_filename << std::endl;

  return 0;
}