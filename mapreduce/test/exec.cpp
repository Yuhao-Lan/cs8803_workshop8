#include <unistd.h>
#include <iostream>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
using namespace std;
#define CMD_LEN 3


int main(){
    pid_t pid = fork();
    string filename = "test.txt";
    int out_fd = open(filename.c_str(), O_RDWR|O_CREAT, 0666);
    int in_fd = open("input.txt", O_RDONLY);
    if(pid == 0){
        
        //string cmd [CMD_LEN] = {"python", "test.py", NULL};
        //char * const * cmd_ = new char* [CMD_LEN];
        const char * loc = "./mapper.py";
        /*for(int i = 0 ; i < CMD_LEN-1 ; i++){
            cmd_[i] = cmd[i].c_str();
        }
        cmd_[CMD_LEN - 1] = NULL;*/
        dup2(in_fd, 0);
        dup2(out_fd, 1);
        char * const cmd[] = {"./mapper.py", nullptr};
        execvp(loc, cmd);
        
    }else{
        int status;
        waitpid(pid, &status,0);
        cout << "Child done!" << endl;
        close(out_fd);
        close(in_fd);
    }
}