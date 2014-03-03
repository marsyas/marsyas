
function [tempo_lags] = beat_period_detection(oss, oss_sr)

hold on;

WINDOWSIZE = 2048;
HOPSIZE = 128;

BPM_MAX = 210;
BPM_MIN = 50;

minlag = fix(oss_sr*60 / BPM_MAX);
maxlag = fix(oss_sr*60 / BPM_MIN) + 1;


%%% 1) Overlap
buffered = buffer(oss, WINDOWSIZE, WINDOWSIZE - HOPSIZE, 'nodelay');

% the buffer() function ends up with an extra frame relative to
% marsyas and python
buffered = buffered(:,1:end-1);

beat_detection_sr = oss_sr / HOPSIZE;

%%% 2) Generalized Autocorrelation
N = WINDOWSIZE;
ffts = fft(buffered, 2*N) / (2*N);
ffts_abs = abs(ffts);
ffts_abs_scaled_real = real(power(ffts_abs, 0.5));
scaled = real(ifft(ffts_abs_scaled_real)) * 2*N;
autocorr = scaled(1:N,:)';

num_frames = size(autocorr, 1);



%%% 3) Enhance Harmonics
harmonic_enhanced = zeros( size(autocorr) );
stretched = zeros( 1, WINDOWSIZE );
for i = 1:num_frames
	auto = autocorr(i,:);
	% we don't want to wrap around, but also don't care about
	% anything above maxlag, which is 414
	for l = 0:511
		% weird off-by-one tricks required because
		% matlab/octave have the 1-index arrays
		stretched(l+1) = auto(2*l+1) + auto(4*l+1);
	end
	harmonic_enhanced(i,:) = auto + stretched;

end


%%% 4) Pick peaks
all_lags = zeros(num_frames, 10);
for i = 1:num_frames
	peaks = zeros(1,2);
	k = 1;
	% off-by-one indices again
	for j = minlag+1:maxlag+1
		if ((harmonic_enhanced(i, j-1) < harmonic_enhanced(i, j))
		 && (harmonic_enhanced(i, j)   > harmonic_enhanced(i, j+1)))
			strength = harmonic_enhanced(i, j);
			peaks(k,:) = [strength j];
			k = k+1;
		end
	end
	sorted = sortrows(peaks);
	l = size(sorted,1) - 9;
	best = flipud(sorted(l:end, :));
	best = best(:,2);
	% translate matlab-style indices into tempo lags
	all_lags(i,:) = best - 1;
end


%%% 5) Evaluate pulse trains
tempo_lags = zeros(num_frames, 1);

onset_scores = zeros(10,1);
tempo_scores = zeros(10,1);
for i = 1:num_frames
	window = buffered(:,i);
	samples = int32(WINDOWSIZE);
	cands = all_lags(i,:);

	for j = 1:10
		period = cands(j);

		% perform cross-correlation between the
		% three I_{ P, phi, v } pulse trains, for all
		% possible phases (phi)
		bp_mags = zeros(period, 1);
		for phase = samples-1:-1:samples-period
			phase = int32(phase);

			mag = 0.0;
			for b = 0:3
				b = int32(b);
				ind = phase - b*period;
				% this is I_{ P, phi, 1 }
				if ind >= 0
					mag = mag + window(ind+1);
				end

				% this is I_{ P, phi, 2 }
				ind = phase - b*period*2;
				if ind >= 0
					mag = mag + 0.5*window(ind+1);
				end

				% this is I_{ P, phi, 1.5 }
				ind = phase - idivide(b*period*3, 2);
				if ind >= 0
					mag = mag + 0.5*window(ind+1);
				end
			end
			bp_mags(samples-1-phase+1) = mag;
		end
		bp_max = max(bp_mags);
		bp_var = var(bp_mags, 1);

		tempo_scores(j) = bp_max;
		onset_scores(j) = bp_var;
	end
	% normalize the scores
	tempo_scores = tempo_scores / sum(tempo_scores);
	onset_scores = onset_scores / sum(onset_scores);

	combo_scores = tempo_scores + onset_scores;
	combo_scores = combo_scores / sum(combo_scores);

	[beststr, besti] = max(combo_scores);
	bestlag = cands(besti);

	tempo_lags(i) = bestlag;
end



