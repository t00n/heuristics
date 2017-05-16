#!/usr/bin/env python3

import subprocess
import os
from multiprocessing import Pool, cpu_count
import json
import glob
from tqdm import tqdm

COMMAND = "./src/fssp {} {} --timeout {}"


def run(instance, algo, timeout=150):
    a = '--genetic' if algo == 'genetic' else '--ig'
    command = COMMAND.format(instance, a, timeout)
    with open('LOLOLOL', 'a') as f:
        output = subprocess.check_output(command, shell=True, stderr=f)
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


def run_proxy(args):
    try:
        return run(*args)
    except Exception as e:
        return {'error': str(e), 'args': args}


def save(result):
    try:
        with open('results.json', 'r') as f:
            all_results = json.load(f)
    except FileNotFoundError:
        all_results = []
    all_results.append(result)
    with open('results.json', 'w') as f:
        json.dump(all_results, f)

INSTANCES50 = glob.glob('instances/50*')
INSTANCES100 = glob.glob('instances/100*')


def run50():
    return [(i, 'ig', 30) for i in INSTANCES50] * 5 + [(i, 'genetic', 30) for i in INSTANCES50] * 5


def run100():
    return [(i, 'ig', 150) for i in INSTANCES100] * 5 + [(i, 'genetic', 150) for i in INSTANCES100] * 5

if __name__ == '__main__':
    jobs = run50() + run100()
    with Pool(None) as p:
        for res in tqdm(p.imap_unordered(run_proxy, jobs)):
            save(res)
