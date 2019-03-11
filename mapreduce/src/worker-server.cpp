#include <iostream>
#include <memory>
#include <string>
#include <grpc++/grpc++.h>
#include <string>
#include <glog/logging.h>
#include <glog/raw_logging.h>
#include "rpc_generated/master-worker.grpc.pb.h"
#include "my_fs.h"
#include <sys/types.h> 
#include <sys/wait.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#define HOSTNAME_MAX_LEN 128

using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::Status;
using masterworker::Filenames;
using masterworker::Filename;
using masterworker::Worker;
string hostname = "";
// Logic and data behind the server's behavior.
class WorkerServiceImpl final : public Worker::Service {

  Status StartMapper(ServerContext* context, 
    const Filename* request, Filename* response) override {
        LOG(INFO) << hostname << ".Mapper(" <<  request->filename() << ")";
        //download 
        string filename = request->filename();
        download(filename, filename);
        //exec
        //cat ../input_files/big.txt | ./mapper.py | sort | ./reducer.py
        string map_output_file = filename + ".map";
        int out_fd = open(map_output_file.c_str(), O_RDWR|O_CREAT, 0666);
        int in_fd = open(filename.c_str(), O_RDONLY);

        pid_t pid = fork();
        if(pid == -1)
        {
          LOG(WARNING) << hostname << ".Mapper(" <<  request->filename() << ") failed";
          return Status::CANCELLED;
        }
        else if (pid == 0)
        {
          dup2(in_fd, 0);
          dup2(out_fd, 1);
          //const char * loc = "~/mapreduce/mapper.py";
          //char * const cmd[] = {"./mapper.py", nullptr};
          const char * loc = "/home/nan/mapreduce";
          char * const cmd[] = {"mapper.py", nullptr};
          execvp(loc, cmd);
        }else{
          int status;
          waitpid(pid, &status,0);
        }
        
        //upload
        upload(map_output_file, map_output_file);
        response->set_filename(map_output_file);
        LOG(INFO) << "The mapper is done with output file: " << map_output_file;
        close(out_fd);
        close(in_fd);
        return Status::OK;
  }
  Status StartReducer(ServerContext* context, 
    const Filenames* request, Filename* response) override {
        LOG(INFO) << "A reducer is running with input files: " <<  request->filenames();
        // download all the mappers' output files
        // exec sort, [partition]
        // exec("cat filename | python reduce.py > output.txt");
        // upload(output.txt);
        response->set_filename("Reducer,Hello " + request->filenames());
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
  hostname = string(cstr_hostname);

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