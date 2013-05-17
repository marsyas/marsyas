#!/usr/bin/env python

import sys
import os.path

import scipy.io.wavfile

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
#sox /tmp/5_reverb.wav /tmp/noise.wav synth white
#sox /tmp/noise.wav /tmp/quietnoise.wav vol -20d
#sox -m /tmp/5_reverb.wav /tmp/quietnoise.wav \"%(outfile)s\"
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

def process_file(in_out_filename):
    infile, outfile = in_out_filename

    cmd = DISTORTIONS % locals()
    if DEBUG:
        print cmd
    os.system(cmd)

    fs, data = scipy.io.wavfile.read("/tmp/5_reverb.wav")
    SNR = 20.0
    Px = (data**2).sum()
    noise = scipy.rand(len(data)) - 0.5
    Pnoise = (noise**2).sum()
    scaling = scipy.sqrt( (Px/Pnoise) * (10**(-SNR/10)))
    noisy_to_add = noise*scaling
    noisyX = data + scipy.array(noisy_to_add, dtype=scipy.int16)
    #print Px, Pnoise, scaling
    if False:
    #if True:
        import pylab
        pylab.plot(data[:100000])
        pylab.plot(noise[:100000])
        pylab.plot(noisy_to_add[:100000])
        pylab.show()
    scipy.io.wavfile.write(outfile, fs, noisyX)
    #print infile, outfile
    #exit(1)

def process_files(input_dir, output_dir, filenames):
    total = len(filenames)
    for count, in_out_filename in enumerate(filenames):
        process_file(in_out_filename)
        print "Finished %i / %i" % (count+1, total)
        #if DEBUG:
        #    print in_out_filename[1]

def process_mfs(input_dir, output_dir):
    ### get list of mfs
    inmfs = []
    for root, dirnames, filenames in os.walk(input_dir):
        for filename in filenames:
            if filename.endswith(".mf"):
                mf_filename = os.path.join(root, filename)
                inmfs.append( mf_filename )
    ### process
    reldirs = [os.path.relpath(x, input_dir) for x in inmfs]
    outmfs = [os.path.join(output_dir, x) for x in reldirs]
    for inmf, outmf in zip(inmfs, outmfs):
        lines = open(inmf).readlines()
        out = open(outmf, "w")
        for line in lines:
            infilename, bpm = line.split("\t")
            relfilename = infilename.replace("MARSYAS_DATADIR/", "")
            outfilename = os.path.join(output_dir, relfilename)
            text = "%s\t%s" % (outfilename, bpm)
            out.write(text)
        out.close()

def main(input_dir):
    output_dir = os.path.join(
        os.path.split(os.path.dirname(input_dir))[0],
        "distorted")
    #make_dir_tree(input_dir, output_dir)
    #filenames = get_audio_files_to_process(input_dir, output_dir)
    #process_files(input_dir, output_dir, filenames)
    process_mfs(input_dir, output_dir)


if __name__ == '__main__':
    try:
        input_dir = sys.argv[1]
    except:
        print "Need input dir"
        exit(1)
    main(input_dir)


