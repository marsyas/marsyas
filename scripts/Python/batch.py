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
		   "-T 1 -s 20 -t voabfb -u ", 
		   "-T 10 -s 20 -t voabfb -u ", 
		   "-T 1 -s 20 -t voabfb ", 
		   "-T 10 -s 20 -t voabfb ",
		   #hwps 
		   "-T 1 -s 20 -t hoabfb -u ", 
		   "-T 10 -s 20 -t hoabfb -u ", 
		   "-T 1 -s 20 -t hoabfb ", 
		   "-T 10 -s 20 -t hoabfb ", 
		   # srinivasan
		   " -s 1024 -pp 0 -u  -T 1 -t soabfb ", 
		   "-s 1024 -pp 0 -u -T 10 -t soabfb "];

#execStyle=[" -T 10 -s 20 -t hoabfb -u  "];

for style in execStyle:
  for name in glob(inputDirectory+"*V*.wav"):
    command = beginCommand+style+testCommand+endCommand+name
    print command
    os.system(command)
