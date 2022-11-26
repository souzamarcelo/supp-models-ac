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

print('instance,bkv,rep,result,adev', flush = True)
for instance in glob.glob(instances + '*'):
    for rep in range(reps):
        t = None
        if 'bqp2500' in instance: t = 30
        elif 'bqp1000' in instance: t = 15
        elif 'bqp500' in instance: t = 8
        elif 'bqp250' in instance: t = 4
        m = '0'
        if 'bqp2500' in instance: m = '1'

        command = './ilsbqp --ins ' + instance + ' --format sparse --maximize ' + m + ' --seed ' + str(random.randint(1, 999999)) + ' --runtime ' + str(t) + ' ' + config
        result = str(subprocess.check_output(command, shell = True))
        result = result.replace('b\'', '').replace('\'', '')
        if result[-2:] == '\\n': result = result[:-2]
        result = float(result)
        inst = instance.replace(instances, '').replace('.sparse', '')
        adev = result - bkv[inst]
        report = inst + ',' + str(bkv[inst]) + ',' + str(rep + 1) + ',' + str(result) + ',' + str(round(adev, 2))
        print(report, flush = True)
