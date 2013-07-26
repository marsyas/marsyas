
function [energy] = energy_in_range(histo, low, high)

index_low = int32(low);
index_high = int32(high);
if high == 1
	index_high = length(histo)-1;
end
if high > length(histo)-1
	index_high = length(histo)-1;
end
if low < 1
	index_low = 0;
end

energy = sum(histo(1+index_low:1+index_high));



