
function [bpm_cand, bphase] = beat_phase(oss, oss_sr, bh_cands)

FAST_DEBUG = 1;
FAST_DEBUG = 0;

WINDOWSIZE = 2048;
HOPSIZE = 128;

BPM_MAX = 180;

buffered = buffer(oss, WINDOWSIZE, WINDOWSIZE - HOPSIZE, 'nodelay');

% the buffer() function ends up with an extra frame relative to
% marsyas and python
buffered = buffered(:,1:end-1);

bp_sr = oss_sr / HOPSIZE;

buffered = buffered';

num_frames = size(buffered, 1);
if FAST_DEBUG
	num_frames = 1
end


%ref = load("reference/bp-accum.txt");

bphase = zeros(BPM_MAX, 1);
onset_scores = zeros(10,1);
tempo_scores = zeros(10,1);

for i = 1:num_frames
	window = buffered(i,:);
	samples = int32(WINDOWSIZE);
	cands = bh_cands(i,:);

	for j = 1:10
		bpm = cands(j);
		period = int32(60.0 * oss_sr / bpm);

		bp_mags = zeros(period, 1);

		%printf("----\n");
		for phase = samples-1:-1:samples-period
			phase = int32(phase);

			mag = 0.0;
			for b = 0:3
				b = int32(b);
				ind = phase - b*period;
				if ind >= 0
					mag = mag + window(ind+1);
				end
				%printf("\t%i\t%f\n", ind, mag);

				ind = phase - b*period*2;
				if ind >= 0
					mag = mag + 0.5*window(ind+1);
				end
				%printf("\t%i\t%f\n", ind, mag);

				ind = phase - idivide(b*period*3, 2);
				if ind >= 0
					mag = mag + 0.5*window(ind+1);
				end
				%printf("\t%i\t%f\n", ind, mag);
			end
			bp_mags(samples-1-phase+1) = mag;
		end
		bp_max = max(bp_mags);
		bp_var = var(bp_mags, 1);

		tempo_scores(j) = bp_max;
		onset_scores(j) = bp_var;
		%if (i > 1) && (j == 9)
	%		save "mags.txt" bp_mags
	%		exit(1)
			%plot(bp_mags)
			%pause
	%	end
	%	printf("%.2f\t%i\t%.9f\t%.9f\n", bpm, period, bp_max, bp_var);
	end
	%exit(1)
	%printf("\n");
	%if i > 1
%		exit(1)
%	end
	tempo_scores = tempo_scores / sum(tempo_scores);
	onset_scores = onset_scores / sum(onset_scores);

	combo_scores = tempo_scores + onset_scores;
	combo_scores = combo_scores / sum(combo_scores);

	[beststr, besti] = max(combo_scores);
	bestbpm = int32(cands(besti));

	%printf("%i\t%.9f\n", bestbpm, beststr);

	bphase(1+bestbpm) = bphase(1+bestbpm) + beststr;

	%d_bpm = bestbpm - ref(i,1);
	%d_str = beststr - ref(i,2);
	%printf("%i\t%i\t%g\t%g\n", bestbpm, ref(i,1), beststr, ref(i,2));
	%if (d_bpm > 0) || (d_str > 1e-5)
%		printf("%i\t%i\t%g\tPROBLEM, DELTAS\n", i, d_bpm, d_str);
%	end
end

[beststr, besti] = max(bphase);

bpm_cand = besti - 1;

