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

pertsize_min = None
pertsize_max = None

for i in range(len(conf_params)):
    value = conf_params[i]
    if value == '--pertsize_min': pertsize_min = int(conf_params[i + 1])
    if value == '--pertsize_max': pertsize_max = int(conf_params[i + 1])

n = None
if 'bqp2500' in instance: n = 2500
elif 'bqp1000' in instance: n = 1000
elif 'bqp500' in instance: n = 500
elif 'bqp250' in instance: n = 250
n_min = 250
n_max = 2500

t = None
if 'bqp2500' in instance: t = 30
elif 'bqp1000' in instance: t = 15
elif 'bqp500' in instance: t = 8
elif 'bqp250' in instance: t = 4
m = '0'
if 'bqp2500' in instance: m = '1'

a, b = tuple(np.polyfit([n_min, n_max], [pertsize_min, pertsize_max], 1))
pertsize = int(round(a * n + b))

built_params = '--pertsize ' + str(pertsize) + ' '

# Create execution command
command = '../ilsbqp --ins ' + instance + ' --format sparse --maximize ' + m + ' --seed ' + str(seed) + ' --runtime ' + str(t) + ' ' + built_params

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
