#!/usr/bin/env python

import sys
import os.path
import csv



def main():
    mf_filename = sys.argv[1]
    csv_filename = sys.argv[2]

    out_mf_filename = os.path.join(
        os.path.dirname(csv_filename),
        os.path.basename(mf_filename))
    mf_lines = open(mf_filename).readlines()
    
    filenames_labels = {}
    for line in mf_lines:
        filename = line.split('\t')[0]
        filename = filename.replace(
            os.path.dirname(csv_filename), '')
        label = line.split('\t')[1].rstrip()
        #label = label.replace('"','')
        filenames_labels[filename] = label

    with open(out_mf_filename, 'w') as out_mf:
        with open(csv_filename, 'rb') as csvfile:
            csvreader = csv.reader(csvfile)
            for row in csvreader:
                number_filename = row[0]
                orig_filename = row[1]
                label = filenames_labels[orig_filename]

                out_mf.write("%s\t%s\n" % (number_filename, label))

main()

