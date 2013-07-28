
function [bh_cands] = beat_histogram(oss, oss_sr)

FAST_DEBUG = 1;
FAST_DEBUG = 0;

WINDOWSIZE = 2048;
HOPSIZE = 128;

BPM_MAX = 180;
BPM_MIN = 40;


buffered = buffer(oss, WINDOWSIZE, WINDOWSIZE - HOPSIZE, 'nodelay');
%buffered = buffer(oss, 2048, 1920, 'nodelay');

% the buffer() function ends up with an extra frame relative to
% marsyas and python
buffered = buffered(:,1:end-1);

bh_sr = oss_sr / HOPSIZE;

%%% generalized autocorrelation
N = WINDOWSIZE;
ffts = fft(buffered, 2*N) / (2*N);
ffts_abs = abs(ffts);
ffts_abs_scaled_real = real(power(ffts_abs, 0.5));
scaled = real(ifft(ffts_abs_scaled_real)) * 2*N;
autocorr = scaled(1:N,:);

num_frames = size(autocorr, 2);
if FAST_DEBUG
	num_frames = 1
end

if 0
	hold on;
	python_bh = load('aq-10.txt');
    python_bh = python_bh(:,2);
	a = autocorr(:, 10);
	%plot(python_bh, 'b');
	%plot(a, 'g');
	plot(python_bh - a, 'r');
	pause
	exit(1)
end

%%% beat histogram
Hn = zeros(num_frames, 4*BPM_MAX);
for i = 1:num_frames
	prev_Hni = 4*BPM_MAX - 1;
	pprev_Hni = prev_Hni;
	sumamp = 0.0;
	count = 1;

	for j = 3:WINDOWSIZE-1
		factor = 8/2;
		Hni = int32((oss_sr * 60.0 * factor / (j+1)));
		if Hni < 4*BPM_MAX
			% j+1 for off-by-one error between matlab and C++
			amp = autocorr(j+1,i);
			%printf("%i\t%i\t%f\n", j, Hni, amp);
			if amp < 0
				amp = 0;
			end
			if prev_Hni == Hni
				sumamp = sumamp + amp;
				count = count + 1;
			else
				sumamp = sumamp + amp;
				Hn(i, 1+prev_Hni) = sumamp / double(count);
				sumamp = 0.0;
				count = 1;
			end
			%%% linear interpolate non-set bins
			if pprev_Hni - prev_Hni > 1
				x0 = double(prev_Hni);
				x1 = double(pprev_Hni);
				y0 = Hn(i, 1+prev_Hni);
				y1 = Hn(i, 1+pprev_Hni);
				for k = prev_Hni+1:pprev_Hni
					Hn(i, 1+k) = y0 + (y1-y0)*(k-x0) / (x1-x0);
				end
			end
			pprev_Hni = prev_Hni;
			prev_Hni = Hni;
		end
	end
end

if 0
	hold on;
	python_bh = load('bh-10.txt');
    python_bh = python_bh(:,2)';
	a = Hn(10, :);
	%plot(python_bh, 'b');
	%plot(a, 'g');
	plot(python_bh - a, 'r');
	pause
	exit(1)
end


%%%%%%%%% time stretch, add
harmonic_strengthened_bh = zeros( size(Hn) );
M = 4*BPM_MAX;
for i = 1:num_frames
	% direct translation of marsyas C++
	factor2 = 0.5;
	factor4 = 0.25;
	stretched = zeros( 1, M );
	for t = 0:M-1
		ni = t*factor2;
		li = fix(ni); % numSamples
		%exit(1)
		ri = li + 1;
		w = ni - li;
		%printf("%i\t%i\t%f\t%f\n", li, ri, w, ni);
		if ri < M - 1
			stretched(1+t) = stretched(1+t) + Hn(i, 1+li) + w * (Hn(i, 1+ri) - Hn(i, 1+li));
		else
			stretched(1+t) = stretched(1+t) + Hn(i, 1+t);
		end

		ni = t*factor4;
		li = fix(ni); % numSamples
		ri = li + 1;
		w = ni - li;
		if ri < M - 1
			stretched(1+t) = stretched(1+t) + Hn(i, 1+li) + w * (Hn(i, 1+ri) - Hn(i, 1+li));
		else
			stretched(1+t) = stretched(1+t) + Hn(i, 1+t);
		end
	end
	%exit(1)
	harmonic_strengthened_bh(i,:) = Hn(i,:) + stretched;
end

if 0
	hold on;
	python_bh = load('hbh-10.txt');
    python_bh = python_bh(:,2)';
	a = harmonic_strengthened_bh(10,:);
	%plot(python_bh, 'b');
	%plot(a, 'g');
	plot(python_bh - a, 'r');
	pause
	exit(1)
end


%%%%%%%%% find peaks
bh_cands = zeros(num_frames, 10);
for i = 1:num_frames
	peaks = zeros(1,2);
%	cell( );
	k = 1;
	for j = 4*BPM_MIN:4*BPM_MAX-1
		if ((harmonic_strengthened_bh(i, j-1) < harmonic_strengthened_bh(i, j)) && (harmonic_strengthened_bh(i, j) > harmonic_strengthened_bh(i, j+1)))
			strength = harmonic_strengthened_bh(i, j);
			peaks(k,:) = [strength j];
			k = k+1;
		end
	end
	sorted = sortrows(peaks);
	l = size(sorted,1) - 9;
	best = flipud(sorted(l:end, :));
    best = best(:,2);
	% translate matlab-style indices into BPM
	bh_cands(i,:) = (best-1.0) / 4.0;
end


