#!/usr/bin/env python3

import subprocess
import os
from multiprocessing import Pool, cpu_count

COMMAND = "./src/fssp {} {} --timeout {}"


def run(instance, algo, timeout=150):
    i = os.path.join('instances', instance)
    a = '--genetic' if algo == 'genetic' else '--ig'
    command = COMMAND.format(i, a, timeout)
    output = subprocess.check_output(command, shell=True, stderr=subprocess.DEVNULL)
    output = output.decode().split('\n')
    output = [x.strip() for x in output if x]
    times = [(float(x[0].strip()), int(x[1].strip())) for x in [x.split(":") for x in output[:-2]]]
    solution = [int(x) for x in output[-2].split(" ")]
    score = int(output[-1])
    return {
        'instance': instance,
        'algo': algo,
        'timeout': timeout,
        'times': times,
        'solution': solution,
        'score': score
    }

if __name__ == '__main__':
    res = run('100_20_12', 'ig', 5)
    print(res)
