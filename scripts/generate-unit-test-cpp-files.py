#!/usr/bin/env python

'''
Helper script to generate CxxTest runner files (xxx_runner.cpp) 
from test suite header files (Testxxx.h)
in the unit test folder (src/tests/unit_tests)
using the CxxTest python script cxxtestgen.py.

Copyright 2010 Stefaan Lippens.
'''

import os
import sys
import glob
import re
import subprocess
import logging


def main():
    # Change to unit test folder.
    unit_test_folder = os.path.join(os.path.split(__file__)[0], '../src/tests/unit_tests')
    os.chdir(unit_test_folder)

    # Get a list of test suite header files.
    test_suite_header_files = glob.glob('Test*.h')

    # Run the cxxtestgen tool on the test suite header files.
    for header_file in test_suite_header_files:
        generate_test_runner(header_file)


def generate_test_runner(header_file):
    '''Generate a test runner cpp file from a test suite header file.'''
    # Strip "Test" and ".h" to get the name
    name = header_file[4:-2]
    # Make the test runner file name
    test_runner = name + '_runner.cpp'
    logging.info('Building test runner %s from "%s"' % (test_runner, header_file))
    # Exectute the cxxtestgen.py script.
    job = [
        '../../otherlibs/cxxtest/cxxtestgen.py',
        '--error-printer',
        '-o', test_runner,
        header_file
        ]
    logging.debug('Running command: ' + ' '.join(job))
    subprocess.call(args=job)

if __name__ == '__main__':
    logging.basicConfig(level=logging.INFO)
    main()
