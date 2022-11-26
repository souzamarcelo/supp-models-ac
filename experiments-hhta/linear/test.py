import os
import sys
import subprocess
import random
import glob
import numpy as np

def get_inst_size(instance):
    content = instance.split('_')
    return int(content[0]) * (int(content[1]) + 1)

def getConfig(iraceResult):
    resultFile = open(iraceResult, 'r')
    read = False
    for line in resultFile:
        algorithm = line
        if read: break
        read = ('# Best configurations as commandlines' in line)
    resultFile.close()
    while '  ' in algorithm: algorithm = algorithm.replace('  ', ' ')
    algorithm = algorithm.replace('\n', '')
    return algorithm[algorithm.index(' ') + 1:]

bkv = {}
with open('./bkv.txt') as f:
    for line in f.readlines():
        content = line.split(':')
        bkv[content[0]] = float(content[1])

config = ''
reps = 5

config = getConfig(sys.argv[1])
instances = 'instances-test/'

tenure_min = None
tenure_max = None
e_min = None
e_max = None
gamma_min = None
gamma_max = None

while '  ' in config: config = config.replace('  ', ' ')
config = config.split(' ')

for i in range(len(config)):
    value = config[i]
    if value == '--param_tenure_min': tenure_min = int(config[i + 1])
    if value == '--param_tenure_max': tenure_max = int(config[i + 1])
    if value == '--param_e_min': e_min = int(config[i + 1])
    if value == '--param_e_max': e_max = int(config[i + 1])
    if value == '--param_gamma_min': gamma_min = float(config[i + 1])
    if value == '--param_gamma_max': gamma_max = float(config[i + 1])

n_min = 60
n_max = 395

print('instance,bkv,rep,result,rdev', flush = True)
for instance in glob.glob(instances + '*'):
    for rep in range(reps):
        n = get_inst_size(instance[instance.rindex('/') + 1:].replace('.txt', ''))
        a, b = tuple(np.polyfit([n_min, n_max], [tenure_min, tenure_max], 1))
        tenure = int(round(a * n + b))
        a, b = tuple(np.polyfit([n_min, n_max], [e_min, e_max], 1))
        e = int(round(a * n + b))
        a, b = tuple(np.polyfit([n_min, n_max], [gamma_min, gamma_max], 1))
        gamma = float(round(a * n + b, 2))
        
        built_params = '--param_tenure ' + str(tenure) + ' --param_e ' + str(e) + ' --param_gamma ' + str(gamma) + ' '
        command = './runner --ins ' + instance + ' --seed ' + str(random.randint(1, 999999)) + ' ' + built_params

        result = str(subprocess.check_output(command, shell = True))
        result = result.replace('b\'', '').replace('\'', '')
        if result[-2:] == '\\n': result = result[:-2]
        result = float(result)
        inst = instance.replace(instances, '').replace('.txt', '')
        rdev = round((100 * result / bkv[inst]) - 100, 2)
        report = inst + ',' + str(bkv[inst]) + ',' + str(rep + 1) + ',' + str(result) + ',' + str(rdev)
        print(report, flush = True)
