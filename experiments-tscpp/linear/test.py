import os
import sys
import subprocess
import random
import glob
import numpy as np

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
        bkv[content[0]] = int(content[1])

config = ''
reps = 5

config = getConfig(sys.argv[1])
instances = 'instances-test/'

t_min = None
t_max = None
c_min = None
c_max = None
p_min = None
p_max = None

while '  ' in config: config = config.replace('  ', ' ')
config = config.split(' ')

for i in range(len(config)):
    value = config[i]
    if value == '-t_min': t_min = int(config[i + 1])
    if value == '-t_max': t_max = int(config[i + 1])
    if value == '-c_min': c_min = int(config[i + 1])
    if value == '-c_max': c_max = int(config[i + 1])
    if value == '-p_min': p_min = int(config[i + 1])
    if value == '-p_max': p_max = int(config[i + 1])

n_min = 100
n_max = 500

print('instance,bkv,rep,result,adev,rdev', flush = True)
for instance in glob.glob(instances + '*'):
    for rep in range(reps):
        n = None
        if 'rand100-' in instance: n = 100
        if 'rand200-' in instance: n = 200
        if 'rand300-' in instance: n = 300
        if 'rand400-' in instance: n = 400
        if 'rand500-' in instance: n = 500

        a, b = tuple(np.polyfit([n_min, n_max], [t_min, t_max], 1))
        t = int(round(a * n + b))
        a, b = tuple(np.polyfit([n_min, n_max], [c_min, c_max], 1))
        c = int(round(a * n + b))
        a, b = tuple(np.polyfit([n_min, n_max], [p_min, p_max], 1))
        p = int(round(a * n + b))
        
        built_params = '-t ' + str(t) + ' -c ' + str(c) + ' -p ' + str(p) + ' '
        built_params = '-t ' + str(t) + ' -c ' + str(c) + ' -p ' + str(p) + ' '
        command = './tscpp -f ' + instance + ' ' + built_params

        result = str(subprocess.check_output(command, shell = True))
        result = result.replace('b\'', '').replace('\'', '')
        content = result.split('\\n')
        for line in content:
            if 'best result:' in line:
                result = int(line.split(' ')[-1])
                break
        inst = instance.replace(instances, '').replace('.txt', '')
        adev = bkv[inst] - result
        rdev = round(100 - (100 * result / bkv[inst]), 2)
        report = inst + ',' + str(bkv[inst]) + ',' + str(rep + 1) + ',' + str(result) + ',' + str(adev) + ',' + str(rdev)
        print(report, flush = True)