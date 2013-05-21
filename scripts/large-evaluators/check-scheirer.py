#!/usr/bin/env python

DEBUG = 0

import os
import sys
import glob

import eval_manager
import mar_collection

import subprocess
import pipes

def single_file_scheirer(filename):
    cmd = "scheirer-tapping -infile %s " % (
        pipes.quote(filename))
    results = subprocess.check_output(cmd, shell=True).splitlines()
    bpm = 0
    for line in results[-3:]:
        try:
            lastline = results[-1].split()
            bpm = lastline[3]
        except:
            pass
    return bpm


def scheirer_collection(queue, datum, detected_mf_template):
    mf_filename = datum[0]
    output_dir = datum[1]
    if not os.path.exists(output_dir):
        os.makedirs(output_dir)

    mf_basename = os.path.basename(
        os.path.splitext(mf_filename)[0])
    dirname = os.path.join(output_dir, mf_basename)
    if not os.path.exists(dirname):
        os.makedirs(dirname)
    os.chdir(dirname)

    detected_mf_filename = '%s-%s.mf' % (mf_basename, detected_mf_template)
    out = open(detected_mf_filename, 'w')
    coll = mar_collection.MarCollection(mf_filename)
    problems = 0
    for audio_filename in coll.get_filenames():
        bpm = single_file_scheirer(
            audio_filename)
        if bpm < 0:
            problems += 1
        out.write("%s\t%s\n" % (audio_filename, bpm))
    out.close()
    
    ### generate a wrong.mf for each collection
    cmd = "tempo -pi %s -m PREDICTED %s" % (
        detected_mf_filename, mf_filename)
    subprocess.check_call(cmd, shell=True)

    queue.put( (mf_filename, problems) )


def scheirer(queue, data):
    detected_mf_template = "scheirer"
    scheirer_collection(queue, data, detected_mf_template)

def main(mf_dir, output_dir):
    dirname = os.environ['MARSYAS_DATADIR']
    collections = glob.glob(os.path.expanduser(
        os.path.join(dirname, "*_tempos.mf")))
    collections.sort()
    data = []
    for coll in collections:
        datum = (coll, output_dir)
        data.append(datum)

    manager = eval_manager.EvalManager(4)
    if DEBUG:
        manager.task_block(scheirer, data)
    else:
        manager.task(scheirer, data)

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


