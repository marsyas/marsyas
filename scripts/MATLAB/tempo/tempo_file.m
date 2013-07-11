
function [bpm] = tempo_file(filename)

TEST_REFERENCE = 0;
PLOT = 0;

if PLOT
	hold on
end

%%%%%%%%% OSS
[wav_data, wav_sr, bps] = wavread(filename);

[oss, oss_sr] = onset_signal_strength(wav_data, wav_sr);

%%% test OSS
python_oss = load('reference/onset_strength.txt')(:,2);
if max(oss - python_oss) < 1e-03
	disp ("Testing... OSS ok")
else
	disp ("Testing... OSS FAILED")
	plot(python_oss)
	plot(oss, 'g')
	plot(python_oss - oss, 'r')
end

if PLOT
	ts = (( 0:(length(oss)-1) ) / oss_sr).';
	plot(ts, oss);
	combo = [ts oss];
	save 'onset_strength.txt' combo;
end


%%%%%%%%% BH

bh_cands = beat_histogram(oss, oss_sr);
python_bh = load('reference/beat_histogram.txt');




if PLOT
	disp("Hit <enter> to continue");
	pause
end

bpm = 60;

