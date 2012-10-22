#!/usr/bin/env python

import glob
import os
import sys
import subprocess

import scipy.stats

ground_truth_dirname = os.path.expanduser("~/src/audio-research/")


def get_results(detected_filename, ground_filename):
    #print "--------", detected_filename
    cmd = "tempo -pi %s -m PREDICTED %s" % (
        detected_filename, ground_filename)
    #print cmd
    results = subprocess.check_output(cmd, shell=True)
    reslist = results.split('\n')
    ending = reslist[-15:]
    #print ending
    for line in ending:
        if line.find('Correct MIREX Prediction') >= 0:
            nums = line.split()[4]
            m_correct_n = int(nums.split('/')[0])
            m_total_n = int(nums.split('/')[1])
            m_percent = float(line.split()[6])
            #print line
        if line.find('Correct Harmonic MIREX prediction') >= 0:
            nums = line.split()[5]
            hm_correct_n = int(nums.split('/')[0])
            hm_total_n = int(nums.split('/')[1])
            hm_percent = float(line.split()[7])
    if m_total_n != hm_total_n:
        raise Exception("Fatal error: Ns do not match!")
    short_name = detected_filename.split('-')[0]
    #print "%s\t%i\t%i\t%i\t%i\t%i" % (short_name, m_total_n,
    #    m_correct_n, m_percent, hm_correct_n, hm_percent)
    return (short_name, m_total_n,
        m_correct_n, m_percent, hm_correct_n, hm_percent)

def gather_results(name, filename_template):
    results = []
    files = sorted(glob.glob("*-%s.mf" % filename_template))
    for filename in files:
        splitname = filename.split('-')
        ground_truth = os.path.join(ground_truth_dirname, splitname[0] + ".mf")
        datum = get_results(filename, ground_truth)
        short_name = datum[0]
        results.append( [short_name] + list(datum[1:]) )
    return results

def mcnemar_stat(mar, dat):
    p1 = mar[2]
    n1 = mar[1] - mar[2]
    p2 = dat[2]
    n2 = dat[1] - dat[2]
    a = p1+p2
    b = p1+n2
    c = n1+p2
    d = n1+n2
    stat = ( abs(b-c) - 1)**2 / float(b+c)
    rv = scipy.stats.chi2(1)
    p = rv.sf(stat)
    #print p

def write_csv(filename, collections, dats, field):
    out = open(filename, 'w')
    out.write("name, ")
    collections_names = [a[0] for a in collections]
    text = "marsyas, " + ", ".join(collections_names)
    out.write(text + '\n')

    for key, value in iter(sorted(dats.items())):
        text = key + ", "
        percents = [v[field] for v in value]
        #percents_harmonic_mirex = [v[4] for v in value]
        #percents = percents_mirex + percents_harmonic_mirex
        percents_text = ["%.1f" % a for a in percents]
        text += ", ".join(percents_text)
        out.write(text + '\n')
    out.close()


def main():
    mar_results = gather_results("marsyas", "detected")
    dats = {}
    mcnemar = {}
    for a in mar_results:
        dats[a[0]] = []
        dats[a[0]].append(a[1:])
        mcnemar[a[0]] = []

    collections = [
            ("vamp_fixed", "fixed_tempo"),
            ("qm_default_mean", "qm_default_mean"),
            ("qm_default_median", "qm_default_median"),
            ("qm_default_mode", "qm_default_mode"),
            ("ibt", "ibt"),
            ("ibt_off_auto", "ibt-off-auto"),
            ("ibt_off_reg", "ibt-off-reg"),
            ("scheirer", "scheirer"),
            ("echonest", "echonest_bpm"),
        ]
    for name, template in collections:
        data = gather_results(name, template)
        for d in data:
            shortname = d[0]
            dats[shortname].append(d[1:])
            mar = None
            for f in mar_results:
                if f[0] == shortname:
                    mar = f
            mcnemar_stat(mar, d)

    write_csv("mirex.csv", collections, dats, 2)
    write_csv("harmonic.csv", collections, dats, 4)

main()


