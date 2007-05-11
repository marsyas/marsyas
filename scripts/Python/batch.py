import os
from glob import glob

inputDirectory = "../../../../Databases/taslp/";
outputDirectory = "../../../output3 ";

testCommand = " ";
#testCommand = " -q 1 ";

beginCommand = "../../bin/release/peakClustering  ";
beginCommand = "..\\..\\bin\\release\\peakClustering.exe  ";

endCommand = " -P -f -S 0 -r -k 2 -c 3 -N music -i 250_2500 -o "+outputDirectory;

execStyle=[
		   #hwps 
		   "-T 1 -s 20 -t hoabfb ", 
		   "-T 10 -s 20 -t hoabfb ", 
		   "-T 1 -s 20 -t hoabfb -u ", 
		   "-T 10 -s 20 -t hoabfb -u ", 
		   #virtanen
		   "-T 1 -s 20 -t voabfb ", 
		   "-T 10 -s 20 -t voabfb ",
		   "-T 1 -s 20 -t voabfb -u ", 
		   "-T 10 -s 20 -t voabfb -u ", 
           #srinivasan criterion
		   "-T 1 -s 20 -t soabfb ", 
		   "-T 10 -s 20 -t soabfb ",
		   "-T 1 -s 20 -t soabfb -u ", 
		   "-T 10 -s 20 -t soabfb -u ", 
           # amplitude only
           "-T 1 -s 20 -t abfb ", 
		   "-T 1 -s 20 -t abfb -u ", 
           # harmonicity only
           "-T 1 -s 20 -t ho ", 
		   "-T 1 -s 20 -t ho -u ", 
           "-T 1 -s 20 -t vo ", 
		   "-T 1 -s 20 -t vo -u ", 
           "-T 1 -s 20 -t so ", 
		   "-T 1 -s 20 -t so -u ", 
		   # srinivasan algo
		   " -s 1024 -npp -u  -T 1 -t soabfb ", 
		   "-s 1024 -npp -u -T 10 -t soabfb "];

for style in execStyle:
  for name in glob(inputDirectory+"*V*.wav"):
    command = beginCommand+style+testCommand+endCommand+name
    print command
    os.system(command)
