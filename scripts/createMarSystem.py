#!/usr/bin/env python

'''
Script to generate skeleton files for a new MarSystem.

Usage:
    createMarSystem.py NameOfNewMarSystem

This will create the files NameOfNewMarSystem.h and NameOfNewMarSystem.cpp
if the current directory.
'''

import os
import sys


def create_from_template(template_file, template_name, target_file, target_name):
    '''
    Create a MarSystem based on a given template.

    Copy the lines from the given template_file to target_file and replace
    the occurences of template_name to target_name.

    Also remove the '//' style comments, but not the '/* */' and '///' ones.
    '''

    # If file already exists: quit.
    if os.path.exists(target_file):
        raise ValueError('file "%s" already exists' % target_file)

    template = open(template_file, 'r')
    target = open(target_file, 'w')

    for line in template:
        # Skip comment lines starting with '// '
        # (other comments like '/* */' and '///' are kept.)
        if line.strip() == '//' or line.strip()[:3] == '// ':
            continue
        # Replace the template_name with the target_name
        line = line.replace(template_name, target_name)
        # Replace the upper case versions too (for the #define stuff)
        line = line.replace(template_name.upper(), target_name.upper())
        target.write(line)

    template.close()
    target.close()


if __name__ == '__main__':

    # Get target name from command line.
    if len(sys.argv) == 2:
        target_name = sys.argv[1]
    else:
        print >>sys.stderr, 'usage: %s MarSystemName' % sys.argv[0]
        sys.exit(1)

    # Get the base directory of Marsyas
    marsyas_base_dir = os.path.normpath(os.path.join(os.path.dirname(__file__), '..'))
    print 'Marsyas base directory:', marsyas_base_dir

    # The template file.
    template_name = 'MarSystemTemplateBasic'

    for ext in ['.h', '.cpp']:
        # Construct the file names.
        template_file = os.path.join(marsyas_base_dir, 'src',
            'marsyas', 'marsystems', template_name + ext)
        target_file = target_name + ext

        # Do the copy and replace stuff.
        create_from_template(template_file, template_name, target_file, target_name)
        print 'created "%s" from template "%s"' % (target_file, template_file)
