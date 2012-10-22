#!/usr/bin/env python

import sys

import mar_collection

base = sys.argv[1]
new = sys.argv[2]

base_coll = mar_collection.MarCollection(base)
base_coll.merge_mf(new)
base_coll.write()


