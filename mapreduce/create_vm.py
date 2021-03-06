import subprocess
from user_parameters import *

subprocess_list = []
mapreduce_node_hostnames = []
# rm  ~/.ssh/known_hosts
subprocess.Popen(["rm", "~/.ssh/known_hosts"]).wait()
for counter in range(1, number_node + 1):
    print("Creating VM: " + hostname_prefix + str(counter))
    subprocess_list.append(subprocess.Popen(["az", 
        "vm", "create", 
        "--resource-group", resource_group_name,
        "--name",  hostname_prefix + str(counter),
        "--image", image_name,
        "--admin-username", admin_name,
        "--ssh-key-value", ssh_public_key_location,
        "--size", vm_size,
        "--location", location,
        "--private-ip-address", private_ip_prefix + str(counter + node_IP_start - 1)
        ]))
    mapreduce_node_hostnames.append(hostname_prefix + str(counter))

######
print("Waiting for new VM running...")
for p in subprocess_list:
    p.wait()
print("Now " + str(number_node) + " VM instance(s) is running!")

