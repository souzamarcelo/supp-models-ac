#!/usr/bin/python3
import os
import sys
import subprocess
import random

# Get the parameters as command line arguments
configuration_id = sys.argv[1]
instance_id = sys.argv[2]
seed = sys.argv[3]
instance = sys.argv[4]
conf_params = ' '.join(sys.argv[5:])

command = '../tscpp -f ' + instance + ' ' + conf_params

# Get result and print it
result = str(subprocess.check_output(command, shell = True))
result = result.replace('b\'', '').replace('\'', '')
content = result.split('\\n')
for line in content:
    if 'best result:' in line:
        result = int(line.split(' ')[-1])
        break
result = result * (-1)
print(result)