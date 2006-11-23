function [res] = getSnr (f1, f2)
  
debug=1;

 if(debug)
        f1
  f2
 end

  [x1, fs, nb] = wavread (f1);
  x2 = wavread (f2);

  if (length(x1)>length(x2))
x2 = [x2; zeros(length(x1)-length(x2), 1)];
else
x1 = [x1; zeros(length(x2)-length(x1), 1)];
  end
  
  res = 20*log10(norm(x1)/norm(x1-x2));
%   -computeLevel(x1)+computeLevel(x1-x2);
  
%   name = [f1 '_res.wav'];
%    wavwrite (x1-x2, fs, nb, 'res');
%   
  if(debug)


   clf
   subplot(2, 1, 1)
specgram(x1,2048);
%   hold on
   subplot(2, 1, 2)
 specgram(x2, 2048);
res 
%pause
  end

  sound(x2, 44100);
  
%        whos x_ori x_synth
% % length(x_ori)-length(x_synth)
%    figure;
  
%     t=(1:length (x_ori));
%   t1=(1:length (x_synth));
%     subplot (2,1,1);
%   plot (t, x_ori, t1, x_synth) ;

%   subplot (2,1,2);
%     plot (20*log10(minus.*minus));
%   zoom on;
