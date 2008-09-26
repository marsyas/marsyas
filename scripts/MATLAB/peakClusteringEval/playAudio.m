for i=1:length(refAudioTextWinds)
    plot(sum(refAudioTextWinds{i}(2,:),1))
    soundsc(sum(refAudioTextWinds{i}(2,:),1), fs);
    pause
    
    %plot(sum(resynthAudioTextWinds{7}(1,:),1))
    %soundsc(sum(resynthAudioTextWinds{i}(1,:),1), fs);
    %pause
end
