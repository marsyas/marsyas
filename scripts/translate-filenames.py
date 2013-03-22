#!/usr/bin/env python

from optparse import OptionParser
import os
import csv
import shutil

AUDIO_FILES_PATTERN=('wav','au','mp3','ogg')

DEBUG = False
#DEBUG = True

MOVE_FILES = True
#MOVE_FILES = False


def write_numbers(dirname, csv_filename, number_dirname):
    number_dirname_actual = os.path.join(dirname,
        number_dirname.replace(dirname, ""))
    if not os.path.exists(number_dirname_actual):
        os.mkdir(number_dirname_actual)
    ### recursively find all audio files
    matches = []
    for root, dirnames, filenames in os.walk(dirname):
        for filename in filenames:
            if filename.endswith(AUDIO_FILES_PATTERN):
                joined_filename = os.path.join(root, filename)
                local_filename = os.path.relpath(joined_filename, dirname)
                matches.append(local_filename)
    matches.sort()
    csv_filename_actual = os.path.join(dirname, csv_filename)
    with open(csv_filename_actual, 'wb') as csvfile:
        csvwrite = csv.writer(csvfile, quoting=csv.QUOTE_ALL)
        for i, orig_filename in enumerate(matches):
            ### make new filename
            #base_dirname = os.path.dirname(orig_filename)
            #number_filename = os.path.join(base_dirname,
            #    '%08i.wav' % i)
            number_filename = os.path.join(
                os.path.join(number_dirname_actual,
                '%08i.wav' % i))
            number_filename = os.path.relpath(number_filename, dirname)
            ### save pair to csvfile
            csvwrite.writerow([number_filename, orig_filename])
            if DEBUG:
                print orig_filename, "->", number_filename
            ### actual rename
            if MOVE_FILES:
                shutil.move(
                    os.path.join(dirname, orig_filename),
                    os.path.join(dirname, number_filename))

def write_names(dirname, csv_filename, number_dirname):
    number_dirname_actual = os.path.join(dirname,
        number_dirname.replace(dirname, ""))
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
            ensure_dirname = os.path.dirname(orig_filename)
            if not os.path.exists(ensure_dirname):
                os.makedirs(ensure_dirname)
            ### actual rename
            if MOVE_FILES:
                shutil.move(
                    os.path.join(dirname, number_filename),
                    os.path.join(dirname, orig_filename))
    if MOVE_FILES:
        os.removedirs(number_dirname_actual)
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
                      metavar="DIRECTORY",
                      action="store",
                      help="directory to examine")
    parser.add_option("-m", "--my_dirname", type="string",
                      default="numbers", metavar="DIRECTORY",
                      action="store",
                      help="directory to store numbered files")

    (options, args) = parser.parse_args()
    if options.numbers:
        write_numbers(
            options.dirname, options.csvfile, options.my_dirname)
    elif options.restore:
        write_names(
            options.dirname, options.csvfile, options.my_dirname)
    else:
        print parser.format_help()

if __name__ == "__main__":
    main()

