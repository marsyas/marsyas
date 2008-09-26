for i=1:length(refAudioTextWinds)
    plot(sum(refAudioTextWinds{i}(:,:),1))
    soundsc(sum(refAudioTextWinds{i}(:,:),1), fs);
    pause

    plot(sum(resynthAudioTextWinds{i}(:,:),1))
    soundsc(sum(resynthAudioTextWinds{i}(:,:),1), fs);
    pause
end
