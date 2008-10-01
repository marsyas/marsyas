
resynthFramesCount = resynthFramesCount + 1;

%check if we already resynthesized a complete texture window
if resynthFramesCount == refFramesCount
    %get the last audio frame
    resynthAudio = [resynthAudio, PlotSink_send2MATLAB_indata];
    
    %get reference audio for current texture window
    %(ignore mix track, i.e. 1st row, and "silent" 
    %audio channels in this texture window)
%     refAudio = [];
%     for k=1:numChannels
%         if sum(audio(k+1,textWinStart:textWinEnd).^2) > eps*1e12
%             refAudio = [refAudio; audio(k+1,textWinStart:textWinEnd)];
%         end
%     end
    %optimized version
    channelSel = sum(audio(2:end,textWinStart:textWinEnd).^2, 2) > eps*1e12;
    channelSel = [false; channelSel]; %also ignore mix channel
    refAudio = audio(channelSel,textWinStart:textWinEnd); 
        
    %make sure both audio vectors have the same length (PHASE PROBLEMS?!?!)
    audioLen = min(size(refAudio,2), size(resynthAudio,2));
    refAudio = refAudio(:,1:audioLen);
    resynthAudio = resynthAudio(1:numActiveNotes,1:audioLen);
        
    %compute SDR for current texture window
    %SDRresults = computeSDR(refAudio, resynthAudio); %%%%%%%%%%%%%%%%%%%%%%
    %SDRTextWinds = [SDRTextWinds, SDRresults];
    
    %store results for entire file
    refAudioTextWinds = [refAudioTextWinds, refAudio];
    resynthAudioTextWinds = [resynthAudioTextWinds, resynthAudio];
    numActiveNotesTextWinds = [numActiveNotesTextWinds, numActiveNotes];
 
    % init vars for next texture window
    numActiveNotes = 0;
    refFramesCount = 0;
    resynthFramesCount = 0;
    resynthAudio = [];   
else 
    %keep accumulating resynthesized audio frame from PlotSink MarSystem.
    %each row has the audio from each cluster 
    %(i.e. only the first numActiveNotes rows contain audio... the others
    %should be silence).
    resynthAudio = [resynthAudio, PlotSink_send2MATLAB_indata];
end





