#!/usr/bin/env python


def late_heuristic(bpm1, bpm2, bh_bpm):
    tempos = [bpm1, bpm2, bh_bpm]
    print tempos
    heuristic_tempo = bpm1

    for i in range(3):
        for j in range(3):
            if i != j:
                if (abs( 2*tempos[i] - tempos[j]) < 0.04*tempos[j]):
                    if tempos[i] <= 68:
                        heuristic_tempo = 2*tempos[i]

    print heuristic_tempo

    return 0


