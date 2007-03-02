function LSP = LSP_test(LSP_order, LSP_in, LSP_P, LSP_Q, LSP_Proots, LSP_Qroots, LSP_out1, LSP_out2)

%LPC coefs --> A(z)
Az = [-1*LSP_in(1:LSP_order)]';

%Z-Plane contraction of H(z) poles (= A(z) zeros)
gamma = 0.9;
i=[1:LSP_order];
AzGamma = Az.*(gamma.^i); 
%build complete A(z) polinomial
Az = [1 Az];
AzGamma = [1 AzGamma];
%calculate A(z) roots
Azroots = roots(Az);
AzGammaroots = roots(AzGamma);

%determine P and Q polinomials
Pgamma = [AzGamma 0] + [0 AzGamma(length(AzGamma):-1:1)];
Qgamma = [AzGamma 0] - [0 AzGamma(length(AzGamma):-1:1)];
%determine roots
Pgammaroots = roots(Pgamma);
Qgammaroots = roots(Qgamma);


%data from Marsyas LSP...
Proots = roots(LSP_P);
Qroots = roots(LSP_Q);
%calculate LSPs using marsyas data...
PQroots = [Proots(2:length(Proots)); Qroots(1:length(Qroots)-1)];
LSP = angle(PQroots);
LSP = sort(LSP);
LSP = LSP(length(LSP)/2+1 : length(LSP)); %just the positive half of the Z-plane


figure(2);

polar(angle(AzGammaroots), abs(AzGammaroots), 'xr'); % H(z)pole shifting

hold on;

polar(angle(Azroots), abs(Azroots), 'xk'); %A(z) roots

polar(angle(Pgammaroots), ones(length(Pgammaroots),1)*1.05, 'oy');% P(z) contracted freqs 
polar(angle(Proots), ones(length(Proots),1), 'ob'); % P(z) freqs 

polar(angle(Qgammaroots), ones(length(Qgammaroots),1)*1.05, 'sy'); % Q(z) contracted freqs
polar(angle(Qroots), ones(length(Qroots),1), 'sb'); % Q(z) freqs

polar(LSP, ones(length(LSP),1)*0.95,'og'); %MATLAB calculated LSPs
polar(LSP_out2,ones(length(LSP_out2),1)*0.95, 'dr');%Marsyas calculated LSPs

hold off;
