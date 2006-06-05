create AudioSource src 
create AudioSink dest

create Series pvseries1 
create PvShiftInput si
create PvFold fo
create Spectrum spk 
create PvConvert conv
create PvOscBank ob 
create PvShiftOutput so 
create Gain gt 

add si > pvseries1
add fo > pvseries1
add spk > pvseries1
add conv > pvseries1
add ob > pvseries1
add so > pvseries1
add gt > pvseries1 



updctrl pvseries1 PvShiftInput/si/natural/Decimation 512
updctrl pvseries1 PvShiftInput/si/natural/WindowSize 2048
updctrl pvseries1 PvFold/fo/natural/FFTSize 2048
updctrl pvseries1 PvFold/fo/natural/WindowSize 2048
updctrl pvseries1 PvFold/fo/natural/Decimation 512
updctrl pvseries1 PvConvert/conv/natural/Decimation 512
updctrl pvseries1 PvOscBank/ob/natural/Interpolation 512
updctrl pvseries1 PvOscBank/ob/real/PitchShift 1.0
updctrl pvseries1 PvShiftOutput/so/natural/Interpolation 512
updctrl pvseries1 PvShiftOutput/so/natural/WindowSize 2048
updctrl pvseries1 PvShiftOutput/so/natural/Decimation 512
updctrl pvseries1 PvConvert/conv/natural/Sinusoids 100



create Series pvseries2 
create PvShiftInput si1
create PvFold fo1
create Spectrum spk1 
create PvConvert conv1
create PvOscBank ob1 
create PvShiftOutput so1 
create Gain gt1 


add si1 > pvseries2
add fo1 > pvseries2
add spk1 > pvseries2
add conv1 > pvseries2
add ob1 > pvseries2
add so1 > pvseries2
add gt1 > pvseries2 


updctrl pvseries2 PvShiftInput/si1/natural/Decimation 512
updctrl pvseries2 PvShiftInput/si1/natural/WindowSize 2048
updctrl pvseries2 PvFold/fo1/natural/FFTSize 2048
updctrl pvseries2 PvFold/fo1/natural/WindowSize 2048
updctrl pvseries2 PvFold/fo1/natural/Decimation 512
updctrl pvseries2 PvConvert/conv1/natural/Decimation 512
updctrl pvseries2 PvOscBank/ob1/natural/Interpolation 512
updctrl pvseries2 PvOscBank/ob1/real/PitchShift 0.5
updctrl pvseries2 PvShiftOutput/so1/natural/Interpolation 512
updctrl pvseries2 PvShiftOutput/so1/natural/WindowSize 2048
updctrl pvseries2 PvShiftOutput/so1/natural/Decimation 512
updctrl pvseries2 PvConvert/conv1/natural/Sinusoids 100


create Series total
add src > total 
create Fanout fanout 
add pvseries1 > fanout 
add pvseries2 > fanout 

create Sum sum 
add fanout > total
add sum > total
add dest > total 

updctrl total AudioSource/src/natural/inSamples 512
# updctrl total SoundFileSource/src/string/filename /home/gtzan/data/sound/music_speech/speech/allison.au

link total bool/notEmpty AudioSource/src/bool/notEmpty 
