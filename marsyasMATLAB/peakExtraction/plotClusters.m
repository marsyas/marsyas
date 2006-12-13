% function [] = plotClusters(clusters)
clf

name = [path '/' fileName(1:end-4) 'Grd_'];

[nbClusters, nbAttributes] = size(clusters);

nb_coul = 64 ;
coul = floor(rand(1, nbClusters)*nb_coul)+1;
cm = colormap;

maxA = max(clusters(:, 8));
maxAstd = max(clusters(:, 9));
 
[nbPeaks, nbP] = size(peaks);
    
for i=1:nbClusters
    oriLabel = clusters(i, 1)
    groundLabel = clusters(i, 2);
    label = clusters(i, 3);
    start = clusters(i, 4);
    length = clusters(i, 5);
    freqMean = clusters(i, 6);
    freqStd = clusters(i, 7);
    ampMean = clusters(i, 8)/maxA;
    ampStd = clusters(i, 9)/maxAstd;
    voicing = clusters(i, 10)

    frequencyEvolution = clusters(i, 11:11+length);
    amplitudeEvolution = clusters(i, 11+length+1:11+length*2+1);

    value_color = coul(i);
    color = cm(value_color, :) ;
   
    firstC = zeros(1, nbP)+-1;
    firstC(2) = oriLabel;
    iPeaks = [firstC; peaks];
    
    subplot(2, 1, 1);
    line([start start+length], [ freqMean freqMean], ...
        'Color', color, 'LineWidth', ampMean*3, ...
    'ButtonDownFcn', 'plotCluster', 'UserData', iPeaks);
    line([start+length/2 start+length/2], [freqMean-freqStd/2 freqMean+freqStd/2], ...
        'Color', color, 'LineWidth', 1+voicing*10, ...
        'ButtonDownFcn', 'plotCluster', 'UserData', iPeaks); % , 'playCluster', 'UserData', [name num2str(oriLabel) '.wav']);

ylabel('Frequency (Hz)');
xlabel('Time (ms)');
    
    % line([start start], [freqMean-freqStd/2 freqMean+freqStd/2], 'Color', color, 'Marker', '+');
% line([start], [freqMean], 'Color', 'k', 'Marker', 'o', 'MarkerSize', 2);

%      subplot(4, 1, 3);
%      line((start:start+length), frequencyEvolution, 'Color', color, 'LineWidth', ampMean*3);
%      subplot(4, 1, 4);
%      line((start:start+length), amplitudeEvolution, 'Color', color, 'LineWidth', ampMean*3);

end
