function [] = plotCluster()

peaks = get(gcbo, 'UserData');


  subplot(2, 1, 2);
  cla
plotGpPeaks(peaks, peaks(1, 1)-1);
