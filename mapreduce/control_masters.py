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
    masters.append(mapreduce_node_hostnames[i])
for i in range(number_master, number_node):
    workers.append(mapreduce_node_hostnames[i])


# compile MapReduce framework to binaries


# divides nodes into master group and worker groups
# ssh nan@master ./startmaster -input 
# distribute code
# ssh -o "StrictHostKeyChecking no" user@host

# upload input files/binaries
print("Uploading input files/binaries to masters")
master_upload_tasks = []
for master_hostname in masters:
    master_upload_tasks.append(subprocess.Popen(["ssh", "-o", "StrictHostKeyChecking no", admin_name + "@" + master_hostname, "mkdir", "master"]))
for p in master_upload_tasks:
    p.wait()

master_upload_tasks = []
for master_hostname in masters:
    master_upload_tasks.append(subprocess.Popen(["scp", "-o", "StrictHostKeyChecking no", input_file, admin_name + "@" + master_hostname + ":~/master"]))
for p in master_upload_tasks:
    p.wait()

print("Uploading done")