
function [bpm] = tempo_file(filename)

CACHE_OSS = 0;
CACHE_BH = 0;
CACHE_BP = 0;
TEST_REFERENCE = 0;
PLOT = 0;

if PLOT
	hold on
end

%%%%%%%%% OSS
[wav_data, wav_sr, bps] = wavread(filename);
wav_data = wav_data * 32767.0 / 32768.0;

if not(CACHE_OSS)
	disp('Calculating new OSS')
	[oss, oss_sr] = onset_signal_strength(wav_data, wav_sr);
	save 'oss.mat' oss;
	save 'oss_sr.mat' oss_sr;
else
	disp('Loading old OSS')
	load('oss.mat');
	load('oss_sr.mat');
end

if PLOT
	ts = (( 0:(length(oss)-1) ) / oss_sr).';
	plot(ts, oss);
	combo = [ts oss];
	save 'onset_strength.txt' combo;
end

%%% test OSS
if TEST_REFERENCE
	python_oss = load('reference/onset_strength.txt')(:,2);
	delta = oss - python_oss;
	if max(abs(delta)) < 1e-13
		disp ('Testing... OSS ok');
	else
		disp ('Testing... OSS FAILED');
		%plot(python_oss)
		%plot(oss, 'g')
		plot(python_oss - oss, 'r')
		pause
		exit(1)
	end
end


%%%%%%%%% BH

if not(CACHE_BH)
	disp('Calculating new BH')
	bh_cands = beat_histogram(oss, oss_sr);
	save 'bh.mat' bh_cands;
else
	disp('Loading old BH')
	load('bh.mat');
end

if TEST_REFERENCE
	python_bh = load('reference/beat_histogram.txt');
	% temp for fast checking
	%python_bh = python_bh(1:10,:);
	delta = python_bh - bh_cands;
	if max(abs(delta)) < 1e-12
		disp ('Testing... BH ok');
	else
		disp ('Testing... BH FAILED');
		exit(1)
	end
end


%%%%%%%%% BP
if not(CACHE_BP)
	disp('Calculating new BP')
	[bpm_cand, bphase] = beat_phase(oss, oss_sr, bh_cands);
	save 'bpm_cand.mat' bpm_cand;
	save 'bp.mat' bphase;
else
	disp('Loading old BP')
	load('bpm_cand.mat');
	load('bp.mat');
end

save 'beat_phase.txt' bphase;

if TEST_REFERENCE
	python_bp = load('reference/beat_phase.txt');
	delta = bphase - python_bp;
	if max(abs(delta)) < 1e-3
		disp ('Testing... BP ok');
	else
		disp ('Testing... BP FAILED');
		%plot(bphase)
		%plot(python_bp, 'g')
		plot(abs(delta), 'r')
		pause
		exit(1);
	end
end


%%%%%%%%% DOUBLING HEURISTIC

bpm = late_heuristic(bpm_cand, bphase, 0.05);



if PLOT
	disp('Hit <enter> to continue');
	pause
end


