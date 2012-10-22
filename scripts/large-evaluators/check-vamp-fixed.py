#!/usr/bin/env python

import os
import sys
import glob

import eval_manager
#import mar_collection
import sonic

def vamp_fixed(queue, data):
    plugin = "vamp:vamp-example-plugins:fixedtempo:tempo"
    detected_mf_template = "fixed_tempo"
    sonic.vamp_collection(queue, data, plugin, detected_mf_template)

def main(mf_dir, output_dir):
    collections = glob.glob(os.path.expanduser(
        "~/src/audio-research/*_tempos.mf"))
    collections.sort()
    data = []
    for coll in collections:
        datum = (coll, output_dir)
        data.append(datum)

    manager = eval_manager.EvalManager(4)
    manager.task(vamp_fixed, data)
    #manager.task_block(vamp_fixed, data)

    aa = manager.get_results()
    print
    problems = 0
    for a in aa:
        #print a
        problems += a[1]
    print "Total problematic files: %i" % problems
    print

if __name__ == '__main__':
    try:
        mf_dir = sys.argv[1]
        output_dir = sys.argv[2]
    except:
        print "Need 1) dir containing the _tempos.mf files"
        print "and  2) dir to output data"
        exit(1)

    main(mf_dir, output_dir)


