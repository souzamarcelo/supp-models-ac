#!/usr/bin/python3
import os
import sys

budget = sys.argv[1]
reps = 5

for i in range(reps):
    if i < 4: continue
    rep = str(i + 1)
    dir = 'b' + budget + '-' + rep
    os.system('mkdir -p ' + dir)
    print('Running budget', budget, 'replication', rep, flush = True)
    os.system('irace --parallel 3 --max-experiments ' + budget + ' --exec-dir ./' + dir + ' 1> ./' + dir + '/results.txt 2> ./' + dir + '/error.txt')
    print('DONE budget', budget, 'replication', rep, flush = True)
