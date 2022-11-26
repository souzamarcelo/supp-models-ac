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
instances = 'instances/'

alpha_1 = None
alpha_2 = None
alpha_3 = None

config = config.split(' ')
for i in range(len(config)):
    value = config[i]
    if value == '--alpha_1': alpha_1 = int(config[i + 1])
    if value == '--alpha_2': alpha_2 = int(config[i + 1])
    if value == '--alpha_3': alpha_3 = int(config[i + 1])

print('instance,bkv,rep,result,rdev', flush = True)
for instance in glob.glob(instances + '*'):
    for rep in range(reps):
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
        
        for pos in range(10):
            seed = random.randint(1, 99999999)
            command = './bubble --instance ' + instance + ' --pos ' + str(pos) + ' --seed ' + str(seed) + ' ' + built_params
            result = str(subprocess.check_output(command, shell = True))
            result = result.replace('b\'', '').replace('\'', '')
            if result[-2:] == '\\n': result = result[:-2]
            result = int(float(result))
            inst = instance.replace(instances, '').replace('.txt', '') + '.' + str(pos)
            rdev = (100 * result / bkv[inst]) - 100
            report = inst + ',' + str(bkv[inst]) + ',' + str(rep + 1) + ',' + str(result) + ',' + str(round(rdev, 2))
            print(report, flush = True)
