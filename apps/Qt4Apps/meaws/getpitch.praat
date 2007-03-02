form File_name
	sentence file_name foo.wav
endform
out_file$ = "praatPitchList.txt"
Read from file... 'file_name$'
To Pitch (ac)... 0 200 2 yes 0.03 0.45 0.01 0.35 0.14 550
Kill octave jumps
Write to text file... 'out_file$'
