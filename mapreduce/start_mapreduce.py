import subprocess
from user_parameters import *
import time
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


print()
worker_tasks = []
for worker_hostname in workers:
    print("Start workers: ", worker_hostname)
    worker_tasks.append(subprocess.Popen(["ssh", "-o", "StrictHostKeyChecking no", admin_name + "@" + worker_hostname, 
        dest + "/" + worker_binary_name]))

time.sleep(10)

print("Workers are running....")
'''
print("Start masters [Here, only start a single master without using leader election]")
master_tasks = []
workers_hostname__ = '"' + ";".join(workers) + '"'
for master_hostname in masters:
    master_tasks.append(subprocess.Popen(["ssh", "-o", "StrictHostKeyChecking no", admin_name + "@" + master_hostname, 
        dest + "/" + master_binary_name , 
        dest + "/" + input_file_name, # input files
        # workers_hostname__            # worker hostnames
        ]))
    break # start only one master
for p in master_tasks:
    p.wait()

print("Masters are running....")

'''
time.sleep(4)

print("Killing worker...")
for p in worker_tasks:
    pass
    #p.kill()
