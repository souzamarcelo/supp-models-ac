#!/usr/bin/python3
import os
import sys
import subprocess
import random
import numpy as np

# Get the parameters as command line arguments
configuration_id = sys.argv[1]
instance_id = sys.argv[2]
seed = sys.argv[3]
instance = sys.argv[4]
conf_params = sys.argv[5:]

# Create execution command
fixed_params = ''

t_min = None
t_max = None
c_min = None
c_max = None
p_min = None
p_max = None

for i in range(len(conf_params)):
    value = conf_params[i]
    if value == '-t_min': t_min = int(conf_params[i + 1])
    if value == '-t_max': t_max = int(conf_params[i + 1])
    if value == '-c_min': c_min = int(conf_params[i + 1])
    if value == '-c_max': c_max = int(conf_params[i + 1])
    if value == '-p_min': p_min = int(conf_params[i + 1])
    if value == '-p_max': p_max = int(conf_params[i + 1])

n = None
if 'rand100-' in instance: n = 100
if 'rand200-' in instance: n = 200
if 'rand300-' in instance: n = 300
if 'rand400-' in instance: n = 400
if 'rand500-' in instance: n = 500
n_min = 100
n_max = 500

a, b = tuple(np.polyfit([n_min, n_max], [t_min, t_max], 1))
t = int(round(a * n + b))
a, b = tuple(np.polyfit([n_min, n_max], [c_min, c_max], 1))
c = int(round(a * n + b))
a, b = tuple(np.polyfit([n_min, n_max], [p_min, p_max], 1))
p = int(round(a * n + b))

built_params = '-t ' + str(t) + ' -c ' + str(c) + ' -p ' + str(p) + ' '
command = '../tscpp -f ' + instance + ' ' + built_params

result = str(subprocess.check_output(command, shell = True))
result = result.replace('b\'', '').replace('\'', '')
content = result.split('\\n')
for line in content:
    if 'best result:' in line:
        result = int(line.split(' ')[-1])
        break
result = result * (-1)
print(result)
