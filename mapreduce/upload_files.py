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


# compile MapReduce framework to binaries
# print("Compiling Code...")
# subprocess.Popen(["mkdir", ".build"])
# subprocess.Popen(["cmake", ".."], cwd="./.build")
# subprocess.Popen(["make"], cwd="./.build")
# divides nodes into master group and worker groups
# ssh nan@master ./startmaster -input 
# distribute code
# ssh -o "StrictHostKeyChecking no" user@host

# upload input files/binaries
print("Uploading input files/binaries/application code to masters/workers")
tasks = []
for node in mapreduce_node_hostnames:
    tasks.append(subprocess.Popen(["ssh", "-o", "StrictHostKeyChecking no", admin_name + "@" + node, "rm", "-r", dest]))
for p in tasks:
    p.wait()

tasks = []
for node in mapreduce_node_hostnames:
    tasks.append(subprocess.Popen(["ssh", "-o", "StrictHostKeyChecking no", admin_name + "@" + node, "mkdir", dest]))
for p in tasks:
    p.wait()



master_upload_tasks = []
for master_hostname in masters:
    master_upload_tasks.append(subprocess.Popen(["scp", "-o", "StrictHostKeyChecking no", master_binaries, admin_name + "@" + master_hostname + ":" + dest]))
    master_upload_tasks.append(subprocess.Popen(["scp", "-o", "StrictHostKeyChecking no", input_file, admin_name + "@" + master_hostname + ":" + dest]))

worker_upload_tasks = []
for worker_hostname in workers:
    worker_upload_tasks.append(subprocess.Popen(["scp", "-o", "StrictHostKeyChecking no", worker_binaries, admin_name + "@" + worker_hostname + ":" + dest]))
    worker_upload_tasks.append(subprocess.Popen(["scp", "-o", "StrictHostKeyChecking no", mapper_code, admin_name + "@" + worker_hostname + ":" + dest]))
    worker_upload_tasks.append(subprocess.Popen(["scp", "-o", "StrictHostKeyChecking no", reducer_code, admin_name + "@" + worker_hostname + ":" + dest]))


for p in master_upload_tasks:
    p.wait()
for p in worker_upload_tasks:
    p.wait()

print("Uploading done")