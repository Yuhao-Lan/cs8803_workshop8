#include <iostream>
#include <memory>
#include <string>
#include <grpc++/grpc++.h>
#include <string>
#include <glog/logging.h>
#include <glog/raw_logging.h>
#include "rpc_generated/master-worker.grpc.pb.h"
#include "my_fs.h"

#define HOSTNAME_MAX_LEN 128

using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::Status;
using masterworker::Filenames;
using masterworker::Filename;
using masterworker::Worker;

// Logic and data behind the server's behavior.
class WorkerServiceImpl final : public Worker::Service {

  Status StartMapper(ServerContext* context, 
    const Filename* request, Filename* response) override {
        //LOG(INFO) << "A mapper is running with input file: " <<  request->filename();
        //download 
        //download(request->filename());
        //exec 
        //upload
        response->set_filename("Hello " + request->filename());
        //LOG(INFO) << "The mapper is done with output file: ";
        return Status::OK;
  }
  Status StartReducer(ServerContext* context, 
    const Filenames* request, Filename* response) override {
        LOG(INFO) << "A reducer is running with input files: " <<  request->filenames();
        // download all the mappers' output files
        // exec sort, [partition]
        // exec("cat filename | python reduce.py > output.txt");
        // upload(output.txt);
        response->set_filename("Hello " + request->filenames());
        LOG(INFO) << "The reducer is done with output file: ";
        return Status::OK;
  }
};

void RunServer() {
   char cstr_hostname[HOSTNAME_MAX_LEN];
   if(gethostname(cstr_hostname, HOSTNAME_MAX_LEN) != 0){
      cout << "Error: Cannot get hostname" << endl;
      return;
  }
  string hostname = string(cstr_hostname);

  std::string server_address("0.0.0.0:50051");
  WorkerServiceImpl service;

  ServerBuilder builder;
  // Listen on the given address without any authentication mechanism.
  builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
  // Register "service" as the instance through which we'll communicate with
  // clients. In this case it corresponds to an *synchronous* service.
  builder.RegisterService(&service);
  // Finally assemble the server.
  std::unique_ptr<Server> server(builder.BuildAndStart());
  LOG(INFO) << "Worker (" << hostname << ") is listening on " << server_address;

  // Wait for the server to shutdown. Note that some other thread must be
  // responsible for shutting down the server for this call to ever return.
  server->Wait();
}

int main(int argc, char** argv) {
  // register with zookeeper


  // run server
  RunServer();
  return 0;
}