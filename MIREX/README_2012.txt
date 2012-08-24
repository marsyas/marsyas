
This file contains instructions for how to run Marsyas 
for various music information retrieval (MIR) tasks 
using the formats proposed in the Music Information 
Retrieval Evaluation Exchange (MIREX). This file 
is for MIREX 2012. 

Quick instructions for compiling Marsyas 
(more detailed instructions can be found in the manual 
which is online at http://marsyas.info - the instructions 
assume that subversion and cmake are available in the system
the revision number is provided separately for each task). 
The last command enters the subdirectory where all the 
Marsyas executables reside. 

> svn -r REVISION_NUMBER co https://marsyas.svn.sourceforge.net/svnroot/marsyas/trunk marsyas 
> cd marsyas 
> mkdir build 
> cd build 
> ccmake ../src 


------------- MIREX 2012 Audio Beat Tracking Tasks ---------------------
(use REVISION_NUMBER: 4767)

The algorithm contains two distinct versions, differentiated by its mode of operation.
The application outputs two text files: one with the tempo measure (median IBI, 
in BPMs) - XXX_medianTempo.txt; and other with the beat times, in seconds - XXX.txt.
For running each version of the algorithm use the following commands:

1-> ./ibt input.wav outputDir/output.txt
2-> ./ibt -off "regular" input.wav outputDir/output.txt
3-> ./ibt -off -i "auto-regen" input.wav outputDir/output.txt

NOTE: If no ouput directory is assigned, the executable dir and the audio filename will be assumed.
If only the output directory is assigned, the audio filename will be assumed.
