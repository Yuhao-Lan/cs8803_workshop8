#include <iostream>
#include <string>
#include <map>
#include <vector>
#include <grpc++/grpc++.h>
#include <glog/logging.h>
#include <glog/raw_logging.h>
#include <thread>
#include <mutex>
#include "master-client-utilities.h"
#include "my_fs.h"
#include <chrono>
#include <conservator/ConservatorFrameworkFactory.h>
#include <zookeeper/zookeeper.h>
using namespace std;
//////// declaration ///////////
void ping_worker_node(string hostname);
void add_worker(string worker_hostname);
void delete_worker(string worker_hostname);


unique_ptr<ConservatorFramework> framework;
//////////////////////// Vector keeps track of workers /////////////////

static int __counter = 0;
vector<string> __vct;
map<string, thread *> ping_threads; 
mutex __vct_mtx;

void ping_worker_node(string hostname){
    LOG(INFO) << "....Ping." << hostname; 
    int count = 0;
    while(1){
        MasterClient cli(grpc::CreateChannel(hostname + ":50051", grpc::InsecureChannelCredentials()));
        this_thread::sleep_for(chrono::seconds(2));
        if(cli.Ping()){
          count = 0;
        }else{
          count++;
        }
        // retry 5 times, if all failed...
        if(count > 5){
          LOG(INFO) << "Ping." << hostname << " Failed....";
           delete_worker(hostname);
          return;
        }
    }
}
/* Add worker */
void add_worker(string worker_hostname){
  __vct_mtx.lock();
  __vct.push_back(worker_hostname);
  thread * ping_thread = new thread(ping_worker_node, worker_hostname);
  ping_threads.insert(pair<string, thread *>(worker_hostname, ping_thread));
  __vct_mtx.unlock();
}
/* Delete worker */
void delete_worker(string worker_hostname){
  __vct_mtx.lock();
  vector<string>::iterator it = find(__vct.begin(), __vct.end(), worker_hostname);
  if(it != __vct.end()){  
    __vct.erase(it);
    ping_threads.erase(worker_hostname);
  }
  __vct_mtx.unlock();
}

/* For each worker, start a pthread to do ping */
void update_worker(vector<string> *worker_hostnames){
  vector<string> new_workers;
  vector<string> removed_workers;
  __vct_mtx.lock();
  for (string& hostname : *worker_hostnames) {
    vector<string>::iterator it = find(__vct.begin(), __vct.end(), hostname);
    if(it != __vct.end()){  
      // already exist
    }else{
      new_workers.push_back(hostname);
    }
  }
  for (string& hostname : __vct) {
    vector<string>::iterator it = find(worker_hostnames->begin(), worker_hostnames->end(), hostname);
    if(it != worker_hostnames->end()){  
      // already exist
    }else{
      removed_workers.push_back(hostname);
    }
  }
  __vct_mtx.unlock();

  for(string &hostname: new_workers){
    // cout << "Add ::: new ::: " << hostname << endl;
    add_worker(hostname);
  }
  for(string &hostname: removed_workers){
    // cout << "Delete ::: old ::: " << hostname << endl;
    delete_worker(hostname);
  }
}

string get_next_worker_hostname(){
    __vct_mtx.lock();
    if(__counter >= __vct.size()){
      __counter = 0;
    }
    string __hostname = __vct[__counter];
    __counter ++;
    __vct_mtx.unlock();
    return __hostname;
}

//////////////// watch worker updated ///////////////////////////////
void worker_update_fn(zhandle_t *zh, int type,
                    int state, const char *path,void *watcherCtx) {
    cout << "new worker updated" << endl;
    cout << type << state << endl;
    // update vector

    //ConservatorFramework* framework = (ConservatorFramework *) watcherCtx;
    //vector<string> workers = framework->getChildren()->withWatcher(worker_update_fn, framework)->forPath("/worker");
    
    /*ConservatorFrameworkFactory factory = ConservatorFrameworkFactory();
    unique_ptr<ConservatorFramework> framework = factory.newClient("cli-node:2181");
    framework->start();*/
    vector<string> worker_hostnames = framework->getChildren()->withWatcher(worker_update_fn, &framework)->forPath("/worker");
    update_worker( &worker_hostnames );
    
}

///////////////// watch leader /////////////////////////
void watch_leader(zhandle_t *zh, int type,
                             int state, const char *path,void *watcherCtx) {
    leader_election(); // re-run leader election
}
////////////////// job status //////////////////////////
void job_status_watcher_fn(zhandle_t *zh, int type,
                       int state, const char *path,void *watcherCtx) {
    follower_listen(); // re-run listen
}

////////////////////////// Mapper /////////////////////


void start_mapper(string file_chunk){
  string output_file = file_chunk + ".map";
  // check if the output_file existed in jobdata znode, if does, then done



  string worker_hostname = "";
  //
  while(1){
    worker_hostname = get_next_worker_hostname();
    //next_client_mtx.unlock();
    MasterClient cli(grpc::CreateChannel(worker_hostname + ":50051", grpc::InsecureChannelCredentials()));
    LOG(INFO) << ".....StartMapper: " << file_chunk << ". Using worker node: " << worker_hostname; 
    output_file = cli.StartMapper(file_chunk);
    if(output_file != "RPC failed"){
      break;
    }else{
      cout << "failed with worker: " << worker_hostname << ", retry " << file_chunk << endl;
      this_thread::sleep_for(chrono::seconds(2));
    }
  }
  LOG(INFO) << worker_hostname << ".StartMapper(" << file_chunk << ") => " << output_file; 

}


/////////////////////// Reducer //////////////////////////////

void start_reducer(string filenames){
  string output_file = "";
  string worker_hostname = "";

  while(1){
    worker_hostname = get_next_worker_hostname();
    //next_client_mtx.unlock();
    MasterClient cli(grpc::CreateChannel(worker_hostname + ":50051", grpc::InsecureChannelCredentials()));
    LOG(INFO) << ".....StartMapper. Using worker node: " << worker_hostname; 
    output_file = cli.StartReducer(filenames);
    if(output_file != "RPC failed"){
      break;
    }
  }
  LOG(INFO) << worker_hostname << ".StartReducer(" << filenames << ") => " << output_file; 
}

///////////////////////// Leader ///////////////////////////////

int is_leader = 0;
string job_status = "init"; 
string job_data = "";
int num_chunk = 0;


string job_status_convert_to_string(int job_status){
  switch(job_status){
    case 0: return "init";
    case 1: return "uploaded";
    case 2: return "splitted";
    case 3: return "mapping";
    case 4: return "mapped";
    case 5: return "reduced";
    default: return "none";
  }
}

int job_status_convert_to_int(string job_status){
  if(job_status == "init"){
    return 0;
  }else if(job_status == "uploaded"){
    return 1;
  }else if(job_status == "splitted"){
    return 2;
  }else if(job_status == "mapping"){
    return 3;
  }else if(job_status == "mapped"){
    return 4;
  }else if(job_status == "reduced"){
    return 5;
  }
  return -1;
}



string blob_filename;
string local_filename;


void start_leader(){
  LOG(INFO) << "Main.watch_workers.ping ....";
  vector<string> worker_hostnames = framework->getChildren()->withWatcher(worker_update_fn, &framework)->forPath("/worker");
  update_worker(&worker_hostnames);

  switch(job_status_convert_to_int(job_status)){
    case 0:{
      // init
      LOG(INFO) << "Leader.upload(" <<  local_filename << "): " << blob_filename;
      upload(local_filename, blob_filename);
      string __job_status = "uploaded";
      framework->setData()->forPath("/jobstatus", __job_status.c_str());
      job_status = __job_status;
    };
    case 1:{
      // uploaded
      LOG(INFO) << "Leader.split(" << blob_filename << ")";
      num_chunk = split(blob_filename, 1024);
      string __job_status = "splitted";
      string __job_data = to_string(num_chunk);
      framework->setData()->forPath("/jobstatus", __job_status.c_str());
      framework->setData()->forPath("/jobdata", __job_data.c_str());
      job_status = __job_status;
      job_data = __job_data;
    }
    case 2:
    case 3:{
      // splitted, mapping
      LOG(INFO) << "Leader.startMapper";
      string __job_status = "mapping";
      string __job_data = "";
      framework->setData()->forPath("/jobstatus", __job_status.c_str());
      framework->setData()->forPath("/jobdata", __job_data.c_str());
      job_status = __job_status;
      job_data = __job_data;
      // start N pthreads, each thread selects a client based on round robin, and then calls cli.startmapper();
      string __num_chunk = framework->getData()->forPath("/jobdata");
      num_chunk = stoi(__num_chunk);
      thread * mapper_thread = new thread[num_chunk];
      for(int i = 0; i < num_chunk; i++){
        mapper_thread[i] = thread(start_mapper, blob_filename + "." + to_string(i+1));
      }
      // wait all N pthreds to finish, and start reducers
      for(int i = 0; i < num_chunk; i++){
        mapper_thread[i].join();
      }
      string __job_status = "mapped";
      framework->setData()->forPath("/jobstatus", __job_status.c_str());
      job_status = __job_status;
    }
    case 4:{
      // mapped
      string mappers_outputs = framework->getData()->forPath("/jobdata");
      LOG(INFO) << "Leader.startReducer(" << mappers_outputs << ")";
      start_reducer(mappers_outputs);
      string __job_status = "reduced";
      string __job_data = "";
      framework->setData()->forPath("/jobstatus", __job_status.c_str());
      framework->setData()->forPath("/jobdata", __job_data.c_str());
      job_status = __job_status;
      job_data = __job_data;
    }
  }
}

////////////////////////// Leader Election /////////////////////


void leader_election(){
  LOG(INFO) << "Main.leader_election ....";
  // try to create parent directory /master and node
  char cstr_hostname[HOSTNAME_MAX_LEN];
  if(gethostname(cstr_hostname, HOSTNAME_MAX_LEN) != 0){
    LOG(INFO) << << "Error: Cannot get hostname";
    return 0;
  }
  string hostname = string(cstr_hostname);
  framework->create()->forPath("/master", (char *) "master-nodes");
  string nodename = "/master/leader";
  if(framework->create()->withFlags(ZOO_EPHEMERAL)->forPath(nodename, hostname.c_str()) != 0){
    LOG(INFO) << "Main." << hostname << ".Acting as leader ...";
    is_leader = 1;
    start_leader();
  }else{
    LOG(INFO) << "Main." << hostname << ".Acting as follower ...";
    is_leader = 0;
    // watch leader
    framework->checkExists()->withWatcher(watch_leader, framework)->forPath("/master/leader");
    return;
  }
}

void follower_listen(){
  if(is_leader == 1){
    return;
  }
  job_status = framework->getData()->withWatcher(job_status_watcher_fn, framework)->forPath("/jobstatus");
}


int main(int argc, char** argv) {
   //0 = program self,  1 = input file
    if(argc != 2){
      LOG(INFO) << "Main.Failed.Invalid Input Arguments ....";
      return;
    }
    // upload input file to blob
    string str(argv[1]);
    //string workers(argv[2]);
    size_t found = str.find_last_of("/");
    
    if(found == string::npos) {
      blob_filename = str;
    }else{
      blob_filename = str.substr(found+1);
    }
    local_filename = str;

    // connect to local zookeeper server
    ConservatorFrameworkFactory factory = ConservatorFrameworkFactory();
    framework = factory.newClient("cli-node:2181");
    framework->start();
    framework->create()->forPath("/master", (char *) "master-nodes");
    framework->create()->forPath("/jobstatus", job_status.c_str());
    framework->create()->forPath("/jobdata", job_data.c_str());
    leader_election();
    follower_listen();
    return 0;
}