#!/usr/bin/env python

DEBUG = 0

import os
import sys
import subprocess
import glob

import numpy
import pipes

import eval_manager
import mar_collection


def single_file_generate(audio_filename, basedir):
    plugin_added = "vamp:qm-vamp-plugins:qm-tempotracker:tempo"
    cmd = "sonic-annotator -d %s %s -w csv --csv-force --csv-basedir %s " % (
        plugin_added, pipes.quote(audio_filename), basedir)
    subprocess.call(cmd, shell=True)
    basename = os.path.basename(os.path.splitext(audio_filename)[0])
    logname = os.path.join(basedir, "%s_%s.csv" % (basename, plugin_added))
    logname = logname.replace(":", "_")
    return logname

def single_file_evaluate_mean_median(audio_filename, logname):
    try:
        bpm_regions = open(logname).readlines()
    except:
        return -1
    bpm = bpm_regions[0].split(',')[2]
    os.remove(logname)

    cmd = "sfinfo %s" % (pipes.quote(audio_filename))
    sfinfo_output = subprocess.check_output(cmd, shell=True)
    for line in sfinfo_output.split('\n'):
        if line.find("Duration") >= 0:
            splitline = line.split()
            total_length_seconds = float(splitline[3])

    regs = []
    for region in bpm_regions:
        start = float(region.split(',')[0])
        bpm = float(region.split(',')[1])
        regs.append( (start, bpm) )
    regs.append( (total_length_seconds, 0) )
    average = 0.0
    durs = []
    for i in range(len(regs)-1):
        rega = regs[i]
        regb = regs[i+1]
        region_duration = regb[0] - rega[0]
        durs.append( (region_duration, rega[1]) )
        region_bpm = rega[1]
        average += region_duration * region_bpm
    regioned_duration = total_length_seconds - regs[0][0]
    average /= regioned_duration
    median = numpy.median( [a[1] for a in regs] )

    durs.sort()
    mode = durs[-1][1]

    return average, median, mode


def vamp_qm_default(queue, datum):
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

    detected_mf_filename_mean = '%s-qm_default_mean.mf' % (mf_basename)
    detected_mf_filename_median = '%s-qm_default_median.mf' % (mf_basename)
    detected_mf_filename_mode = '%s-qm_default_mode.mf' % (mf_basename)
    out_mean = open(detected_mf_filename_mean, 'w')
    out_median = open(detected_mf_filename_median, 'w')
    out_mode = open(detected_mf_filename_mode, 'w')
    coll = mar_collection.MarCollection(mf_filename)
    problems = 0
    for audio_filename in coll.get_filenames():
        result_filename = single_file_generate(audio_filename, dirname)
        bpm_mean, bpm_median, bpm_mode = single_file_evaluate_mean_median(audio_filename, result_filename)
        if bpm_mean < 0 or bpm_median < 0:
            problems += 1
        out_mean.write("%s\t%s\n" % (
            audio_filename, bpm_mean))
        out_median.write("%s\t%s\n" % (
            audio_filename, bpm_median))
        out_mode.write("%s\t%s\n" % (
            audio_filename, bpm_mode))
    out_mean.close()
    out_median.close()
    out_mode.close()
    
    ### generate a wrong.mf for each collection
    cmd = "tempo -pi %s -m PREDICTED %s" % (
        detected_mf_filename_mean, mf_filename)
    subprocess.check_call(cmd, shell=True)
    #cmd = "mv %s %s" % ("wrong.mf", "wrong-mean.mf")
    #subprocess.check_call(cmd, shell=True)

    cmd = "tempo -pi %s -m PREDICTED %s" % (
        detected_mf_filename_median, mf_filename)
    subprocess.check_call(cmd, shell=True)
    #cmd = "mv %s %s" % ("wrong.mf", "wrong-median.mf")
    #subprocess.check_call(cmd, shell=True)

    cmd = "tempo -pi %s -m PREDICTED %s" % (
        detected_mf_filename_mode, mf_filename)
    subprocess.check_call(cmd, shell=True)
    #cmd = "mv %s %s" % ("wrong.mf", "wrong-mode.mf")
    #subprocess.check_call(cmd, shell=True)

    queue.put( (mf_filename, problems) )

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
        manager.task_block(vamp_qm_default, data)
    else:
        manager.task(vamp_qm_default, data)

    aa = manager.get_results()
    print
    problems = 0
    for a in aa:
        #print a
        problems += a[1]
    print "Total problematic files: %i" % problems
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


