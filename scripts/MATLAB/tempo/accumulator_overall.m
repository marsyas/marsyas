
function [bpm] = accumulator_overall(tempo_lags, oss_sr)

pdf = normpdf(-1000:999, 0, 10);

accum = zeros(414,1);
for i = 1:length(tempo_lags)
	lag = tempo_lags(i);
	begin = 1000-lag;
	endindex = 1000-lag+413;
	accum += pdf(begin:endindex)';
end

%ref = load('reference/ACCUM-2-sum-final.txt')
%hold on
%plot(ref, 'g')
%plot(accum, 'b')
%pause

%plot(accum)
%pause

[val index] = max(accum);
lag = index-1;
bpm = oss_sr*60 / (lag-1);


TYPE = 2;

mult = 1.0;

if TYPE == 1
	if bpm_cand <= 71.9
		mult = 2.0;
	end
end
if TYPE == 2
	features = info_histogram(lag, accum, 10, bpm);
	mins = [ 0.0321812, 1.68126e-83, 50.1745 ];
	maxs = [ 0.863237, 0.449184, 208.807 ];
	svm_weights51 = [ -1.9551, 0.4348, -4.6442, 3.2896 ];
	svm_weights52 = [ -3.0408, 2.7591, -6.5367, 3.081 ];
	svm_weights12 = [ -3.4624, 3.4397, -9.4897, 1.6297 ];

	features_normalized = zeros(size(features));
	for i = 1:length(features)
		features_normalized(i) = ((features(i) - mins(i)) / (maxs(i) - mins(i)));
	end

	svm_sum51 = svm_weights51(end);
	for i = 1:length(features_normalized)
		svm_sum51 += (features_normalized(i) * svm_weights51(i));
	end
	svm_sum52 = svm_weights52(end);
	for i = 1:length(features_normalized)
		svm_sum52 += (features_normalized(i) * svm_weights52(i));
	end
	svm_sum12 = svm_weights12(end);
	for i = 1:length(features_normalized)
		svm_sum12 += (features_normalized(i) * svm_weights12(i));
	end

	if (svm_sum52 > 0) && (svm_sum12 > 0)
		mult = 2.0;
	end
	if (svm_sum51 <= 0) && (svm_sum52 <= 0)
		mult = 0.5;
	end

	if 0
		features
		features_normalized
		svm_sum51
		mult
	end
end

bpm = mult * bpm;

