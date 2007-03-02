w = Omega*2*pi/length(Omega);

c = LPCC_out(1:end);

lpccEnv = zeros(size(w));%g_MARS_dB*ones(size(Omega));

for i=1:length(w)/2
    for j=1:length(c)
        lpccEnv(i) = lpccEnv(i)+ c(j)*cos(j*w(i));
    end;
    lpccEnv(i) = exp(lpccEnv(i));
end;

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% plots
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
hold on
subplot(212);
plot(Omega, 20*log10(lpccEnv)+10*log10(g_MARS),'k','Linewidth',1.5); %LPCC Spectrum envelope
hold off

figure(22);
subplot(3,1,1)
plot(LPCC_in); %LPC coefs
subplot(3,1,2)
plot(c, 'r'); %Marsyas LPCC
subplot(3,1,3)
plot(w,lpccEnv,'k','Linewidth',1.5); %LPCC Spectrum envelope






