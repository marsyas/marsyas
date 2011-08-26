


Examples for pitch_plots.py: 

(Display the command-line arguments) 
python pitch_plots.py -h

(Display a correlogram. Each column is an autocorrelation
function.)

python pitch_plots.py -i ~/data/sound/neptune_timeline/orchive-2005-449A-5min.wav -o foo.png -m correlogram -w 2048 -s 24 -e 33 -p "Correlogram"

(ADMFgram: each column is an Average Magnitude Difference function) 
 
python pitch_plots.py -i
~/data/sound/neptune_timeline/orchive-2005-449A-5min.wav -o foo.png -m
amdfogram-w 4096 -s 24 -e 33 -p "AMDFgram" -c hot
