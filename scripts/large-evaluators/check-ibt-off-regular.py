#!/usr/bin/env python

import os
import sys
import glob

import eval_manager
import mar_collection

import subprocess
import pipes

def single_file_ibt(filename):
    cmd = "ibt -off -i \"regular\" %s " % (
        pipes.quote(filename))
    subprocess.call(cmd, shell=True)
    basename = os.path.basename(os.path.splitext(filename)[0])

    os.remove(basename+".txt")
    result_filename = basename+"_medianTempo.txt"
    return result_filename

def single_file_evaluate(filename):
    try:
        bpm_regions = open(filename).readlines()
    except:
        return -1
    bpm = bpm_regions[0].split(',')[0]
    os.remove(filename)
    return bpm




def single_file_bpm(audio_filename, dirname):
    result_filename = single_file_ibt(
        audio_filename)
    bpm = single_file_evaluate(result_filename)
    return bpm


def ibt_collection(queue, datum, detected_mf_template):
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
        bpm = single_file_bpm(
            audio_filename, dirname)
        if bpm < 0:
            problems += 1
        out.write("%s\t%s\n" % (audio_filename, bpm))
    out.close()
    
    ### generate a wrong.mf for each collection
    cmd = "tempo -pi %s -m PREDICTED %s" % (
        detected_mf_filename, mf_filename)
    subprocess.check_call(cmd, shell=True)

    queue.put( (mf_filename, problems) )


def ibt(queue, data):
    detected_mf_template = "ibt-off-reg"
    ibt_collection(queue, data, detected_mf_template)

def main(mf_dir, output_dir):
    collections = glob.glob(os.path.expanduser(
        "~/src/audio-research/*_tempos.mf"))
    collections.sort()
    data = []
    for coll in collections:
        datum = (coll, output_dir)
        data.append(datum)

    manager = eval_manager.EvalManager(4)
    manager.task(ibt, data)
    #manager.task_block(ibt, data)

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


