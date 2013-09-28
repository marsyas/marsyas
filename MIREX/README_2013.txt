
This file contains instructions for how to run Marsyas 
for various music information retrieval (MIR) tasks 
using the formats proposed in the Music Information 
Retrieval Evaluation Exchange (MIREX). This file 
is for MIREX 2013. 

Quick instructions for compiling Marsyas 
(more detailed instructions can be found in the manual 
which is online at http://marsyas.info - the instructions 
assume that subversion and cmake are available in the system
the revision number is provided separately for each task). 
The last command enters the subdirectory where all the 
Marsyas executables reside. 

> svn -r REVISION_NUMBER co https://svn.code.sf.net/p/marsyas/code/trunk marsyas
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


------------- MIREX 2012 Audio Train/Test Tasks ---------------------------
(use REVISION_NUMBER: 4819) 

Extract features, train classifier and predict for 1 fold: 
./bextract -sv train.txt -tc test.txt -pr test_predicted.txt -od /path/to/workdir -w features.arff 
(the .arff files contains the calculated features in case anyone is interested) 

The executables can be launched in parallel for each fold to 
take advantage of multiple cores without a problem as long as there are different 
scratch directories for each fold. 


------------- MIREX 2012 Audio Tag Classification -------------------------
(use REVISION_NUMBER: 4819) 

Assumes train.txt is a training list file (files and tags) 
and test.txt is a testing list file (just files) 

Step 1) Extract features for both lists 
> bextract -ws 1024 -l 10 -sv -fe train.txt -w train.arff -od /path/to/workdir 
> bextract -ws 1024 -l 10 -sv -fe test.txt -w test.arff -od /path/to/workdir 

These two commands will generate two files in Weka .arff format that 
will be placed in the working directory specified. 

Step 2) First stage automatic tag annotation 

The kea command can take up to 30-50 minutes to compute and does not show any 
progress output until the full model is trained. 

> kea -m tags -id /path/to/workdir -od /path/to/workdir -w train.arff -tw test.arff -pr stage1_affinities.txt 
> ../../scripts/Ruby/threshold_binarization.rb train.txt stage1_affinities > stage1_predictions.txt 

stage1_affinities.txt should contain the predicted tag affinities for the test.txt collection 
and stage1_predictions.txt should contain the predicted tag binary relevance file. 
Although the output of this stage can be directly evaluated we have found that
a second stage of stacked generalization where the tag affinities of each
song are used as feature vectors improves the results in most cases.

The files train.arff.affinities.arff and test.arff.affinities.arff automatically generated 
and used for the second stage. 

Step3) Second stage (stacked generalization) for automatic tag annotation 
> kea -m tags -id /path/to/workdir -od /path/to/workdir -w train.arff.affinities.arff -tw test.arff.affinities.arff -pr stage2_affinities.txt
> ../../scripts/Ruby/threshold_binarization.rb train.txt stage2_affinities.txt > stage2_predictions.txt


----------- MIREX 2012 Audio Similarity ------------------------------
(use REVISION_NUMBER 4819) 

Extract features 
> bextract -fe -sv filelist.txt -od /path/to/workdir -w filelist.arff
Calculate distance matrix 
> cp filelist.txt /path/to/workdir/filelist.txt
> kea -m distance_matrix_MIREX -id /path/to/workdir -od /path/to/workdir -w filelist.arff -pr filelist.txt -dm filelist_matrix.txt 

The generated filelist_matrix.txt contains the full distance matrix of all song 
to all songs of filelist.txt 


---------- MIREX 2012 Audio Key Detection ------------------------ 
(use REVISION_NUMBER 4828) 

> pitchextract -m key -w 16384 -h 4096 input.wav -o output.txt 

where input.wav is the input .wav file (mono, 44100 samping rate) and 
the estimated key (something like G minor) is written to file
output.txt. 




 

