#!/usr/bin/python

#
# Read in a .json file and pipe it into a Django database
#

import sys
import os
import datetime
import commands
import re

from django.core.management import setup_environ
import settings
setup_environ(settings)

if len(sys.argv) != 1:
    print "Usage: populate_database.py in.json"
    exit(0);


    


