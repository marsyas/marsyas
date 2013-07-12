
function [bpm_cand, bphase] = beat_phase(oss, oss_sr, bh_cands)

FAST_DEBUG = 1;
FAST_DEBUG = 0;

WINDOWSIZE = 2048;
HOPSIZE = 128;

BPM_MAX = 180;

buffered = buffer(oss, WINDOWSIZE, WINDOWSIZE - HOPSIZE, 'nodelay');
bp_sr = oss_sr / HOPSIZE;
% only include complete buffers
if mod(length(oss), HOPSIZE) > 0
	buffered = buffered(:,1:end-1);
end

buffered = buffered';

num_frames = size(buffered, 1);
if FAST_DEBUG
	num_frames = 10
end


bphase = zeros(BPM_MAX, 1);

for i = 1:num_frames
	window = buffered(i,:);
	samples = WINDOWSIZE;
	cands = bh_cands(i,:);

	onset_scores = zeros(length(cands),1);
	tempo_scores = zeros(length(cands),1);

	for j = 1:length(cands)
		bpm = cands(j);
		period = int32(60.0 * oss_sr / bpm);

		bp_mags = zeros(period, 1);
		for phase = samples-period:samples-1
			%disp([bpm, period, phase])
			mag = 0.0;
			for b = 0:3
				ind = 1+fix(phase - b*period);
				if ind >= 1
					mag += window(ind);
				end
				%printf("%i\t%f\n", ind, mag);
				ind = 1+fix(phase - b*2*period);
				if ind >= 1
					mag += 0.5*window(ind);
				end
				%printf("%i\t%f\n", ind, mag);
				ind = 1+fix(phase - b*1.5*period);
				if ind >= 1
					mag += 0.5*window(ind);
				end
				%printf("%i\t%f\n", ind, mag);
			end
			bp_mags(samples-1-phase+1) = mag;
		end

		%plot(bp_mags)
		%pause
		%exit(1)
		bp_max = max(bp_mags);
		bp_var = var(bp_mags);

		tempo_scores(j) = bp_max;
		onset_scores(j) = bp_var;
		%printf("%i\t%.2f\t%.2f\n", bpm, bp_max, bp_var);
	end
	tempo_scores /= sum(tempo_scores);
	onset_scores /= sum(onset_scores);

	combo_scores = tempo_scores + onset_scores;
	combo_scores /= sum(combo_scores);

	[beststr, besti] = max(combo_scores);
	bestbpm = int32(cands(besti));

	bphase(1+bestbpm) += beststr;
end

[beststr, besti] = max(bphase);

bpm_cand = besti - 1

