#include <was/storage_account.h>
#include <was/blob.h>
#include <cpprest/filestream.h>  
#include <cpprest/containerstream.h>
#include <fstream>
#include <stdio.h>
#include "my_fs.h"
//#include "my_fs_stream.h"
using namespace std;

static string connect_string = "DefaultEndpointsProtocol=https;AccountName=mapreducestorage123;AccountKey=ofq5oXyrljPqpD/0eNOqAI47BhwXEdgDEPPEKJOKI/yK+hi/6Bi79Z5zr/8CXb3qK/9PnPSFVbb9hn9DTt2jhg==;EndpointSuffix=core.windows.net";
static string container_name = "mapreduce";
int upload(string src_filename, string dest_filename){
    // Define the connection-string with your values.
    const utility::string_t storage_connection_string(U(connect_string));
    // Retrieve storage account from connection string.
    azure::storage::cloud_storage_account storage_account = azure::storage::cloud_storage_account::parse(storage_connection_string);
    // Create the blob client.
    azure::storage::cloud_blob_client blob_client = storage_account.create_cloud_blob_client();
    // Retrieve a reference to a container.
    azure::storage::cloud_blob_container container = blob_client.get_container_reference(U(container_name));
    // Retrieve reference to a blob
    azure::storage::cloud_block_blob blockBlob = container.get_block_blob_reference(U(dest_filename));
    // Create or overwrite the blob with contents from a local file.
    concurrency::streams::istream input_stream = concurrency::streams::file_stream<uint8_t>::open_istream(U(src_filename)).get();
    blockBlob.upload_from_stream(input_stream);
    input_stream.close().wait();
    return 0;
}

int download(string src_filename, string dest_filename){
     // Define the connection-string with your values.
    const utility::string_t storage_connection_string(U(connect_string));
    // Retrieve storage account from connection string.
    azure::storage::cloud_storage_account storage_account = azure::storage::cloud_storage_account::parse(storage_connection_string);
    // Create the blob client.
    azure::storage::cloud_blob_client blob_client = storage_account.create_cloud_blob_client();
    // Retrieve a reference to a previously created container.
    azure::storage::cloud_blob_container container = blob_client.get_container_reference(U(container_name));
    // Retrieve reference to a blob name.
    azure::storage::cloud_block_blob blockBlob = container.get_block_blob_reference(U(src_filename)); 
    blockBlob.download_to_file(dest_filename);
}

static string prefix = "qoeijnfughregurhgreg.";
int split(string filename, int chunk_size_kb){
    // Define the connection-string with your values.
    const utility::string_t storage_connection_string(U(connect_string));
    // Retrieve storage account from connection string.
    azure::storage::cloud_storage_account storage_account = azure::storage::cloud_storage_account::parse(storage_connection_string);
    // Create the blob client.
    azure::storage::cloud_blob_client blob_client = storage_account.create_cloud_blob_client();
    // Retrieve a reference to a previously created container.
    azure::storage::cloud_blob_container container = blob_client.get_container_reference(U(container_name));
    // Retrieve reference to a blob name.
    azure::storage::cloud_block_blob blockBlob = container.get_block_blob_reference(U(filename)); 
    blockBlob.download_to_file(prefix + filename);
    //cout << "downloaded" << endl;
    // split the local file
    azure::storage::cloud_block_blob split_blob;
    ifstream file(prefix + filename);
    ofstream * out = new ofstream(prefix + filename + ".temp");
    string str; 
    int file_counter = 1;
    int byte_counter = 0;
    int flag = 0;
    while (getline(file, str))
    {
        if (flag == 0){
            byte_counter = str.length();
            *out << str;
            flag = 1;
            continue;
        }
        if(byte_counter + str.length() < chunk_size_kb * 1024){
            //store local temp
            byte_counter += str.length();
            *out << str;
            flag = 2;
        }else{
            //upload the old one and start a new local file
            //cout << "uploading " << file_counter << endl;
            azure::storage::cloud_block_blob split_blob = container.get_block_blob_reference(U(filename + "." + to_string(file_counter)));
            split_blob.upload_from_file(prefix + filename + ".temp");
            byte_counter = str.length();
            file_counter++;
            out = new ofstream(prefix + filename + ".temp");
            *out << str;
            flag = 3;
            //cout << "done uploading " << file_counter << endl;
        }

    }
     //upload
    azure::storage::cloud_block_blob split_blob2 = container.get_block_blob_reference(U(filename + "." + to_string(file_counter)));
    split_blob2.upload_from_file(prefix + filename + ".temp");
    // remove the local temp file
    string last_temp_file = prefix + filename;
    remove(last_temp_file.c_str());
    last_temp_file = prefix + filename + ".temp";
    remove(last_temp_file.c_str());
    return file_counter;

}
