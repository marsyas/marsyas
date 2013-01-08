#!/usr/bin/env python

import os
import sys
import subprocess
import filecmp
import shutil

try:
    working_dir = sys.argv[1]
    env_vars = sys.argv[2]
    bin_cmd_args = sys.argv[3:-1]
    good_filename = sys.argv[-1]
except:
    print "Syntax:  cmake-test-wrapper.py WORKING_DIR ENV_VARS CMD ARG1 ARG2 ... ARGn "
    sys.exit(0)

def run_command(bin_cmd_args, working_dir, env_vars):
    cmd = env_vars + ' ' + ' '.join(bin_cmd_args)
    print "DIR:\t" + working_dir
    print "ENV_VARS:\t" + env_vars
    print "CMD:\t" + cmd
    os.chdir(working_dir)
    print cmd
    p = subprocess.Popen(cmd, shell=True)
    p.communicate()
    #return p.returncode

if __name__ == "__main__":
    #print '----'
    #print sys.argv
    #new_filename = os.path.abspath(os.path.join(
    #    sys.argv[8], '../output',
    #    os.path.basename(sys.argv[6])))
    new_filename = sys.argv[6]
    print "comparing ", new_filename, good_filename
    run_command(bin_cmd_args, working_dir, env_vars)
    # python truth values are opposite to return values!
    return_code = not filecmp.cmp(new_filename, good_filename)
    sys.exit(return_code)

