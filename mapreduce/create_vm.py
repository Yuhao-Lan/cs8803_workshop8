import subprocess
from user_parameters import *

subprocess_list = []

for counter in range(1, number_node + 1):
    print("Deleting VM: " + "map-reduce-node-" + str(counter))
    subprocess_list.append(subprocess.Popen(["az", 
        "vm", "create", 
        "--resource-group", resource_group_name,
        "--name", "map-reduce-node-" + str(counter),
        "--image", image_name,
        "--admin-username", admin_name,
        "--ssh-key-value", ssh_public_key_location,
        "--size", vm_size,
        "--location", location,
        "--private-ip-address", private_ip_prefix + str(counter + node_IP_start - 1)
        ]))

######
print("Waiting for new VM running...")
for p in subprocess_list:
    p.wait()
print("Now " + str(number_node) + " VM instance(s) is running!")