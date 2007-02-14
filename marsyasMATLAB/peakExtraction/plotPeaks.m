function [] = plotPeaks(P)
P=P';
maxGp = max(P(7, :))+1;
nb_coul = 64 ;
if(P(1, 1) == -1)
     timeInterval = P(3, 1)/P(2, 1)*1000;
     P=P(:, 2:end);
     coul = floor(rand(1, maxGp)*nb_coul)+1;
 else
     timeInterval=1;
     coul = floor((1:maxGp)*nb_coul/maxGp);
    end
timeInterval=1;

maxA = max(P(2,:)) ;

cm = colormap ;

size_pt = .03 ;
inc_x = 1 ;
inc_y = 1 ;

% seuil = 30;
% P(7, :) = P(7,:)-min(P(7, :));

hold on
for i=1:length(P(1,:))
   if(P(7, i)>-1)
  amp_plot = 20*log10(P(2, i))+80;
  amp_plot = 20*log10(P(2, i)/maxA)+80;
  amp_plot = amp_plot-20*log10(maxA)+80;
% amp_plot=100;
%   value_color = floor((nb_coul)*(P(7, i))/maxGp)+1;
P(7, i)
value_color = coul(P(7, i)+1);

color = cm(value_color, :) ;
  plot (P(6, i)*inc_x*timeInterval, inc_y*P(1, i), 'go', 'LineWidth', 1, ...
        'MarkerEdgeColor', 'k', ...
        'MarkerFaceColor', color, ...
        'MarkerSize', amp_plot*size_pt) ;
  %	'MarkerSize', size_pt*amp_plot) ;
  %	      'MarkerSize', size_pt*amp(i)/amp_max) ;  
   end
end
hold off

ylabel('Frequency (Hz)');
xlabel('Time (ms)');