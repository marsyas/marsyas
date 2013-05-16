#!/usr/bin/env python

import sys
import os.path

DEBUG = False
#DEBUG = True

DISTORTIONS = [
    ("distorted", """
sox \"%(infile)s\" -r8000 /tmp/0_r8000.wav rate
sox /tmp/0_r8000.wav -r8000 /tmp/1_gsm.gsm
sox /tmp/1_gsm.gsm -r44100 -s /tmp/2_gsm-post.wav rate
sox /tmp/2_gsm-post.wav /tmp/3_filtered.wav sinc 500-2000
sox /tmp/3_filtered.wav /tmp/4_volboost.wav vol 1.8
sox /tmp/4_volboost.wav /tmp/5_reverb.wav reverb
sox /tmp/5_reverb.wav /tmp/noise.wav synth white
sox /tmp/noise.wav /tmp/quietnoise.wav vol -20d
sox -m /tmp/5_reverb.wav /tmp/quietnoise.wav \"%(outfile)s\"
"""),
#"sox %(infile)s %(outfile)s reverb 1 2000 1000 700 750 760 880"),
#    ("noise",
#"sox %(infile)s %(noisefile)s synth white; sox %(noisefile)s %(small_noisefile)s vol -20d; sox -m %(infile)s %(small_noisefile)s %(outfile)s"),
    ]
AUDIO_FILES_PATTERN=('wav','au','mp3','ogg')


def make_dir_tree(input_dir, output_dir):
    dirs = [x[0] for x in os.walk(input_dir)]
    reldirs = [os.path.relpath(x, input_dir) for x in dirs]
    for reldir in reldirs:
        mydir = os.path.join(output_dir, reldir)
        if not os.path.exists(mydir):
            os.makedirs(mydir)

def get_audio_files_to_process(input_dir, output_dir):
    matches = []
    for root, dirnames, filenames in os.walk(input_dir):
        for filename in filenames:
            if filename.endswith(AUDIO_FILES_PATTERN):
                input_filename = os.path.join(root, filename)
                output_filename = os.path.join(
                    output_dir,
                    os.path.relpath(input_filename, input_dir))
                matches.append( (input_filename, output_filename) )
    return matches

def process_file(in_out_filename, cmd):
    infile, outfile = in_out_filename

    cmd = cmd % locals()
    if DEBUG:
        print cmd
    os.system(cmd)


def main(input_dir):
    for dist in DISTORTIONS:
        name, cmd = dist
        print "-----", name
        output_dir = os.path.join(
            os.path.split(os.path.dirname(input_dir))[0],
            name
            )
        make_dir_tree(input_dir, output_dir)
        filenames = get_audio_files_to_process(input_dir, output_dir)
        if DEBUG:
            filenames = filenames[:1]

        total = len(filenames)
        for count, in_out_filename in enumerate(filenames):
            process_file(in_out_filename, cmd)
            print "Finished %i / %i" % (count+1, total)
            #if DEBUG:
            #    print in_out_filename[1]


if __name__ == '__main__':
    try:
        input_dir = sys.argv[1]
    except:
        print "Need input dir"
        exit(1)
    main(input_dir)


