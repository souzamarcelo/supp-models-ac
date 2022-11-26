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
        bkv[content[0]] = float(content[1])

config = ''
reps = 5

config = getConfig(sys.argv[1])
instances = 'instances-test/'

print('instance,bkv,rep,result,rdev', flush = True)
for instance in glob.glob(instances + '*'):
    for rep in range(reps):
        command = './runner --ins ' + instance + ' --seed ' + str(random.randint(1, 999999)) + ' ' + config
        result = str(subprocess.check_output(command, shell = True))
        result = result.replace('b\'', '').replace('\'', '')
        if result[-2:] == '\\n': result = result[:-2]
        result = float(result)
        inst = instance.replace(instances, '').replace('.txt', '')
        rdev = round((100 * result / bkv[inst]) - 100, 2)
        report = inst + ',' + str(bkv[inst]) + ',' + str(rep + 1) + ',' + str(result) + ',' + str(rdev)
        print(report, flush = True)
