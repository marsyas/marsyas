#!/usr/bin/env python

import sys
import os.path
import os
import time

import mar_collection
import pyechonest
import pyechonest.track

WAIT_SECONDS = 5.0
PROCESS_PROBLEMS = False
#PROCESS_PROBLEMS = True

def echonest_upload(full_filename):
    print "trying: ", full_filename
    try:
        track = pyechonest.track.track_from_filename(full_filename)
        bpm = track.tempo
        echonest_id = track.id
    except:
        bpm = 0
        echonest_id = 0
    print full_filename, bpm
    time.sleep(WAIT_SECONDS)
    return echonest_id, bpm



def main(mf_name):
    try:
        _ = os.environ["ECHO_NEST_API_KEY"]
    except:
        print "Need echonest key!  you must run:"
        print "  export ECHO_NEST_API_KEY="
        exit(1)
    coll = mar_collection.MarCollection(mf_name)
    if PROCESS_PROBLEMS:
        filenames_z = coll.get_filenames_matching_label("0")
        filenames_zp = coll.get_filenames_matching_label("0.0")
        filenames = filenames_z + filenames_zp
    else:
        filenames = coll.get_filenames()

    id_mf_name = os.path.basename(mf_name)
    id_mf_name = id_mf_name.replace(".mf", "-echonest_id.mf")
    id_coll = mar_collection.MarCollection(id_mf_name)
    bpm_mf_name = os.path.basename(mf_name)
    bpm_mf_name = bpm_mf_name.replace(".mf", "-echonest_bpm.mf")
    bpm_coll = mar_collection.MarCollection(bpm_mf_name)

    problems = []
    for filename in filenames:
        #print filename
        #continue
        echonest_id, bpm = echonest_upload(filename)
        id_coll.set_item(filename, echonest_id)
        bpm_coll.set_item(filename, bpm)
        if bpm is 0:
            problems.append(filename)

    id_coll.write()
    bpm_coll.write()

    if len(problems) > 0:
        print "*** problems:", len(problems)
        out = open("problems.mf", 'w')
        for p in problems:
            out.write("%s\n" % p)
        out.close()


mf_name = sys.argv[1]
main(mf_name)


