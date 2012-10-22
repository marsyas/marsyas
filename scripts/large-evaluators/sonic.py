#!/usr/bin/env python

import os.path
import subprocess
import pipes

import mar_collection

def single_file_generate_default_plugin(filename, basedir, plugin_name):
    cmd = "sonic-annotator -d %s %s -w csv --csv-force --csv-basedir %s " % (
        plugin_name, pipes.quote(filename), basedir)
    subprocess.call(cmd, shell=True)
    basename = os.path.basename(os.path.splitext(filename)[0])

    result_filename = os.path.join(basedir, "%s_%s.csv" % (
        basename, plugin_name))
    result_filename = result_filename.replace(":", "_")
    return result_filename

def single_file_evaluate(filename):
    try:
        bpm_regions = open(filename).readlines()
    except:
        return -1
    bpm = bpm_regions[0].split(',')[2]
    os.remove(filename)
    return bpm




def single_file_bpm(audio_filename, dirname, plugin):
    result_filename = single_file_generate_default_plugin(
        audio_filename, dirname, plugin)
    bpm = single_file_evaluate(result_filename)
    return bpm


def vamp_collection(queue, datum, plugin, detected_mf_template):
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
            audio_filename, dirname, plugin)
        if bpm < 0:
            problems += 1
        out.write("%s\t%s\n" % (audio_filename, bpm))
    out.close()
    
    ### generate a wrong.mf for each collection
    cmd = "tempo -pi %s -m PREDICTED %s" % (
        detected_mf_filename, mf_filename)
    subprocess.check_call(cmd, shell=True)

    queue.put( (mf_filename, problems) )


