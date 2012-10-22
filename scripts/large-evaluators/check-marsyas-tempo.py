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
    subprocess.check_call(cmd, shell=True)
    queue.put(cmd)

def main(mf_dir, output_dir):
    collections = glob.glob(os.path.expanduser(
        "~/src/audio-research/*_tempos.mf"))
    collections.sort()
    data = []
    for coll in collections:
        datum = (coll, output_dir)
        data.append(datum)

    manager = eval_manager.EvalManager(4)
    manager.task(marsyas_tempo, data)
    #manager.task_block(marsyas_tempo, data)

    aa = manager.get_results()
    for a in aa:
        print a
    print
    #global missing_files
    #if missing_files > 0:
    #    print "Warning: missing files:", missing_files


if __name__ == '__main__':
    try:
        mf_dir = sys.argv[1]
        output_dir = sys.argv[2]
    except:
        print "Need 1) dir containing the _tempos.mf files"
        print "and  2) dir to output data"
        exit(1)

    main(mf_dir, output_dir)


