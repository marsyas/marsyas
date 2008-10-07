%% PLAY AND PLOT ENTIRE FILE
ref = horzcat(refAudioTextWinds{:});
seg = horzcat(segregatedAudioTextWinds{:});

sound(ref(1,:), fs);
for i=2:size(ref,1)
    subplot(2,1,1)
    plot(ref(i,:), 'g');
    subplot(2,1,2)
    plot(seg(i,:), 'r');
    pause     
    sound(ref(i,:), fs);  
    sound(seg(i,:), fs);
    %sound(vertcat(ref(i,:),seg(i,:))', fs); % to listen as stereo
    pause
end

%% PLAY AND PLOT TEXTURE WINDOWS
% for i=1:length(refAudioTextWinds)
%     display('>>>>>>>>>>>>>>>>>>>>>>>>>');
%     display(['Texture window ' num2str(i)]);
%     display(['Num. active notes = ' num2str(numActiveNotesTextWinds(i))]);
%     display(['SDR = ' num2str(SDRTextWinds(i))]);
%     for c=2:size(refAudioTextWinds{i},1)%ignore MIX channel (i.e. c = 1)
%         display(['  Channel ' num2str(c-1)]);
%         subplot(2,1,1)
%         plot(refAudioTextWinds{i}(c,:), 'g')
%         subplot(2,1,2)
%         plot(segregatedAudioTextWinds{i}(c,:), 'r')
%         sound(refAudioTextWinds{i}(c,:), fs);
%         sound(segregatedAudioTextWinds{i}(c,:), fs);
%         pause
%     end
% end

%% PLOT ALL SEGREGATED CHANNELS
% ref = horzcat(refAudioTextWinds{:});
% seg = horzcat(segregatedAudioTextWinds{:});
% for c=1:numChannels
%     subplot(numChannels, 1, c);
%     plot(seg(c+1,:));
% end

%% Plot clustered audio tracks for each texture window
% for i=1:length(resynthAudioTextWinds)
%     nch = size(resynthAudioTextWinds{i},1);
%     display('>>>>>>>>>>>>>>>>>>>>>>>>>');
%     display(['Texture window ' num2str(i)]);
%     display(['Num. active notes = ' num2str(numActiveNotesTextWinds(i))]);
%     for c=1:nch
%         subplot(nch,1,c)
%         plot(resynthAudioTextWinds{i}(c,:))
%     end
%     pause
% end
