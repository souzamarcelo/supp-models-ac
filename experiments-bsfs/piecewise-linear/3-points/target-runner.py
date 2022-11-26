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

alpha_1 = None
alpha_2 = None
alpha_3 = None

for i in range(len(conf_params)):
    value = conf_params[i]
    if value == '--alpha_1': alpha_1 = int(conf_params[i + 1])
    if value == '--alpha_2': alpha_2 = int(conf_params[i + 1])
    if value == '--alpha_3': alpha_3 = int(conf_params[i + 1])

n = None
if 'tai20_5.' in instance: n = 100
if 'tai20_10.' in instance: n = 200
if 'tai50_5.' in instance: n = 250
if 'tai20_20.' in instance: n = 400
if 'tai50_10.' in instance: n = 500
if 'tai50_20.' in instance: n = 1000
if 'tai100_5.' in instance: n = 500
if 'tai100_10.' in instance: n = 1000
if 'tai100_20.' in instance: n = 2000
if 'tai200_10.' in instance: n = 2000
if 'tai200_20.' in instance: n = 4000
if 'tai500_20.' in instance: n = 10000
n_1 = 100
n_2 = 2000
n_3 = 10000

if n > n_1 and n <= n_2:
    n_min = 100
    n_max = 2000
    alpha_min = alpha_1
    alpha_max = alpha_2
else:
    n_min = 2000
    n_max = 10000
    alpha_min = alpha_2
    alpha_max = alpha_3

a, b = tuple(np.polyfit([n_min, n_max], [alpha_min, alpha_max], 1))
alpha = int(round(a * n + b))

built_params = '--alpha ' + str(alpha) + ' '

# Create execution command
command = '../bubble --instance ' + instance + ' --seed ' + str(seed) + ' ' + built_params

# Define the stdout and stderr files
r = str(random.randint(1, 999999))
out_file = './c' + str(configuration_id) + '-' + str(instance_id) + '-' + r + '.stdout'
err_file = './c' + str(configuration_id) + '-' + str(instance_id) + '-' + r + '.stderr'

# Execute
outf = open(out_file, "w")
errf = open(err_file, "w")
return_code = subprocess.call(command, stdout = outf, stderr = errf, shell = True)
outf.close()
errf.close()

# Check return code
if return_code != 0:
    print('Command returned code <' + str(return_code) + '>.')
    sys.exit(1)

# Check output file
if not os.path.isfile(out_file):
    print('Output file <' + out_file  + '> not found.')
    sys.exit(1)

# Get result and print it
result = open(out_file).readlines()[-1]
result = float(result.replace('\n', ''))
print(result)

# Clean files and exit
os.remove(out_file)
os.remove(err_file)
sys.exit(0)
