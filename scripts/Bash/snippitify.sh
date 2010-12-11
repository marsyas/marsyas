#!/bin/bash 

for i in `find -name '*.mp3'`; do 
	mpg123 -m -r 22050 --wav temp.wav $i 
	i=${i/\.\//}
	i=${i/\.mp3/\.au} 
	sox temp.wav temp.au
	mv temp.au $i 
	rm tem.wav
	
done
