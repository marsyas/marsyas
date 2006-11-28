% function [] = plotClusters(clusters)
clf

fileName = 'toto';
[nbClusters, nbAttributes] = size(clusters);

nb_coul = 64 ;
coul = floor(rand(1, nbClusters)*nb_coul)+1;
cm = colormap;

maxA = max(clusters(:, 5));
maxAstd = max(clusters(:, 6));
 
[nbPeaks, nbP] = size(peaks);
    
for i=1:nbClusters
    start = clusters(i, 1);
    length = clusters(i, 2);
    freqMean = clusters(i, 3);
    freqStd = clusters(i, 4);
    ampMean = clusters(i, 5)/maxA;
    ampStd = clusters(i, 6)/maxAstd;

    frequencyEvolution = clusters(i, 7:7+length);
    amplitudeEvolution = clusters(i, 7+length+1:7+length*2+1);

    value_color = coul(i);
    color = cm(value_color, :) ;
   
    firstC = zeros(1, nbP)+-2;
    firstC(1) = i;
    iPeaks = [firstC; peaks];
    
    subplot(2, 1, 1);
    line([start start+length], [ freqMean freqMean], ...
        'Color', color, 'LineWidth', ampMean*3, ...
    'ButtonDownFcn', 'plotCluster', 'UserData', iPeaks);
    line([start+length/2 start+length/2], [freqMean-freqStd/2 freqMean+freqStd/2], ...
        'Color', color, 'LineWidth', ampStd*10, ...
        'ButtonDownFcn', 'playCluster', 'UserData', [fileName num2str(i)]);

ylabel('Frequency (Hz)');
xlabel('Time (ms)');
    
    % line([start start], [freqMean-freqStd/2 freqMean+freqStd/2], 'Color', color, 'Marker', '+');
% line([start], [freqMean], 'Color', 'k', 'Marker', 'o', 'MarkerSize', 2);

%     subplot(4, 1, 2);
%     line((start:start+length), frequencyEvolution, 'Color', color, 'LineWidth', ampMean*3);
%     subplot(4, 1, 3);
%     line((start:start+length), amplitudeEvolution, 'Color', color, 'LineWidth', ampMean*3);

end
