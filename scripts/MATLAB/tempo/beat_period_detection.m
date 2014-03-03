
function [bh_cands] = beat_period_detection(oss, oss_sr)

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
autocorr = scaled(1:N,:);

num_frames = size(autocorr, 2);

%ref = load('foo.txt');

%%%%%%%%% time stretch, add
harmonic_enhanced = zeros( size(autocorr) )';
for i = 1:num_frames
	stretched = zeros( 1, WINDOWSIZE );
	for j = 1:511
		stretched(j) = autocorr(2*j) + autocorr(4*j);
	end
	%plot(harmonic_enhanced(:,i))
	%plot(autocorr(:,i))
	%plot(stretched(1:end-1), 'g')
	harmonic_enhanced(i,:) = autocorr(:,i)' + stretched;
	%plot(harmonic_enhanced(1:end-1,i), 'g')
	%plot(ref(2:end), 'r')
	%pause
	%exit(1)
end


%ref = load('foo.txt');
%compare = harmonic_strengthened(:,1);
%hold on
%plot(ref)
%plot(compare, 'g')
%pause
%delta = ref - compare;
%max(delta)
%exit(1)



%%%%%%%%% find peaks
all_peaks = zeros(num_frames, 10);
for i = 1:num_frames
	peaks = zeros(1,2);
%	cell( );
	k = 1;
	for j = minlag:maxlag
		if ((harmonic_enhanced(i, j-1) < harmonic_enhanced(i, j)) && (harmonic_enhanced(i, j) > harmonic_enhanced(i, j+1)))
			strength = harmonic_enhanced(i, j);
			peaks(k,:) = [strength j];
			k = k+1;
		end
	end
	sorted = sortrows(peaks);
	l = size(sorted,1) - 9;
	best = flipud(sorted(l:end, :));
	best = best(:,2)
	exit(1)
	% translate matlab-style indices into BPM
	all_peaks(i,:) = (best-1.0) / 4.0;
end


