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

struct WorkerStruct{
    string hostname;
    MasterClient * handle;
};

int create_client_handles(vector<WorkerStruct> * vct);

#endif // MASTER_CLIENT_U__