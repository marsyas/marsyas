#!/usr/bin/env python

def energy_in_histo_range(histo, low, high):
    index_low = round(low)
    index_high = round(high)
    if high == 1:
        index_high = len(histo)-1
    if high > len(histo)-1:
        high = len(histo)-1
    if low < 0:
        low = 0
    return sum(histo[index_low:index_high+1])


def info_histogram(bpm, histo, tolerance):
    size = len(histo)
    str1 = 0.0
    bpm2 = 0
    str2 = 0.0
    bpm3 = 0
    str3 = 0.0
    for i in range(1, size-1):
        if histo[i] > str1:
            str1 = histo[i]
    for i in range(1, size-1):
        if ((histo[i] > str2) and (histo[i] < str1)
            and (histo[i-1] < histo[i]) and (histo[i+1]) < histo[i]):
            str2 = histo[i]
            bpm2 = i
    for i in range(1, size-1):
        if ((histo[i] > str3) and (histo[i] < str2) and (histo[i] < str1)
            and (histo[i-1] < histo[i]) and (histo[i+1]) < histo[i]):
            str3 = histo[i]
            bpm3 = i
    num_non_zero = sum( [1 if x > 0 else 0 for x in histo] )

    energy_total = energy_in_histo_range(histo, 0, 1.0)
    energy_under = energy_in_histo_range(histo,
        0, bpm*(1.0-tolerance)) / energy_total
    energy_over= energy_in_histo_range(histo,
        bpm*(1.0+tolerance), 1.0) / energy_total

    str05 = energy_in_histo_range(histo,
        0.5*bpm*(1.0-tolerance), 0.5*bpm*(1.0+tolerance)) / energy_total
    str10 = energy_in_histo_range(histo,
        1.0*bpm*(1.0-tolerance), 1.0*bpm*(1.0+tolerance)) / energy_total
    str20 = energy_in_histo_range(histo,
        2.0*bpm*(1.0-tolerance), 2.0*bpm*(1.0+tolerance)) / energy_total

    info = [ energy_under, energy_over, 1.0 - (energy_under + energy_over),
        str05, str10, str20, 1.0 - (str05+str10+str20),
        float(bpm2) / bpm, float(bpm3) / bpm,
        num_non_zero
        ]
    return info



def late_heuristic(defs, heuristic_tempo, bp):
    #print tempos
    mult = 1.0
    if defs.DOUBLE_TYPE == 1:
        if heuristic_tempo <= 72.5:
            mult = 2.0
    elif defs.DOUBLE_TYPE == 2:
        features = info_histogram(heuristic_tempo, bp, 0.05)
        features.append(heuristic_tempo)
        ## hard-coded values trained elsewhere; see tempo.cpp
        mins = [ 0.0, 0.0, 0.0507398, 0.0, 0.0670043, 0.0,
            -4.44089e-16, 0.0, 0.0, 1.0, 41.0, 0]
        maxs = [ 0.875346, 0.932996, 1.0, 0.535128, 1.0, 0.738602,
        0.919375, 3.93182, 4.02439, 93.0, 178.0, 0 ]
        svm_weights = [
            1.1071, -0.8404, -0.1949, -0.2892,
            -0.2094, 2.1781, -1.369, -0.4589,
            -0.8486, -0.3786, -7.953, 0,
            ]
        svm_sum = 2.1748;

        # normalize
        features_normalized = list(features)
        for i in range(len(features)):
            if mins[i] == maxs[i]:
                continue
            features_normalized[i] = (features[i] - mins[i]) / (maxs[i] - mins[i])
        # svm
        for i in range(len(features)):
            svm_sum += (features_normalized[i] * svm_weights[i])
        if svm_sum > 0:
            mult = 2.0


        if defs.WRITE_LATE:
            out = open("out/double_heuristic_svm.txt", 'w')
            text = " ".join( [str("%g" % v) for v in features ] )
            out.write(text+"\n")

            text = " ".join( [str("%g" % v) for v in features_normalized ] )
            out.write(text+"\n")

            out.write("%g\n" % svm_sum)
            out.close()

    return mult*heuristic_tempo


