import os
from glob import glob

beginCommand = "peakClustering.exe -a -s 2 -c 3 -k 2 -i 0_300 -o c:\output\\bass -p 1_-1_0.05_-1 "

for name in glob("..\..\..\jazz\*.wav"):
  command = beginCommand+name
  print command
  os.system(command)


beginCommand = "peakClustering.exe -a -s 2 -c 3 -k 2 -i 250_2500 -o c:\output\up -p 1_-1_0.2_-1 "

for name in glob("..\..\..\jazz\*.wav"):
  command = beginCommand+name
  print command
  os.system(command)
