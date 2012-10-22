#!/bin/sh
DATA_DIR=~/src/audio-research/
OUT_DIR=~/bpm/

./check-marsyas-tempo.py ${DATA_DIR} ${OUT_DIR}/marsyas/ > mar.txt
./check-vamp-fixed.py ${DATA_DIR} ${OUT_DIR}/vamp-fixed/ > vamp-fixed.txt
./check-vamp-qm-default.py ${DATA_DIR} ${OUT_DIR}/vamp-qm/ > vamp-qm.txt
./check-ibt.py ${DATA_DIR} ${OUT_DIR}/ibt/ > ibt.txt
./check-ibt-off-auto.py ${DATA_DIR} ${OUT_DIR}/ibt-off-auto/ > ibt-off-auto.txt
./check-ibt-off-regular.py ${DATA_DIR} ${OUT_DIR}/ibt-off-reg/ > ibt-off-reg.txt
./check-scheirer.py ${DATA_DIR} ${OUT_DIR}/scheirer/ > scheirer.txt


