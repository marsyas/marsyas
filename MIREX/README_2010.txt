
This file contains instructions for how to run Marsyas 
for various music information retrieval (MIR) tasks 
using the formats proposed in the Music Information 
Retrieval Evaluation Exchange (MIREX). This file 
is for MIREX 2010. 

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

IMPORTANT: (For the SAI/VQ method of feature extraction, please enable the WITH_ANN option in CMake)

> make 
> cd build/bin 


------------- MIREX 2010 Audio Classification Tasks ---------------------
(use REVISION_NUMBER: 4178) 

Extract features, train classifier and predict for 1 fold: 
./bextract -sv -bf train.txt -tc test.txt -pr test_predicted.txt -od /path/to/workdir -w features.arff 
(the .arff file contains the calculated features in case anyone is interested) 

The executables can be launched in parallel for each fold to take 
advantage of multiple cores without a problem as long as there 
are different scratch directories for each fold. 

Multi-core version (Requires Ruby to be installed)
This assumes a big unlabelled feature extraction filelist from 
which features are computed followed by train/predict collection 
for each fold that are used for classification and prediction. 

From the Marsyas top-level directory: 
cd src/apps/mirex2010 

Feature Extraction Step (while it is running the output of the program
can be observed in the files stdout1.txt,...stdoutN.txt: 
> ./extractFeatures.rb path_to_bextract #cores path_to_scratch_folder path_to_feature_extract_list.txt features.arff "-sv -fe -bf"

Train/Classify Step: 
> ./trainAndClasify.rb path_to_kea path_to_scratch folder
path_train.txt path_to_test.txt path_to_predicted_output.txt





------------- MIREX 2010 Audio Classification Tasks SAI/VQ -------- 
Vector Quantized Stabilized Auditory Image 
 SAI/VQ - Vector Quantized Stabilized Auditory Image ---------------------
(use REVISION_NUMBER: 4178) 

IMPORTANT:

When building Marsyas, please enable the "WITH_ANN" flag in cmake.

Extract features, train classifier and predict for 1 fold: 
./bextract -saivq -sv train.txt -tc test.txt -pr test_predicted.txt -od /path/to/workdir -w features.arff 
(the .arff file contains the calculated features in case anyone is interested) 

The executables can be launched in parallel for each fold to take 
advantage of multiple cores without a problem as long as there 
are different scratch directories for each fold. 

Multi-core version (Requires Ruby to be installed)
This assumes a big unlabelled feature extraction filelist from 
which features are computed followed by train/predict collection 
for each fold that are used for classification and prediction. 

From the Marsyas top-level directory: 
cd src/apps/mirex2010 

Feature Extraction Step (while it is running the output of the program
can be observed in the files stdout1.txt,...stdoutN.txt: 
> ./extractFeatures.rb path_to_bextract #cores path_to_scratch_folder path_to_feature_extract_list.txt features.arff "-sv -fe -saivq"

Train/Classify Step: 
> ./trainAndClasify.rb path_to_kea path_to_scratch folder
path_train.txt path_to_test.txt path_to_predicted_output.txt






---------------  MIREX 2010 Audio Tag Classification --------------------------
(for more information check ACM Multimedia 2009 paper: 
"Improving Automatic Music Tag Annotation Using Stacked Generalization Of Probabilistic SVM 
Outputs" 
) 
(use REVISION_NUMBER 4178)) 
Assumes train.txt is a training list file (files and tags) and 
test.txt is a testing list file (just files) 

Step 1) Extract features for both lists 
> bextract -ws 1024 -as 400 -sv -fe -bf train.txt -w train.arff -od /path/to/workdir 
> bextract -ws 1024 -as 400 -sv -fe -bf test.txt -w test.arff -od /path/to/workdir 

These two commands will generate two files in Weka .arff format 
that will be placed in the working_directory specified. 

Step 2) First stage automatic tag annotation 

The kea command can take up to 30-50 minutes to compute and does not show any progress output 
until the full model is trained. 

> kea -m tags -id /path/to_working_dir -od /path/to/workdir -w train.arff -tc test.arff -pr stage1_affinities.txt 
> ../../scripts/Ruby/threshold_binarization.rb train.txt stage1_affinities.txt > stage1_predictions.txt


stage1_affinities.txt should contain the predicted tag affinities for the test.txt collection 
and stage1_predictions.txt should contain the predicted tag binary relevance file. 
Although the output of this stage can be directly evaluated we have found that 
a second stage of stacked generalization where the tag affinities of each 
song are used as feature vectors improves the results in most cases. 
In addition it gerates the files stacked_train.arff and stacked_test.arff which 
are used for the second stage describe below. 

Step3) Second stage (stacked generalization) for automatic tag annotation 
> kea -m tags -id /path/to_workdir -od /path/to/workdir -w stacked_train.arff -tc stacked_test.arff -pr stage2_affinities.txt 
> ../../scripts/Ruby/threshold_binarization.rb train.txt stage2_affinities.txt > stage2_predictions.txt 





---------------  MIREX 2010 Audio Similarity --------------------------
(use 4178) 



Extract features: 
> bextract -fe -sv -bf filelist.txt -od /path/to/workdir -w filelist.arff

> kea -m distance_matrix_MIREX -id /path/to/workdir -od /path/to/workdir -w filelist.arff -pr filelist.txt -dm filelist_matrix.txt 

The generated filelist_matrix.txt contains the full distance matrix of all songs 
to all songs of filelist.txt.  



------------- MIREX 2010 Audio Onset Detection Tasks ---------------------
(use REVISION_NUMBER: 4194))

./onsets soundExample.wav

Resulting soundExample.output will be saved in the same directory of the onsets executable.


-------------- MIREX 2010 Audio Tempo Estimation ------------------
(use REVISION_NUMBER: 4206) 

./tempo soundFile.wav -f output_filename.txt 

output_filename contains the output (two tempi and their relative strength tab-separated) of the tempo estimation. 


------------- MIREX 2010 Audio Beat Tracking Tasks ---------------------
(use REVISION_NUMBER: 4194))

This implementation contains two distinct versions, differentiated by its functional heuristics.
The application outputs two text files: one with the tempo measure (median IBI, 
in BPMs) - XXX_medianTempo.txt; and other with the beat times, in seconds - XXX.txt.
For running each of them just use the following commands:

1-> ./ibt -s "squareCorr" input.wav outputDir/output.txt
2-> ./ibt -s "regular" input.wav outputDir/output.txt

NOTE: If no ouput directory is assigned, the executable dir and the audio filename will be assumed.
If only the output directory is assigned, the audio filename will be assumed.
