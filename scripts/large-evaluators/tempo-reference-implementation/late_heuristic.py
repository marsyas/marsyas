#!/usr/bin/env python


def late_heuristic(tempos):
    heuristic_tempo = tempos[0]

    for i in range(3):
        for j in range(3):
            if i != j:
                if (abs( 2*tempos[i] - tempos[j]) < 0.04*tempos[j]):
                    if tempos[i] <= 68:
                        heuristic_tempo = 2*tempos[i]

    return heuristic_tempo


