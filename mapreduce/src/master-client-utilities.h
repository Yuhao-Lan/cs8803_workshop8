#ifndef MASTER_CLIENT_U__
#define MASTER_CLIENT_U__

#include <iostream>
#include <memory>
#include <string>
#include <cstddef>
#include <vector>
#include <grpc++/grpc++.h>

#include "rpc_generated/master-worker.grpc.pb.h"

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

  
  std::string StartMapper(const std::string& str_filename) {
  
    Filename filename;
    filename.set_filename(str_filename);
    Filename return_filename;
    ClientContext context;
    Status status = stub_->StartMapper(&context, filename, &return_filename);
    if (status.ok()) {
      return return_filename.filename();
    } else {
      std::cout << status.error_code() << ": " << status.error_message() << std::endl;
      return "RPC failed";
    }
  }
  std::string StartReducer(const std::string& str_filenames) {
  
    Filenames filenames;
    filenames.set_filenames(str_filenames);
    Filename return_filename;
    ClientContext context;
    Status status = stub_->StartReducer(&context, filenames, &return_filename);
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

/*struct WorkerStruct{
    string hostname;
    MasterClient * handle;
};

int create_client_handles(vector<WorkerStruct> * vct);*/
int create_client_handles(vector<string> * vct);

#endif // MASTER_CLIENT_U__