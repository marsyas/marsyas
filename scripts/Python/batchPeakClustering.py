import os
from glob import glob

beginCommand = "./peakClustering  "
endCommand = " -a -s -p 2 -c 3 -o ~/output -N music  ";
for name in glob("../../../Database/*V.wav"):
  command = beginCommand+name+endCommand
  print command
  os.system(command)
