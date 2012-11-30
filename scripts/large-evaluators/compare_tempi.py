#!/usr/bin/env python

import glob
import os
import sys
import subprocess

import scipy.stats
import numpy

#DEBUG_MCNEMAR = True
DEBUG_MCNEMAR = False

ground_truth_dirname = os.path.expanduser("~/src/audio-research/")
#ground_truth_dirname = os.path.expanduser("~/src/audio-research/not-now")
results_subdir = "mfs/"


def load_mf(mf_filename):
    lines = open(mf_filename).readlines()
    coll = []
    for line in lines:
        if line[0] == '#' or len(line) < 2:
            continue
        filename = line.split('\t')[0]
        coll.append( os.path.basename(filename) )
    return coll

def check_files_in_mf(one, two):
    one_coll = load_mf(one)
    two_coll = load_mf(two)
    for o, t in zip(one_coll, two_coll):
        if o != t:
            text = "filenames inside collections do not match!\n%s\n%s\n%s\n%s" % (one, two, o, t)
            #print text
            raise Exception(text)

def get_results(detected_filename, ground_filename):
    #print "--------", detected_filename, ground_filename
    check_files_in_mf(detected_filename, ground_filename)
    wrong_filename = detected_filename.replace(".mf", "-wrong.mf")
    cmd = "tempo -pi %s -m PREDICTED -wo %s %s" % (
        detected_filename, wrong_filename, ground_filename)
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
    return [short_name, m_total_n,
        m_correct_n, m_percent, hm_correct_n, hm_percent]

def gather_results(name, filename_template):
    results = []
    files = sorted(glob.glob(
        os.path.join(results_subdir, "*-%s.mf" % filename_template)))
    # FIXME: temporary remove ivl
    files = [ f for f in files if "ivl" not in f ]
    #print "%s\t%i" % (name, len(files))
    for filename in files:
        splitname = filename.split('-')
        ground_truth = os.path.join(ground_truth_dirname, splitname[0] + ".mf")
        ground_truth = ground_truth.replace(results_subdir, "")
        datum = get_results(filename, ground_truth)
        short_name = datum[0].replace(results_subdir, "")
        results.append( [short_name] + list(datum[1:]) )
        #if "ismir2004" in filename:
        #    ground_truth = ground_truth.replace(
        #        "ismir2004_song_tempos.mf",
        #        "not-now/ismir2004_song_tempos_gonzalez.mf")
        #    datum = get_results(filename, ground_truth)
        #    datum[0] = 'ismir2004_song_gonzalez'
        #    short_name = datum[0].replace(results_subdir, "")
        #    results.append( [short_name] + list(datum[1:]) )
    return results

def mcnemar_stat(mar, dat, harmonic):
    """ see:
        http://en.wikipedia.org/wiki/McNemar%27s_test
    """
    if not harmonic:
        p1 = mar[2]
        n1 = mar[1] - mar[2]
        p2 = dat[2]
        n2 = dat[1] - dat[2]
    else:
        p1 = mar[4]
        n1 = mar[1] - mar[4]
        p2 = dat[4]
        n2 = dat[1] - dat[4]
    #print "mar: %i\tother: %i" % (p1, p2)
    #print dat, p2
    a = p1+p2
    b = p1+n2
    c = n1+p2
    d = n1+n2


    stat = (b-c)**2 / float(b+c)
    #stat = ( abs(b-c) - 1.0)**2 / float(b+c)
    rv = scipy.stats.chi2(1)
    p = rv.sf(stat)
    direction = 1 if p1 > p2 else -1
    if DEBUG_MCNEMAR:
        print "%i\t%i\t%i\t%i\t%i\t%i\t%i\t%.2g\t%i" % (
            a+c+b+d, a, b, c, d, p1, p2, p, direction)
    return p, direction

def sort_names(val):
    examine = val[0]
    if examine.startswith("ismir"):
        return 1
    elif examine.startswith("ball"):
        return 2
    elif examine.startswith("hains"):
        return 3
    elif examine.startswith("genre"):
        return 4
    elif examine.startswith("ivl"):
        return 5
    elif examine.startswith("means"):
        return 6
    elif examine.startswith("total"):
        return 7
    else:
        return 1


def write_csv(filename, collections, dats, field):
    #print "---------------", filename
    out = open(filename, 'w')
    out.write("name, ")
    collections_names = [a[0] for a in collections]
    text = "marsyas, " + ", ".join(collections_names)
    out.write(text + '\n')

    for key, value in iter(sorted(dats.items(), key=sort_names)):
        #print '----', key
        text = key
        if key == 'means':
            out.write('\n')
        for a in value:
            #print a
            if len(a) == 10:
                a = a[1:]
            if len(a) == 9:
                if field == 2:
                    p = a[5]
                    c = '-' if a[7] == 1 else '+'
                elif field == 4:
                    p = a[6]
                    c = '-' if a[8] == 1 else '+'
                sig = ""
                if p < 1e-3:
                    sig = c*3
                elif p < 1e-2:
                    sig = c*2
                elif p < 5e-2:
                    sig = c*1
                text += " , %.02f%s" % (a[field], sig)
            else:
                if len(a) == 6:
                    a = a[1:]
                text += ", %.02f" % (a[field])
        out.write(text + '\n')
    out.close()

def format_header(text):
    # icky, but screw it
    if text == 'ibt_off_auto':
        text = "ibt_oa"
    elif text == 'ibt_off_reg':
        text = "ibt_og"
    elif text == 'qm_default_mean':
        text = "qm_vamp"
    elif text == 'qm_default_median':
        text = "qm_md"
    elif text == 'qm_default_mode':
        text = "qm_mo"
    text = text.replace("_", "\\_")
    text = "\\alg{%s}" % text

    return text

def format_row(text):
    # icky, but screw it
    if text == "acm_mirum":
        text = "ACM MIRUM"
    elif text == "ismir2004_song":
        text = "ISMIR04\\_SONG"
    elif text == "ballroom":
        text = "BALLROOM"
    elif text == "hains":
        text = "HAINSWORTH"
    elif text == "genres":
        text = "GTZAN GENRES"
    elif text == "means":
        text = "Dataset average"
    elif text == "total":
        text = "Total average"
    return text

def write_latex(filename, collections, dats, field):
    out = open(filename, 'w')
    out.write("\\begin{tabular}{lc||c|%s}\n" % ('c'*(len(collections))))
    collections_names = [format_header(a[0]) for a in collections]
    text = "& files & \\alg{marsyas} & " + " & ".join(collections_names)
    out.write(text + '\\\\\n')
    out.write('\hline\n')

    for key, value in iter(sorted(dats.items(), key=sort_names)):
        if key == 'means':
            out.write('\hline\n')
        #text = key.replace("_", "\\_")
        text = key.replace("_tempos", "")
        text = format_row(text)

        if value[0][0] != "means" and value[0][0] != "totals":
            text += " & %i" % (value[0][0])
        else:
            if value[0][0] == "totals":
                text += " & %i" % (1410+465+698+222+1000)
            else:
                text += " & %i" % numpy.mean([1410,465,698,222,1000])

        maxvalue = -1
        for a in value:
            if len(a) == 10:
                a = a[1:]
            if maxvalue < a[field]:
                maxvalue = a[field]
        for a in value:
            if len(a) == 10:
                a = a[1:]
            if len(a) == 9:
                if field == 2:
                    p = a[5]
                    c = '\\texttt{-}' if a[7] == 1 else '\\texttt{+}'
                elif field == 4:
                    p = a[6]
                    c = '\\texttt{-}' if a[8] == 1 else '\\texttt{+}'
                sig = ""
                if p < 1e-3:
                    sig = c*1
                elif p < 1e-2:
                    sig = c*1
                elif p < 5e-2:
                    sig = c*0+'\phantom{%s}' % c
                else:
                    sig = '\phantom{%s}' % c
                if abs(a[field] - maxvalue) < 0.001:
                    text += " & \\textbf{%.1f}%s" % (a[field], sig)
                else:
                    text += " & %.1f$%s$" % (a[field], sig)
            else:
                if len(a) == 6:
                    a = a[1:]
                if abs(a[field] - maxvalue) < 0.001:
                    text += " & \\textbf{%.1f}" % (a[field])
                else:
                    text += " & %.1f" % (a[field])
                text += '$\phantom{\\texttt{+}}$'
                #text += " & %.1f" % (a[field])
        out.write(text + '\\\\\n')
    out.write("\\end{tabular}")
    out.close()


def get_means_totals(data):
    m_mean_percent = 0
    m_mean_count = 0
    m_overall_correct = 0
    m_overall_total = 0
    h_mean_percent = 0
    h_mean_count = 0
    h_overall_correct = 0
    h_overall_total = 0
    for d in data:
        m_mean_percent += d[3]
        m_mean_count += 1
        h_mean_percent += d[5]
        h_mean_count += 1
        m_overall_correct += d[2]
        m_overall_total += d[1]
        h_overall_correct += d[4]
        h_overall_total += d[1]
    m_mean_percent /= float(m_mean_count)
    h_mean_percent /= float(h_mean_count)
    m_overall_correct_p = m_overall_correct / float(m_overall_total)
    h_overall_correct_p = h_overall_correct / float(h_overall_total)
    m_overall_correct_p *= 100.0
    h_overall_correct_p *= 100.0

    means = [ m_overall_total,
       0, m_mean_percent,
       0, h_mean_percent]
    totals = [ h_overall_total,
       m_overall_correct, m_overall_correct_p,
       h_overall_correct, h_overall_correct_p]
    return means, totals


def main():
    mar_results = gather_results("marsyas", "detected")
    dats = {}
    for a in mar_results:
        dats[a[0]] = []
        dats[a[0]].append(a[1:])
    mar_m, mar_t = get_means_totals(mar_results)
    mar_m.insert(0, 'means')
    mar_t.insert(0, 'totals')
    dats["means"] = []
    dats["total"] = []
    dats["means"].append(mar_m)
    dats["total"].append(mar_t)


    collections = [
            ("gkiokas", "gkiokas"),
            ("zplane", "zplane"),
            ("klapuri", "klapuri"),
            ("echonest", "echonest_bpm"),
            #("lartillot", "lartillot"),
            ("ibt", "ibt"),
            #("ibt_off_auto", "ibt-off-auto"),
            #("ibt_off_reg", "ibt-off-reg"),
            ("qm_default_mean", "qm_default_mean"),
            #("qm_default_median", "qm_default_median"),
            #("qm_default_mode", "qm_default_mode"),
            ("scheirer", "scheirer"),
            #("vamp_fixed", "fixed_tempo"),
        ]
    if DEBUG_MCNEMAR:
        print "#n\ta\tb\tc\td\tp"
    for name, template in collections:
        data = gather_results(name, template)
        for d in data:
            shortname = d[0]
            dats[shortname].append(d[1:])

            mar = None
            for f in mar_results:
                if f[0] == shortname:
                    mar = f
            if DEBUG_MCNEMAR:
                print shortname, name
            p_mirex, d_mirex = mcnemar_stat(mar, d, False)
            p_harmonic, d_harmonic = mcnemar_stat(mar, d, True)
            dats[shortname][-1].append(p_mirex)
            dats[shortname][-1].append(p_harmonic)
            dats[shortname][-1].append(d_mirex)
            dats[shortname][-1].append(d_harmonic)
        m, t = get_means_totals(data)
        if DEBUG_MCNEMAR:
            print "totals"
            print t
        m.insert(0, 'means')
        t.insert(0, 'totals')
        #print '------------- %s' % name
        #print "totals"
        p_mirex, d_mirex = mcnemar_stat(mar_t, t, False)
        #print p_mirex, d_mirex
        #print '------------- %s' % name
        p_harmonic, d_harmonic = mcnemar_stat(mar_t, t, True)
        #print p_harmonic, d_harmonic
        #print '-------------'
        t.append( p_mirex)
        t.append( p_harmonic)
        t.append( d_mirex)
        t.append( d_harmonic)

        dats["means"].append(m)
        dats["total"].append(t)

    ### TODO: sort based on mirex tempo?
    s_coll = collections
    s_dats = dats

    write_csv("mirex.csv", s_coll, s_dats, 2)
    write_csv("harmonic.csv", s_coll, s_dats, 4)
    write_latex("mirex.latex", s_coll, s_dats, 2)
    write_latex("harmonic.latex", s_coll, s_dats, 4)

if __name__ == '__main__':
    main()


