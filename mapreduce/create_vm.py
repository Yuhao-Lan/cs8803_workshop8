import subprocess

number_node = 3
node_IP_start = 10 # 10.0.0.10

resource_group_name = "nqin8-resource-group"
image_name = "nqin8-workshop-image"
admin_name = "nan"
ssh_public_key_location = "~/.ssh/id_rsa.pub"
vm_size = "Standard_B1s"
location = "eastus"
private_ip_prefix = "10.0.0."
for counter in range(1, number_node + 1):
    subprocess.run(["az", 
        "vm", "create", 
        "--resource-group", resource_group_name,
        "--name", "map-reduce-node-" + str(counter),
        "--image", image_name,
        "--admin-username", admin_name,
        "--ssh-key-value", ssh_public_key_location,
        "--size", vm_size,
        "--location", location,
        "--private-ip-address", private_ip_prefix + str(counter + node_IP_start - 1)
        ])