% for i=1:length(refAudioTextWinds)
%     plot(sum(refAudioTextWinds{i}(:,:),1))
%     sound(sum(refAudioTextWinds{i}(:,:),1), fs);
%     pause
% 
%     plot(sum(resynthAudioTextWinds{i}(:,:),1))
%     sound(sum(resynthAudioTextWinds{i}(:,:),1), fs);
%     pause
% end

% for i = 1:length(refAudioTextWinds)
%     i
%     numActiveNotesTextWinds(i)
%     computeSDR(refAudioTextWinds{i},resynthAudioTextWinds{i}) 
% end


%% Detect "active" channels in each texture window
numTextWinds = length(numActiveNotesTextWinds);
for t=1:numTextWinds
    %t
    numActiveNotesTextWinds(t)
    count = 0;
    for k=1:numChannels
        %k
        %sound(refAudioTextWinds{t}(k,:), fs)
        if sum((refAudioTextWinds{t}(k,:)).^2) > eps*1e12
            count = count +1;
        end
        %pause
    end
    count
    pause
end