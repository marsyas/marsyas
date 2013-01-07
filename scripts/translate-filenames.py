#!/usr/bin/env python

from optparse import OptionParser
import os
import csv
import shutil

AUDIO_FILES_PATTERN=('wav','au','mp3','ogg')

DEBUG = False


def write_numbers(dirname, csv_filename):
    ### recursively find all audio files
    matches = []
    for root, dirnames, filenames in os.walk(dirname):
        for filename in filenames:
            if filename.endswith(AUDIO_FILES_PATTERN):
                joined_filename = os.path.join(root, filename)
                local_filename = joined_filename.replace(dirname, "")
                matches.append(local_filename)
    csv_filename_actual = os.path.join(dirname, csv_filename)
    with open(csv_filename_actual, 'wb') as csvfile:
        csvwrite = csv.writer(csvfile)
        for i, orig_filename in enumerate(matches):
            ### make new filename
            base_dirname = os.path.dirname(orig_filename)
            number_filename = os.path.join(base_dirname,
                '%08i.wav' % i)
            ### save pair to csvfile
            csvwrite.writerow([number_filename, orig_filename])
            if DEBUG:
                print filename, "->", number_filename
            ### actual rename
            shutil.move(
                os.path.join(dirname, orig_filename),
                os.path.join(dirname, number_filename))

def write_names(dirname, csv_filename):
    ### allow user to specify csv_filename either relative or absolute
    csv_filename_actual = csv_filename
    if not os.path.exists(csv_filename_actual):
        csv_filename_actual = os.path.join(dirname, csv_filename)

    with open(csv_filename_actual, 'rb') as csvfile:
        csvreader = csv.reader(csvfile)
        for row in csvreader:
            number_filename = row[0]
            orig_filename = row[1]
            if DEBUG:
                print number_filename, "->", orig_filename
            ### actual rename
            shutil.move(
                os.path.join(dirname, number_filename),
                os.path.join(dirname, orig_filename)
                )
    os.remove(csv_filename_actual)


def main():
    parser = OptionParser()
    parser.add_option("-n", "--numbers",
                      default=False, action="store_true",
                      help="change filenames to numbers, write csv")
    parser.add_option("-r", "--restore",
                      default=False, action="store_true",
                      help="change filenames to text from csv")
    parser.add_option("-c", "--csvfile", type="string",
                      default="filenames-numbers.csv",
                      metavar="FILENAME",
                      action="store",
                      help="change filenames to text from csv")
    parser.add_option("-d", "--dirname", type="string",
                      default=".", metavar="DIRECTORY",
                      action="store",
                      help="directory to examine")

    (options, args) = parser.parse_args()
    if options.numbers:
        write_numbers(options.dirname, options.csvfile)
    elif options.restore:
        write_names(options.dirname, options.csvfile)
    else:
        print parser.format_help()

if __name__ == "__main__":
    main()

