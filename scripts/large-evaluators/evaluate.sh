#!/bin/sh
DATA_DIR=~/src/audio-research/
OUT_DIR=~/src/icassp2013/bpm/

./check-ibt.py ${DATA_DIR} ${OUT_DIR}/ibt/ > ibt.txt
./check-ibt-off-auto.py ${DATA_DIR} ${OUT_DIR}/ibt-off-auto/ > ibt-off-auto.txt
./check-ibt-off-regular.py ${DATA_DIR} ${OUT_DIR}/ibt-off-reg/ > ibt-off-reg.txt
./check-marsyas-tempo.py ${DATA_DIR} ${OUT_DIR}/marsyas/ > mar.txt
./check-scheirer.py ${DATA_DIR} ${OUT_DIR}/scheirer/ > scheirer.txt
#./check-vamp-fixed.py ${DATA_DIR} ${OUT_DIR}/vamp-fixed/ > vamp-fixed.txt
./check-vamp-qm-default.py ${DATA_DIR} ${OUT_DIR}/vamp-qm/ > vamp-qm.txt


