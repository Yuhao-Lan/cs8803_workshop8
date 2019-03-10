
number_node = 5
number_master = 2
node_IP_start = 10 # e.g. 10.0.0.10
resource_group_name = "nqin8-resource-group"
image_name = "nqin8-workshop8-image"
admin_name = "nan"
ssh_public_key_location = "~/.ssh/id_rsa.pub"
vm_size = "Standard_B1s"
location = "eastus"
hostname_prefix = "map-reduce-node-"
private_ip_prefix = "10.0.0."

input_file = "./input_files/big.txt"
master_binaries = "./binaries/master"
worker_binaries = "./binaries/worker"

dest = "~/mapreduce"
master_binary_name = "master"
worker_binary_name = "worker"
input_file_name = "big.txt"

mapper_code = "./application_code/mapper.py"
reducer_code = "./application_code/reducer.py"