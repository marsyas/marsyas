
function [oss, oss_sr] = onset_signal_strength(wav_data, wav_sr)


WINDOWSIZE = 256;
HOPSIZE = 128;
%LOWPASS_N = 15;
%LOWPASS_HZ = 6;

%%% create overlapped signal
buffered = buffer(wav_data, WINDOWSIZE, WINDOWSIZE-HOPSIZE);
%buffered = buffer(wav_data, WINDOWSIZE, WINDOWSIZE-HOPSIZE, 'nodelay');
oss_sr = wav_sr / HOPSIZE;

% only include complete buffers
if mod(length(wav_data), HOPSIZE) > 0
	buffered = buffered(:,1:end-1);
end

%%% windowing and log-magnitude spectrum
% match Marsyas hamming window
ns = 0:WINDOWSIZE-1;
hamm = 0.54 - 0.46 * cos( 2*pi*ns / (WINDOWSIZE-1));

windows = diag(hamm) * buffered;

fft_res = fft(windows);
fftabs = abs(fft_res);
fftabs_reduced = fftabs(1:HOPSIZE+1,:) / WINDOWSIZE;
logmag = log(1+1000*fftabs_reduced);

num_fft = size(logmag, 1);
num_frames = size(logmag, 2);

%%% spectral flux
prev = zeros(num_fft,1);
flux = zeros(num_frames,1);
%%% flux
for i = 1:num_frames
	diff = logmag(:,i) - prev;
	diff(diff<0) = 0;
	diff(1) = 0; % to match marsyas
	flux(i) = sum(diff);
	prev = logmag(:,i);
end

if 0
	python_flux = load('flux.txt');
    python_flux = python_flux(:,2);
	hold on;
	plot(flux);
	plot(python_flux, 'g');
	plot(flux-python_flux, 'r');
	pause;
	exit(1);
end


%%% filter
%%% octave has a bug in the FIR filter design code, so for now
%%% I just manually specify the coefficients after calculating
%%% them in scipy
%b = fir1(LOWPASS_N-1, LOWPASS_HZ / (oss_sr/2));
b = [
    0.0096350145101721 
    0.0155332463596257 
    0.0320375094304201 
    0.0563964822180710 
    0.0839538807274150 
    0.1091494735026997 
    0.1267578095297140 
    0.1330731674437646 
    0.1267578095297140 
    0.1091494735026998 
    0.0839538807274150 
    0.0563964822180711 
    0.0320375094304201 
    0.0155332463596257 
    0.0096350145101721 
]';
filtered_flux = filter(b, 1.0, flux);


num_bh_frames = fix(length(filtered_flux) / 128);
oss = filtered_flux(1:num_bh_frames * 128);


