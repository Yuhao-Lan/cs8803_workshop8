#ifndef MY_FS__
#define MY_FS__
#include <string>

using namespace std;
int upload(string filename);
int split(string filename, int num_chunk); //qinnan random~qinnan.1 , qinnan.
int download(string filename);
#endif // MY_FS__
