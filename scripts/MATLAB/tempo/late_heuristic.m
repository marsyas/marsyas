
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

	mins = [ 0.0, 0.0, 0.0320684, 0.0, 0.0320684, 0.0, -4.44089e-16, 0.0, 0.0, 1.0, 50.0, 0 ];
	maxs = [ 0.876178, 0.94753, 1.0, 0.535006, 1.0, 0.738607, 0.89892, 3.10526, 3.12281, 92.0, 178.0, 0 ];
	svm_weights = [ 1.8788, -1.6107, -0.124, -0.2659, -0.4155, 2.5116, -1.458, -0.7464, -0.4488, 0.1006, -8.0225, 0, ];

	features_normalized = zeros(size(features));
	for i = 1:length(features)
		if mins(i) != maxs(i)
			features_normalized(i) = ( (features(i) - mins(i))
				/ (maxs(i) - mins(i)));
		end
	end

	svm_sum = 0.0;
	for i = 1:length(features_normalized)
		svm_sum += (features_normalized(i) * svm_weights(i))
	end

	if svm_sum > 0
		mult = 2.0;
	end

	features
	features_normalized
	svm_sum
	mult
end

bpm = mult * bpm_cand;

