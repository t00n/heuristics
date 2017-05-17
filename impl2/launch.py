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
    times = [(float(x[0].strip()), float(x[1].strip())) for x in [x.split(":") for x in output[:-2]]]
    solution = [int(x) for x in output[-2].split(" ")]
    score = float(output[-1])
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


def recup_errors():
    with open('results.json', 'r') as f:
        data = json.load(f)
    errors = [x for x in data if 'error' in data]
    data = [x for x in data if 'error' not in data]
    with Pool(None) as p:
        for res in tqdm(p.imap_unordered(run_proxy, [x['args'] for x in errors])):
            data.append(res)
    with open('fixresults.json', 'w') as f:
        json.dump(data, f)


def run_a_lot(instances, timeout, N):
    return [(i, 'ig', timeout) for i in instances] * N + [(i, 'genetic', timeout) for i in instances] * N


def first_exercise():
    return run_a_lot(INSTANCES50, 30, 5) + run_a_lot(INSTANCES100, 150, 5)


def second_exercise():
    return run_a_lot(sorted(INSTANCES50)[:5], 1000, 25)


def parallelize(jobs):
    with Pool(None) as p:
        for res in tqdm(p.imap_unordered(run_proxy, jobs)):
            save(res)

if __name__ == '__main__':
    import sys
    if sys.argv[1] == "error":
        recup_errors()
    elif sys.argv[1] == "first":
        jobs = first_exercise()
        parallelize(jobs)
    elif sys.argv[1] == "second":
        jobs = second_exercise()
        parallelize(jobs)
