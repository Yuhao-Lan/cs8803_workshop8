#include <iostream>
#include <memory>
#include <string>
#include <cstddef>
#include <grpc++/grpc++.h>
#include <glog/logging.h>
#include <glog/raw_logging.h>
#include "rpc_generated/master-worker.grpc.pb.h"
#include "my_fs.h"
using grpc::Channel;
using grpc::ClientContext;
using grpc::Status;
using masterworker::Filename;
using masterworker::Filenames;
using masterworker::Worker;
using namespace std;
class MasterClient {
 public:
  MasterClient(std::shared_ptr<Channel> channel)
      : stub_(Worker::NewStub(channel)) {}

  // Assambles the client's payload, sends it and presents the response back
  // from the server.
  std::string StartMapper(const std::string& str_filename) {
    // Data we are sending to the server.
    Filename filename;
    filename.set_filename(str_filename);

    // Container for the data we expect from the server.
    Filename return_filename;

    // Context for the client. It could be used to convey extra information to
    // the server and/or tweak certain RPC behaviors.
    ClientContext context;

    // The actual RPC.
    Status status = stub_->StartMapper(&context, filename, &return_filename);

    // Act upon its status.
    if (status.ok()) {
      return return_filename.filename();
    } else {
      std::cout << status.error_code() << ": " << status.error_message() << std::endl;
      return "RPC failed";
    }
  }

 private:
  std::unique_ptr<Worker::Stub> stub_;
};

int main(int argc, char** argv) {
  
  if(argc != 2){
    cout << "Invlid arguments " << endl;
  }
  // upload input file to blob
  string str(argv[1]);
  size_t found = str.find_last_of("/");
  string blob_filename;
  if(found == string::nops) {
    blob_filename = str;
  }else{
    blob_filename = str.substr(found+1);
  }
  
  LOG(INFO) << "Master is uploading input file: " <<  str << " to blob " << blob_filename;
  upload(str, blob_filename);
  // split input file into N chunks
  LOG(INFO) << "Master is splitting blob file: " << blob_filename;
  int num_chunk = split(str, 1024);
  LOG(INFO) << "Master splitted blob file: " << blob_filename << " into " << num_chunk << " chunk";
  // create M clients, where M is the number of worker nodes
  // start N pthreads, each thread selects a client based on round robin, and then calls cli.startmapper();
  // wait all N pthreds to finish, and start reducers
  
  //MasterClient cli(grpc::CreateChannel("map-reduce-node-1:50051", grpc::InsecureChannelCredentials()));
  //std::string input_filename("world.txt");
  //std::string output_filename = cli.StartMapper(input_filename);
  //std::cout << "Worker received: " << output_filename << std::endl;

  return 0;
}