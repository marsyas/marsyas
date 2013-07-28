
function [bpm] = tempo_stem_file(filename)

WRITE_CACHE = 0;
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
	if WRITE_CACHE
		save 'oss.mat' oss;
		save 'oss_sr.mat' oss_sr;
	end
else
	disp('Loading old OSS')
	load('oss.mat');
	load('oss_sr.mat');
end

if PLOT
	ts = (( 0:(length(oss)-1) ) / oss_sr).';
	plot(ts, oss);
	combo = [ts oss];
end

%%% test OSS
if TEST_REFERENCE
	python_oss = load('reference/onset_signal_strength.txt');
	delta = oss - python_oss;
	maxerr = max(abs(delta));
	if maxerr < 1e-13
		printf( 'Testing... OSS ok, maximum deviation %.2g\n', maxerr);
	else
		disp ('Testing... OSS FAILED');
		plot(python_oss - oss, 'r')
		pause
		exit(1)
	end
end


%%%%%%%%% BH

if not(CACHE_BH)
	disp('Calculating new BH')
	bh_cands = beat_histogram(oss, oss_sr);
	if WRITE_CACHE
		save 'bh.mat' bh_cands;
	end
else
	disp('Loading old BH')
	load('bh.mat');
end

if TEST_REFERENCE
	python_bh = load('reference/beat_histogram.txt');
	% temp for fast checking
	%python_bh = python_bh(1:10,:);
	delta = python_bh - bh_cands;
	maxerr = max(max(abs(delta)));
	if maxerr < 1e-13
		printf( 'Testing... BH ok, maximum deviation %.2g\n', maxerr);
	else
		disp ('Testing... BH FAILED');
		exit(1)
	end
end


%%%%%%%%% BP
if not(CACHE_BP)
	disp('Calculating new BP')
	[bpm_cand, bphase] = beat_phase(oss, oss_sr, bh_cands);
	if WRITE_CACHE
		save 'bpm_cand.mat' bpm_cand;
		save 'bp.mat' bphase;
	end
else
	disp('Loading old BP')
	load('bpm_cand.mat');
	load('bp.mat');
end

if TEST_REFERENCE
	python_bp = load('reference/beat_phase.txt');
	delta = bphase - python_bp;
	maxerr = max(max(abs(delta)));
	if maxerr < 1e-3
		printf( 'Testing... BP ok, maximum deviation %.2g\n', maxerr);
	else
		printf( 'Testing... BP FAILED, maximum deviation %.2g\n', maxerr);
		%plot(bphase)
		%plot(python_bp, 'g')
		plot(abs(delta), 'r')
		pause
		exit(1);
	end
end


%%%%%%%%% DOUBLING HEURISTIC

bpm = late_heuristic(bpm_cand, bphase);



if PLOT
	disp('Hit <enter> to continue');
	pause;
end


