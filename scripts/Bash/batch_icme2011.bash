rm *.txt
mkcollection -c collection.mf $1
pitchextract collection.mf 
pitchdtw N4*.txt N1*.txt N3*.txt > result.txt
../../scripts/Ruby/icme2011_average-precision-recall.rb result.txt

