
function [bpm] = tempo_stem_file(filename)

WRITE_CACHE = 1;
CACHE_OSS = 0;
CACHE_BEATS = 0;
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
	% output of OSS stage
	reference_oss = load('reference/OSS-4-filter.txt');
	delta = oss - reference_oss;
	maxerr = max(abs(delta));
	if maxerr < 1e-13
		printf( 'Testing... OSS ok, maximum deviation %.2g\n', maxerr);
	else
		disp ('Testing... OSS FAILED');
		hold on;
		plot(reference_oss)
		plot(oss, 'g')
		plot(reference_oss - oss, 'r')
		pause
		exit(1)
	end
end


%%%%%%%%% Beat Periods Detection

if not(CACHE_BEATS)
	disp('Calculating new Beat Period Detection')
	tempo_lags = beat_period_detection(oss, oss_sr);
	if WRITE_CACHE
		save 'beat_periods.mat' tempo_lags;
	end
else
	disp('Loading old Beat Periods')
	load('beat_periods.mat');
end

if TEST_REFERENCE
	reference_beat_periods = load('reference/BEATS-5-pulse.txt');
	%delta = reference_beat_periods - tempo_lags;
	delta = reference_beat_periods - tempo_lags;
	maxerr = max(max(abs(delta)));
	if maxerr < 1e-13
		printf( 'Testing... BH ok, maximum deviation %.2g\n', maxerr);
	else
		disp ('Testing... BH FAILED');
		exit(1)
	end
end


%%%%%%%%% ACCUMULATION AND OVERALL ESTIMATE

bpm = accumulator_overall(tempo_lags, oss_sr);



if PLOT
	disp('Hit <enter> to continue');
	pause;
end


