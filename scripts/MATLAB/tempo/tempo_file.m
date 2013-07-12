
function [bpm] = tempo_file(filename)

TEST_REFERENCE = 1;
PLOT = 0;

if PLOT
	hold on
end

%%%%%%%%% OSS
[wav_data, wav_sr, bps] = wavread(filename);
wav_data *= 32767.0 / 32768.0;


[oss, oss_sr] = onset_signal_strength(wav_data, wav_sr);

%%% test OSS
if TEST_REFERENCE
	python_oss = load('reference/onset_strength.txt')(:,2);
	delta = oss - python_oss;
	if max(abs(delta)) < 1e-13
		disp ("Testing... OSS ok");
	else
		disp ("Testing... OSS FAILED");
		%plot(python_oss)
		%plot(oss, 'g')
		plot(python_oss - oss, 'r')
		pause
	end
end

if PLOT
	ts = (( 0:(length(oss)-1) ) / oss_sr).';
	plot(ts, oss);
	combo = [ts oss];
	save 'onset_strength.txt' combo;
end


%%%%%%%%% BH

bh_cands = beat_histogram(oss, oss_sr);
if TEST_REFERENCE
	python_bh = load('reference/beat_histogram.txt');
	% temp for fast checking
	%python_bh = python_bh(1:10,:);
	delta = python_bh - bh_cands;
	if max(abs(delta)) < 1e-12
		disp ("Testing... BH ok");
	else
		disp ("Testing... BH FAILED");
	end
end



%%%%%%%%% BP
[bpm_cand, bphase] = beat_phase(oss, oss_sr, bh_cands);

if TEST_REFERENCE
	python_bp = load('reference/beat_phase.txt')(:,1);
	delta = bphase - python_bp;
	if max(abs(delta)) < 1e-2
		disp ("Testing... BP ok");
	else
		disp ("Testing... BP FAILED");
	end
end


%%%%%%%%% DOUBLING HEURISTIC




if PLOT
	disp("Hit <enter> to continue");
	pause
end


