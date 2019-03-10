#include <iostream>
#include <string>
#include <grpc++/grpc++.h>
#include <glog/logging.h>
#include <glog/raw_logging.h>
#include <thread>
#include <mutex>
#include "master-client-utilities.h"
#include "my_fs.h"
using namespace std;
int next_client = 0;
mutex next_client_mtx;
//vector<WorkerStruct> vct;
vector<string> vct;
mutex vct_mtx;
void start_mapper(string file_chunk){
  //
  int local_client_id = 0;
  next_client_mtx.lock();
  local_client_id = next_client;
  next_client ++;
  if(next_client >= vct.size()){
    next_client = 0;
  }
  next_client_mtx.unlock();
  vct_mtx.lock();
  string worker_hostname = vct[local_client_id];
  vct_mtx.unlock();
  MasterClient cli(grpc::CreateChannel(worker_hostname + ":50051", grpc::InsecureChannelCredentials()));
  LOG(INFO) << "StartMapper: " << file_chunk << ". Using worker node: " << worker_hostname; 
  string output_file = cli.StartMapper(file_chunk);
  LOG(INFO) << "StartMapper: " << file_chunk << " done with output file: " << output_file; 
}
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

  
  create_client_handles(&vct);
  
  // start N pthreads, each thread selects a client based on round robin, and then calls cli.startmapper();
  thread * mapper_thread = new thread[num_chunk];
  for(int i = 0; i < num_chunk; i++){
    mapper_thread[i] = thread(start_mapper, blob_filename + "." + to_string(i));
  }
  for(int i = 0; i < num_chunk; i++){
    mapper_thread[i].join();
  }
  cout << "All done " << endl;
  // wait all N pthreds to finish, and start reducers
  
  //MasterClient cli(grpc::CreateChannel("map-reduce-node-1:50051", grpc::InsecureChannelCredentials()));
  //std::string input_filename("world.txt");
  //std::string output_filename = cli.StartMapper(input_filename);
  //std::cout << "Worker received: " << output_filename << std::endl;

  return 0;
}