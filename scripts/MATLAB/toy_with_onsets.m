

% figure(1)
% plot(srcAudio(1,:));
% hold on
% plot(1:hopSize:hopSize*length(FluxTS),FluxTS/max(FluxTS), 'r');
% %hold on
% stem(1:hopSize:hopSize*length(onsetTS2),onsetTS2, 'k');
% plot(onsetAudio, 'g');
% hold off

figure(1)
onsetTS2 = [onsetTS(lookAheadSamples+1:end), zeros(1,lookAheadSamples)];
plot(segmentData);
hold on
plot(1:hopSize:hopSize*length(FluxTS),FluxTS/max(FluxTS), 'r');
%plot(1:hopSize:hopSize*length(FluxTSfilt),FluxTSfilt*1000, 'g');
%hold on
stem(1:hopSize:hopSize*length(onsetTS2),onsetTS2, 'k');
plot(onsetAudio, 'g');
hold off

figure(2)
l = length(Accum_out);
e = length(segmentData);
b = e-l+1;
plot(b:1:e, segmentData(b:1:e));
hold on
ll = l/hopSize+1;
ee = (length(FluxTS)-lookAheadSamples);
bb = ee-ll+1;
plot((bb-1)*hopSize:hopSize:(ee-1)*hopSize,FluxTS(bb:ee)/max(FluxTS(bb:ee)), 'r');
%plot((bb-1)*hopSize:hopSize:(ee-1)*hopSize,FluxTSfilt(bb:ee)*1000, 'g');
%hold on
stem((bb-1)*hopSize:hopSize:(ee-1)*hopSize,onsetTS2(bb:ee), 'k');
hold off

wavplay(segmentData(b:1:e), 44100);

