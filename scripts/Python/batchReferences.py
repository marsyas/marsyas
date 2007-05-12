import os
from glob import glob

inputDirectory = "../../../Databases/taslp/";
outputDirectory = "../../../references ";

testCommand = " ";
#testCommand = " -q 1 ";

beginCommand = "../../bin/release/peakClustering  ";
#beginCommand = "..\\..\\bin\\release\\peakClustering.exe  ";

endCommand = " -P -S 0 -r -i 250_2500 -o "+outputDirectory;

execStyle=[
		   # reference 
		   "-T 1 -s 20 "
		   ];

for style in execStyle:
  for name in glob(inputDirectory+"*V*.wav"):
    command = beginCommand+style+testCommand+endCommand+name
    print command
    os.system(command)
