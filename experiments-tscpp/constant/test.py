import os
import sys
import subprocess
import random
import glob

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

print('instance,bkv,rep,result,adev,rdev', flush = True)
for instance in glob.glob(instances + '*'):
    for rep in range(reps):
        command = './tscpp -f ' + instance + ' ' + config
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