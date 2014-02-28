#!/usr/bin/env python

import os
import sys
import subprocess
import glob

import eval_manager
import mar_collection


def marsyas_tempo(queue, datum):
    coll_filename = datum[0]
    output_dir = datum[1]
    orig_dir = datum[2]
    if not os.path.exists(output_dir):
        os.makedirs(output_dir)

    coll_basename = os.path.basename(
        os.path.splitext(coll_filename)[0])
    dirname = os.path.join(output_dir, coll_basename)
    if not os.path.exists(dirname):
        os.makedirs(dirname)
    os.chdir(dirname)
    cmd = "tempo %s -po %s" % (
        coll_filename, coll_basename + '-detected.mf')
    #cmd = "ls"
    output = subprocess.check_output(cmd, shell=True)

    filename = os.path.join(orig_dir, 
        '%s-output.txt' % coll_basename)
    out = open(filename, 'w')
    out.write(output)
    out.close()

    queue.put(output)

def main(mf_dir, output_dir):
    dirname = os.environ['MARSYAS_DATADIR']
    collections = glob.glob(os.path.expanduser(
        os.path.join(dirname, "*_tempos.mf")))
    collections.sort()
    orig_dir = os.path.abspath(os.path.curdir)
    data = []
    for coll in collections:
        datum = (coll, output_dir, orig_dir)
        data.append(datum)

    manager = eval_manager.EvalManager(3)
    manager.task(marsyas_tempo, data)
    #manager.task_block(marsyas_tempo, data)

    results_datasets = manager.get_results()
    for results in results_datasets:
        # there's an extra \n at the end of the output
        print results[:-1]


    #partial_files = glob.glob("partial-*.txt")
    #lines = []
    #for p in partial_files:
    #    p_lines = open(p).readlines()
    #    lines.extend(p_lines)
    #out = open("combo.txt", 'w')
    #for l in lines:
    #    out.write(l)
    #out.close()


if __name__ == '__main__':
    try:
        mf_dir = sys.argv[1]
        output_dir = sys.argv[2]
    except:
        print "Need 1) dir containing the _tempos.mf files"
        print "and  2) dir to output data"
        exit(1)

    main(mf_dir, output_dir)


