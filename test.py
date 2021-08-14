import os
import subprocess, sys
import multiprocessing as mp
import re
import pdb

TESTER = './tools/target/release/vis'
TEST_DIR = './tools/in'
ANSWER = './answer'

#TEST = [0]
#TEST = [0, 10, 20, 30, 40, 50, 60, 70, 80, 90]
TEST = [0, 5, 10, 15, 20, 25, 30, 35, 40, 45, 50, 55, 60, 65, 70, 75, 80, 85, 90, 95]
#TEST = [0, 2, 5, 8, 10, 12, 15, 18, 20, 22, 25, 28, 30, 32, 35, 38, 40, 42, 45, 48, 
#        50, 52, 55, 58, 60, 62, 65, 68, 70, 72, 75, 78, 80, 82, 85, 88, 90, 92, 95, 98]
#TEST = [1, 3, 6, 9, 11, 13, 16, 19, 21, 23, 26, 29, 31, 33, 36, 39, 41, 43, 46, 49, 
#        51, 53, 56, 59, 61, 63, 66, 69, 71, 73, 76, 79, 81, 83, 86, 89, 91, 93, 96, 99]

MAX_P = 10


def run_test(queue, input_file, tmp_file, args):
    cmd = [ANSWER]
    for arg in args:
        cmd.append(str(arg))
    #print(cmd)

    with open(input_file, 'r') as ifp:
        with open(tmp_file, 'w') as ofp:
            proc = subprocess.run(cmd, encoding='utf-8', stdin=ifp, stderr=subprocess.DEVNULL, stdout=ofp)
    if proc.returncode != 0:
        print(f'{ANSWER} failed.', file=sys.stderr)
        sys.exit(1)

    proc = subprocess.run([TESTER, input_file, tmp_file], encoding='utf-8', stderr=subprocess.PIPE, stdout=subprocess.PIPE)
    if proc.returncode != 0:
        print(f'{TESTER} failed.', file=sys.stderr)
        sys.exit(1)

    score = int(re.sub(r"\D", "", proc.stdout))
    queue.put(score)

def run_process(test_num, args):
    input_file = os.path.join(TEST_DIR, '{:04}.txt'.format(test_num))
    tmp_file = f'./out/{test_num}.txt'

    queue = mp.Queue()
    proc = mp.Process(target=run_test, args=(queue, input_file, tmp_file, args))
    proc.start()
    return proc, queue

def proc_wait(proc, queue):
    proc.join()
    score = queue.get()
    return score

def get_score(debug=False, args=[]):
    total = 0
    proc_list = []

    for t in TEST:
        proc, queue = run_process(t, args)
        proc_list.append((t, proc, queue))
        if len(proc_list) == MAX_P:
            t, proc, queue = proc_list.pop(0)
            score = proc_wait(proc, queue)
            if debug:
                print('test {:02} : {}'.format(t, score))
            total += score

    for t, proc, queue, in proc_list:
        score = proc_wait(proc, queue)
        if debug:
            print('test {:02} : {}'.format(t, score))
        total += score

    if debug:
        print()
        print(f'total score : {total}.')
        print(f'average score : {total // len(TEST)}.')
    else:
        print(f'{total // len(TEST)}')

if __name__ == '__main__':
    #get_score(debug=True, args=[50.0, 1.0])
    get_score(debug=True)
