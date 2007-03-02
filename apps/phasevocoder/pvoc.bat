#!/bin/bash
wish < tcl/pvoc.tcl | ./phasevocoder -n 1024 -w 1024 -d 256 -i 256 -p 1.0 $1
