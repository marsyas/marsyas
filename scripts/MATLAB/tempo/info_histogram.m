
function [info] = info_histogram(bpm_cand, histo, tolerance)

len = length(histo);

[str1, bpm1] = max(histo);
str2 = 0.0;
bpm2 = 0.0;
str3 = 0.0;
bpm3 = 0.0;

for i = 2:len-1
	if ((histo(i) > str2) && (histo(i) < str1) && (histo(i-1) < histo(i)) && (histo(i+1)) < histo(i))
			str2 = histo(i);
			bpm2 = i;
	end
end
for i = 2:len-1
	if ((histo(i) > str3) && (histo(i) < str2) && (histo(i) < str1) && (histo(i-1) < histo(i)) && (histo(i+1)) < histo(i))
			str3 = histo(i);
			bpm3 = i;
	end
end

if bpm2 > 0
	bpm2 = bpm2-1;
end
if bpm3 > 0
	bpm3 = bpm3-1;
end

num_non_zero = 0;
for i = 1:len
	if histo(i) > 0
		num_non_zero = num_non_zero + 1;
	end
end


energy_total = energy_in_range(histo, 0, 1.0);
energy_under = energy_in_range(histo, 0, bpm_cand*(1.0-tolerance)) / energy_total;
energy_over = energy_in_range(histo, bpm_cand*(1.0+tolerance), 1.0) / energy_total;

str05 = energy_in_range(histo, 0.5*bpm_cand*(1.0-tolerance), 0.5*bpm_cand*(1.0+tolerance)) / energy_total;
str10 = energy_in_range(histo, 1.0*bpm_cand*(1.0-tolerance), 1.0*bpm_cand*(1.0+tolerance)) / energy_total;
str20 = energy_in_range(histo, 2.0*bpm_cand*(1.0-tolerance), 2.0*bpm_cand*(1.0+tolerance)) / energy_total;


info = [
	energy_under,
	energy_over,
	1.0 - (energy_under + energy_over),
	str05,
	str10,
	str20,
	1.0 - (str05+str10+str20),
	double(bpm2) / bpm_cand,
	double(bpm3) / bpm_cand,
	double(num_non_zero),
	bpm_cand
	]';



