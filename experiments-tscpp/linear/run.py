#!/usr/bin/python3
import os
import sys

budget = sys.argv[1]
reps = [1, 2, 3, 4, 5]

for i in reps:
    rep = str(i)
    dir = 'b' + budget + '-' + rep
    os.system('mkdir -p ' + dir)
    print('Running budget', budget, 'replication', rep, flush = True)
    os.system('irace --parallel 6 --max-experiments ' + budget + ' --exec-dir ./' + dir + ' 1> ./' + dir + '/results.txt 2> ./' + dir + '/error.txt')
    print('DONE budget', budget, 'replication', rep, flush = True)
