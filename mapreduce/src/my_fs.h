#ifndef MY_FS__
#define MY_FS__
#include <string>

using namespace std;
int upload(string src_filename, dest_filename);
int split(string src_filename, int num_chunk); //qinnan random~qinnan.1 , qinnan.
int download(string src_filename, string dest_filename);
#endif // MY_FS__
