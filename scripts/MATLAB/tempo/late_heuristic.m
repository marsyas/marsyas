
function [bpm] = late_heuristic(bpm_cand, bphase)

TYPE = 2;

mult = 1.0;

if TYPE == 1
	if bpm_cand <= 72.5
		mult = 2.0;
	end
end
if TYPE == 2
	features = info_histogram(bpm_cand, bphase, 0.05);
	mins = [ 0.0, 0.0, 0.0507398, 0.0, 0.0670043, 0.0, -4.44089e-16, 0.0, 0.0, 1.0, 41.0, 0];
	maxs = [ 0.875346, 0.932996, 1.0, 0.535128, 1.0, 0.738602, 0.919375, 3.93182, 4.02439, 93.0, 178.0, 0];
	svm_weights = [ 1.1071, -0.8404, -0.1949, -0.2892, -0.2094, 2.1781, -1.369, -0.4589, -0.8486, -0.3786, -7.953, 0 ];
	svm_sum = 2.1748;

	features_normalized = zeros(size(features));
	for i = 1:length(features)
		if mins(i) ~= maxs(i)
			features_normalized(i) = ((features(i) - mins(i)) / (maxs(i) - mins(i)));
		end
	end

	for i = 1:length(features_normalized)
		svm_sum = svm_sum + (features_normalized(i) * svm_weights(i));
	end

	if svm_sum > 0
		mult = 2.0;
	end

	if 0
		features
		features_normalized
		svm_sum
		mult
	end
end

bpm = mult * bpm_cand;

