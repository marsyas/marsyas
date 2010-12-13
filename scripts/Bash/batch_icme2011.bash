rm *.txt
mkcollection -c collection.mf $1
pitchextract -m yin collection.mf 
pitchdtw N4*.txt N1*.txt N3*.txt > result.txt
ls N1*.txt N4*.txt N3*.txt > 4cl.txt 
python ../../scripts/Python/icme2011_distance_matrix_from_pitch_contours.py 4cl.txt result_features.txt
python ../../scripts/Python/icme2011_distance_matrix_resampled.py 4cl.txt 100 result_resamped.txt
ruby ../../scripts/Ruby/icme2011_average-precision-recall.rb result.txt

