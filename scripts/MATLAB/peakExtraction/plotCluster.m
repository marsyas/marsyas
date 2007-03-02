function [] = plotCluster()

peaks = get(gcbo, 'UserData');


subplot(2, 1, 2);
cla
plotGpPeaks(peaks(2:end,:), peaks(1, 2));
