#!/usr/bin/env python

import sys
import os.path
import os

import time

import pipes
import json

import mar_collection

def echonest_upload(full_filename):
    print "trying: ", full_filename
    cmd = "echonest-analyzer %s" % pipes.quote(full_filename)
    os.system(cmd)
    time.sleep(0.5)

    json_data = open( full_filename+'.json')
    data = json.load(json_data)
    json_data.close()

    bpm = data["track"]["tempo"]
    print bpm
    return bpm



def main(mf_name):
    coll = mar_collection.MarCollection(mf_name)
    filenames = coll.get_filenames()

    bpm_mf_name = os.path.basename(mf_name)
    bpm_mf_name = bpm_mf_name.replace(".mf", "-echonest_bpm.mf")
    bpm_coll = mar_collection.MarCollection(bpm_mf_name)

    for filename in filenames:
        #print filename
        #continue
        bpm = echonest_upload(filename)
        bpm_coll.set_item(filename, bpm)

    bpm_coll.write()

mf_name = sys.argv[1]
main(mf_name)


