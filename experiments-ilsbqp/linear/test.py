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

pertsize_min = None
pertsize_max = None

config = config.split(' ')
for i in range(len(config)):
    value = config[i]
    if value == '--pertsize_min': pertsize_min = int(config[i + 1])
    if value == '--pertsize_max': pertsize_max = int(config[i + 1])

print('instance,bkv,rep,result,adev', flush = True)
for instance in glob.glob(instances + '*'):
    for rep in range(reps):
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
        
        seed = random.randint(1, 99999999)
        command = './ilsbqp --ins ' + instance + ' --format sparse --maximize ' + m + ' --seed ' + str(seed) + ' --runtime ' + str(t) + ' ' + built_params

        result = str(subprocess.check_output(command, shell = True))
        result = result.replace('b\'', '').replace('\'', '')
        if result[-2:] == '\\n': result = result[:-2]
        result = float(result)
        inst = instance.replace(instances, '').replace('.sparse', '')
        adev = result - bkv[inst]
        report = inst + ',' + str(bkv[inst]) + ',' + str(rep + 1) + ',' + str(result) + ',' + str(round(adev, 2))
        print(report, flush = True)
