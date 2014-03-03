
function [info] = info_histogram(lag, histo, tolerance, bpm)

energy_total = energy_in_range(histo, 0, 1.0);
energy_under = energy_in_range(histo, 0, lag-tolerance) / energy_total;

str05 = energy_in_range(histo, 0.5*lag-tolerance, 0.5*lag+tolerance) / energy_total;


info = [
	energy_under,
	str05,
	bpm
	]';



