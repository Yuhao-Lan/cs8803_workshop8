import subprocess
from user_parameters import *

#mapreduce_node_hostnames should comes from create_vm. 
mapreduce_node_hostnames = []
for counter in range(1, number_node + 1):
    mapreduce_node_hostnames.append(hostname_prefix + str(counter))
#master hostnames and worker hostnames
masters = []
workers = []
for i in range(0, number_master):
    print("Master node: ", mapreduce_node_hostnames[i])
    masters.append(mapreduce_node_hostnames[i])
for i in range(number_master, number_node):
    print("Worker node: ", mapreduce_node_hostnames[i])
    workers.append(mapreduce_node_hostnames[i])


print("start workers")
worker_tasks = []
for worker in workers:
    print("start worker: ", worker)
    #ssh

master_tasks = []
for master_hostname in masters:
    master_tasks.append(subprocess.Popen(["ssh", "-o", "StrictHostKeyChecking no", admin_name + "@" + master_hostname, 
        dest + "/" + master_binary_name , dest + "/" + input_file_name]))
    break # start only one master
for p in master_tasks:
    p.wait()

print("Uploading done")