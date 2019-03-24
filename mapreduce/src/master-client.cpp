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


////////////////////////// Mapper /////////////////////

string mappers_outputs = "";
mutex mappers_outputs_mtx;

void start_mapper(string file_chunk){
  string output_file = "";
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
  mappers_outputs_mtx.lock();
  if(mappers_outputs == ""){
    mappers_outputs = output_file;
  }else{
    mappers_outputs = mappers_outputs + ";" + output_file;
  }
  mappers_outputs_mtx.unlock();
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


int main(int argc, char** argv) {
  // connect to local zookeeper server
  ConservatorFrameworkFactory factory = ConservatorFrameworkFactory();
  framework = factory.newClient("cli-node:2181");
  framework->start();
  //framework->close();
  // init workers, master watch workers
  vector<string> worker_hostnames = framework->getChildren()->withWatcher(worker_update_fn, &framework)->forPath("/worker");
  update_worker(&worker_hostnames);
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


  /*
  // start N pthreads, each thread selects a client based on round robin, and then calls cli.startmapper();
  thread * mapper_thread = new thread[num_chunk];
  for(int i = 0; i < num_chunk; i++){
    mapper_thread[i] = thread(start_mapper, blob_filename + "." + to_string(i+1));
  }
  // wait all N pthreds to finish, and start reducers
  for(int i = 0; i < num_chunk; i++){
    mapper_thread[i].join();
  }

  start_reducer(mappers_outputs);
  cout << "All done " << endl;
  
  
  //MasterClient cli(grpc::CreateChannel("map-reduce-node-1:50051", grpc::InsecureChannelCredentials()));
  //std::string input_filename("world.txt");
  //std::string output_filename = cli.StartMapper(input_filename);
  //std::cout << "Worker received: " << output_filename << std::endl;*/

  return 0;
}