#!/usr/bin/env python

import os
try:
    marsyas_datadir = os.environ['MARSYAS_DATADIR']
except:
    marsyas_datadir = "."

class MarCollection():
    def __init__(self, mf_filename=None):
        self.data = []
        if mf_filename is not None:
            try:
                self.read_mf(mf_filename)
            except:
                pass

    def write(self, mf_filename=None):
        if mf_filename is not None:
            self.filename = mf_filename
        out = open(self.filename, 'w')
        for filename, label in self.data:
            out.write("%s\t%s\n" % (filename, label))
        out.close()

    def read_mf(self, mf_filename):
        self.filename = mf_filename
        self.data = []
        self.merge_mf(mf_filename)

    def merge_mf(self, new_mf_filename):
        lines = open(new_mf_filename).readlines()
        for line in lines:
            if len(line) < 2:
                continue
            if line[0] == '#':
                continue
            splitline = line.split('\t')
            filename = splitline[0].rstrip().replace(
                "MARSYAS_DATADIR", marsyas_datadir)
            try:
                label = splitline[1].rstrip()
            except:
                label = ""
            self.set_item(filename, label)

    def get_filenames(self):
        return [f for f,l in self.data ]

    def get_filename_index(self, filename):
        for i, pair in enumerate(self.data):
            if filename == pair[0]:
                return i, pair
        return False, None

    def set_item(self, filename, label=""):
        index, pair = self.get_filename_index(filename)
        if index is not False:
            self.data[index] = (pair[0], label)
        else:
            self.data.append( (filename, label) )

    def get_filenames_matching_label(self, label_match):
        return [f for f,l in self.data if l == label_match]

