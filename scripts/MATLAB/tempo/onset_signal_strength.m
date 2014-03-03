
function [oss, oss_sr] = onset_signal_strength(wav_data, wav_sr)


WINDOWSIZE = 1024;
HOPSIZE = 128;
% due to a bug in GNU/octave, I specify the filter coefficients manually
%LOWPASS_N = 15;
%LOWPASS_HZ = 7;

num_ticks = int32(length(wav_data) / (HOPSIZE*128)) - 1;

extra_zeros = zeros(1, WINDOWSIZE - HOPSIZE);
padded = [extra_zeros wav_data'];


%%% 1) Overlap: create overlapped signal
buffered = buffer(padded, WINDOWSIZE, WINDOWSIZE-HOPSIZE, 'nodelay');
oss_sr = wav_sr / HOPSIZE;

% only include complete buffers
buffered = buffered(:,1:num_ticks*HOPSIZE);

%%% 2) Log Power Spectrum: windowing and log-magnitude spectrum
% match Marsyas hamming window
ns = 0:WINDOWSIZE-1;
hamm = 0.54 - 0.46 * cos( 2*pi*ns / (WINDOWSIZE-1));

windows = diag(hamm) * buffered;

fft_res = fft(windows);
fftabs = abs(fft_res);
fftabs_reduced = fftabs(1:WINDOWSIZE/2+1,:) / WINDOWSIZE;
logmag = log(1+1000*fftabs_reduced);

num_fft = size(logmag, 1);
num_frames = size(logmag, 2);

%%% 3) Flux: spectral flux
prev = zeros(num_fft,1);
flux = zeros(num_frames,1);
% flux
for i = 1:num_frames
	diff = logmag(:,i) - prev;
	diff(diff<0) = 0;
	diff(1) = 0; % to match marsyas
	flux(i) = sum(diff);
	prev = logmag(:,i);
end

%%% 4) Low-pass filter
%%% octave has a bug in the FIR filter design code, so for now
%%% I just manually specify the coefficients after calculating
%%% them in scipy
%b = fir1(LOWPASS_N-1, LOWPASS_HZ / (oss_sr/2));
b = [
    0.0093397750944987 
    0.0152114803785360 
    0.0316389134101753 
    0.0560718686860614 
    0.0839029924263034 
    0.1094819495227379 
    0.1274203841652824 
    0.1338652726328101 
    0.1274203841652824 
    0.1094819495227379 
    0.0839029924263034 
    0.0560718686860614 
    0.0316389134101753 
    0.0152114803785360 
    0.0093397750944987
]';
filtered_flux = filter(b, 1.0, flux);

% output values
complete_beat_frames = fix(length(filtered_flux) / 128);
oss = filtered_flux(1:complete_beat_frames * 128);


