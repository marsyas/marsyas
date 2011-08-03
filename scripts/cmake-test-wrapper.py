#!/usr/bin/env python

import os
import sys
import subprocess

try:
    working_dir = sys.argv[1]
    env_vars = sys.argv[2]
    bin_cmd_args = sys.argv[3:]
except:
    print "Syntax:  cmake-test-wrapper.py WORKING_DIR ENV_VARS CMD ARG1 ARG2 ... ARGn "
    sys.exit(0)

def run_command(bin_cmd_args, working_dir, env_vars):
    cmd = env_vars + ' ' + ' '.join(bin_cmd_args)
    print "DIR:\t" + working_dir
    print "ENV_VARS:\t" + env_vars
    print "CMD:\t" + cmd
    os.chdir(working_dir)
    p = subprocess.Popen(cmd, shell=True)
    p.communicate()
    return p.returncode

if __name__ == "__main__":
    return_code = run_command(bin_cmd_args, working_dir, env_vars)
    sys.exit(return_code)

