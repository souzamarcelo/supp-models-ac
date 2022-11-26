#!/usr/bin/python3
import os
import sys
import subprocess
import random
import numpy as np

def get_inst_size(instance):
    content = instance.split('_')
    return int(content[0]) * (int(content[1]) + 1)

# Get the parameters as command line arguments
configuration_id = sys.argv[1]
instance_id = sys.argv[2]
seed = sys.argv[3]
instance = sys.argv[4]
conf_params = sys.argv[5:]

# Create execution command
fixed_params = ''

tenure_min = None
tenure_max = None
e_min = None
e_max = None
gamma_min = None
gamma_max = None

for i in range(len(conf_params)):
    value = conf_params[i]
    if value == '--param_tenure_min': tenure_min = int(conf_params[i + 1])
    if value == '--param_tenure_max': tenure_max = int(conf_params[i + 1])
    if value == '--param_e_min': e_min = int(conf_params[i + 1])
    if value == '--param_e_max': e_max = int(conf_params[i + 1])
    if value == '--param_gamma_min': gamma_min = float(conf_params[i + 1])
    if value == '--param_gamma_max': gamma_max = float(conf_params[i + 1])

n = get_inst_size(instance[instance.rindex('/') + 1:].replace('.txt', ''))
n_min = 60
n_max = 395

a, b = tuple(np.polyfit([n_min, n_max], [tenure_min, tenure_max], 1))
tenure = int(round(a * n + b))
a, b = tuple(np.polyfit([n_min, n_max], [e_min, e_max], 1))
e = int(round(a * n + b))
a, b = tuple(np.polyfit([n_min, n_max], [gamma_min, gamma_max], 1))
gamma = float(round(a * n + b, 2))

built_params = '--param_tenure ' + str(tenure) + ' --param_e ' + str(e) + ' --param_gamma ' + str(gamma) + ' '
command = '../runner --ins ' + instance + ' --seed ' + str(seed) + ' ' + built_params

# Define the stdout and stderr files
r = str(random.randint(1, 999999))
out_file = './c' + str(configuration_id) + '-' + str(instance_id) + '-' + r + '.stdout'
err_file = './c' + str(configuration_id) + '-' + str(instance_id) + '-' + r + '.stderr'

# Execute
#outf = open(out_file, "w")
#errf = open(err_file, "w")
#return_code = subprocess.call(command, stdout = outf, stderr = errf, shell = True)
#outf.close()
#errf.close()

# Check return code
#if return_code != 0:
#    print('Command returned code <' + str(return_code) + '>.')
#    sys.exit(1)

# Check output file
#if not os.path.isfile(out_file):
#    print('Output file <' + out_file  + '> not found.')
#    sys.exit(1)

# Get result and print it
result = str(subprocess.check_output(command, shell = True))
result = result.replace('b\'', '').replace('\'', '')
if result[-2:] == '\\n': result = result[:-2]
result = float(result)
print(result)

# Clean files and exit
#os.remove(out_file)
#os.remove(err_file)
sys.exit(0)
