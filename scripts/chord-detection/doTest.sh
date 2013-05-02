#!/bin/sh
./doChordID lists/Test.list scratch results/test
./evaluateResults lists/Test.list results/test test.rpt
cat test.rpt
