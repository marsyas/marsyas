if exist('SDR.mat', 'file')
    load SDR.mat SDR
    SDR = [SDR mean(SDRTextWinds)]; 
    save('SDR.mat', 'SDR', '-append') 
else
    SDR = mean(SDRTextWinds);
    save('SDR.mat', 'SDR');
end