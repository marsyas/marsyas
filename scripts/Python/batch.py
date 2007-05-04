import os
from glob import glob

inputDirectory = "../../../../Databases/taslp/";
outputDirectory = "../../../output ";

testCommand = " ";
testCommand = " -q 1 ";

beginCommand = "../../bin/release/peakClustering  ";
beginCommand = "..\\..\\bin\\release\\peakClustering.exe  ";

endCommand = " -P -f -S 0 -k 2 -c 3 -N music -i 250_2500 -o "+outputDirectory;

execStyle=["-T 1 -s 20 -t abfb ", 
		   "-T 1 -s 20 -t abfb -u ", 
		   #virtanen
		   "-T 1 -s 20 -t abfbvo -u ", 
		   "-T 10 -s 20 -t abfbvo -u ", 
		   "-T 1 -s 20 -t abfbvo ", 
		   "-T 10 -s 20 -t abfbvo ",
		   #hwps 
		   "-T 1 -s 20 -t abfbho -u ", 
		   "-T 10 -s 20 -t abfbho -u ", 
		   "-T 1 -s 20 -t abfbho ", 
		   "-T 10 -s 20 -t abfbho ", 
		   # srinivasan
		   " -s 1024 -pp 0 -u  -T 1 -t abfbso ", 
		   "-s 1024 -pp 0 -u -T 10 -t abfbso "];

#execStyle=[" -T 10 -s 20 -t hoabfb -u  "];

for style in execStyle:
  for name in glob(inputDirectory+"*V*.wav"):
    command = beginCommand+style+testCommand+endCommand+name
    print command
    os.system(command)
