#include <unistd.h>
#include <iostream>
using namespace std;
int main(){
    pid_t pid = fork();
    if(pid == 0){
        char *const paramList[] = {"5"," 1", NULL};
        execvp("TestApplication", paramList);
    }else{
        int status;
        waitpid(pid, &status,0);
    }
}