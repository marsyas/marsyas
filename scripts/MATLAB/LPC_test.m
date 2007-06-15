%%function LPC_test%(LPC_order, LPC_gamma, LPC_in, LPC_out)

N = size(LPC_in, 2);
Nfft = N;

MAT_in = LPC_in;

% %pre-emphasis -> MAT_in
% pre = [1 LPC_preemphasis];
% [MAT_in, Zi] = filter(pre, 1, LPC_in, Zi);

% %windowing -> MAT_in
% win = hamming(N)';
% MAT_in = MAT_in.*win;

% LPC from Marsyas
if(featureMode)
    a_MARS = [1, -1*LPC_out(1:LPC_order)'];
    pitch_MARS = LPC_out(LPC_order+1);
    g_MARS = LPC_out(LPC_order+2);
    g_MARS_dB = 20*log10(g_MARS);
else
    g_MARS_dB = 20*log10(g_MARS);
end
%LPC from MATLAB

%MATLAB LPC
%[a_MAT,g_MAT] = lpc(MAT_in, LPC_order);

%%%%%%%%%%%%%%%%%%%%%
%Dan Ellis LPC
%%%%%%%%%%%%%%%%%%%%%
%rxx = xcorr(MAT_in');
%rxx = rxx(length(MAT_in)+[0:LPC_order]);
%R = toeplitz(rxx(1:LPC_order));
%an = inv(R)*rxx(2:(LPC_order+1));
%a_MAT = [1 -an'];

%%%%%%%%%%%%%%%%%%%%%%%
%MATLAB lpc
%%%%%%%%%%%%%%%%%%%%%%%
[a_MAT, LPCgain] = lpc(MAT_in, LPC_order)

rs = filter(a_MAT, 1, MAT_in);
g_MAT = sqrt(mean(rs.^2))
g_MAT_dB = 20*log10(g_MAT);

LPCgain = sqrt(LPCgain);

% Z-Plane pole shifting...
% using MATLAB LPC values
i = [1:LPC_order];
gamma = LPC_gamma;
gamma = gamma.^i;
gamma = [1 gamma];
a_MAT_gamma = a_MAT.*gamma;


%dB scaling for Marsyas LPC
A_MARS=20*log10(abs(fft(a_MARS,Nfft)));
H_MARS=-A_MARS;

%dB scaling for MATLAB LPC
A_MAT=20*log10(abs(fft(a_MAT,Nfft)));
H_MAT=-A_MAT;% + g_MAT_dB;

%dB scaling for shifted LPC coefs
A_MAT_gamma=20*log10(abs(fft(a_MAT_gamma,Nfft)));
H_MAT_gamma=-A_MAT_gamma;% + g_MAT_dB;

%original audio frame spectrum (no pre-emphasis)
spect=20*log10(abs(fft(LPC_in,Nfft)));

%audio spectrum (with possible pre-emphasis)
%%spect_preemp=20*log10(abs(fft(LPC_in_Preemphasis,Nfft)));

n=0:N-1;
figure(1)
clf

%subplot(511)
subplot(211)
hold on
%%plot(n, LPC_in_Preemphasis, 'm');
plot(n,LPC_in, 'b')
hold off
title('original time signal: input audio frame with and w/o pre-emphasis')
xlabel('n \rightarrow')
axis([0 N-1 -1 1])

%subplot(512)
subplot(212)
Omega=(0:Nfft-1);
hold on
%%plot(Omega,spect_preemp,'m');
plot(Omega,spect, 'b')
plot(Omega,H_MAT_gamma,'y','Linewidth',1.5)             %[!] gain scaling
plot(Omega,H_MAT,'g','Linewidth',1.5)                   %[!] gain scaling
plot(Omega,H_MARS+10*log10(g_MARS),'r','Linewidth',1.5) %[!] gain scaling
hold off
title('magnitude spectra |X(f)| and |H(f)| dB')
xlabel('bins \rightarrow')
axis([0 N/2-1 -inf inf])

% subplot(513)
% hold on
% plot (a_MAT - a_MARS, 'r');
% axis([1 length(a_MAT) -0.5 0.5])
% legend('alpha diffs');
% title('LPC coef difference Marsyas/MATLAB');
% hold off
% 
% subplot(514)
% hold on
% bar (1, (g_MAT - g_MARS)*10, 'b');
% bar (2, abs(g_MAT / g_MARS) - 1, 'y');
% bar (3, g_MAT*10, 'g');
% bar (4, g_MARS*10, 'r');
% axis([1 6 -0.5 0.5])
% legend('Gain dif','Gain ratio', 'MAT gain', 'MARS gain');
% title('LPC Gain difference Marsyas/MATLAB');
% hold off
% 
% subplot(515)
% plot(LPCwarped_pitch);
% title('LPC pitch');


