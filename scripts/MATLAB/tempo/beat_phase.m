
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
	num_frames = 1
end


bphase = zeros(BPM_MAX, 1);

for i = 1:num_frames
	window = buffered(i,:);
	samples = int32(WINDOWSIZE);
	cands = bh_cands(i,:);

	onset_scores = zeros(length(cands),1);
	tempo_scores = zeros(length(cands),1);

	for j = 1:length(cands)
		bpm = cands(j);
		period = int32(60.0 * oss_sr / bpm);

		bp_mags = zeros(period, 1);

		for phase = samples-1:-1:samples-period
			phase = int32(phase);
			%printf("# %i\n", phase);

			mag = 0.0;
			for b = 0:3
				b = int32(b);
				ind = phase - b*period;
				if ind >= 1
					mag += window(ind+1);
				end
				%printf("\t%i\t%f\n", ind, mag);

				ind = phase - b*period*2;
				if ind >= 1
					mag += 0.5*window(ind+1);
				end
				%printf("\t%i\t%f\n", ind, mag);

				ind = phase - idivide(b*period*3, 2);
				if ind >= 1
					mag += 0.5*window(ind+1);
				end
				%printf("\t%i\t%f\n", ind, mag);
			end
			bp_mags(samples-1-phase+1) = mag;
			%printf("%f", mag);
			%exit(1)
			%disp([samples-1-phase+1, mag])
			%if j == 4
		%		printf("%i\t%.9f", phase, mag);
			%	exit(1)
			%end
		end
		%save 'mags.txt' bp_mags
		%exit(1)

		%plot(bp_mags)
		%pause
		%exit(1)
		bp_max = max(bp_mags);
		bp_var = var(bp_mags, 1);

		tempo_scores(j) = bp_max;
		onset_scores(j) = bp_var;
		%printf("%.2f\t%i\t%.9f\t%.9f\n", bpm, period, bp_max, bp_var);
		%if j == 4
	%		blah = window';
%			save "input.txt" blah;
%			save "foo.txt" bp_mags;
%			exit(1)
%		end
	end
	%exit(1)
	tempo_scores /= sum(tempo_scores);
	onset_scores /= sum(onset_scores);

	combo_scores = tempo_scores + onset_scores;
	combo_scores /= sum(combo_scores);

	[beststr, besti] = max(combo_scores);
	bestbpm = int32(cands(besti));

	%printf("%i\t%.9f\n", bestbpm, beststr);

	bphase(1+bestbpm) += beststr;
end

[beststr, besti] = max(bphase);

bpm_cand = besti - 1;

