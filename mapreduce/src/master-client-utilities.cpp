#include "master-client-utilities.h"
#include <glog/logging.h>
#include <glog/raw_logging.h>

using grpc::Channel;
using grpc::ClientContext;
using grpc::Status;
using namespace std;
int create_client_handles(vector<WorkerStruct> * vct){
    // right now, it is hard code. But later on, it will use zookeeper.
    WorkerStruct wk1; // , wk2, wk3;
    wk1.hostname = "map-reduce-node-3";
    wk1.handle = new MasterClient(grpc::CreateChannel("map-reduce-node-3:50051", grpc::InsecureChannelCredentials()));
    vct->push_back(wk1);
}