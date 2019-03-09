import subprocess
from user_parameters import *

for counter in range(1, number_node + 1):
    print("Creating VM: " + "map-reduce-node-" + str(counter))
    subprocess.Popen(["az", 
        "vm", "delete", 
        "--resource-group", resource_group_name,
        "--name", "map-reduce-node-" + str(counter),
        "--yes"])