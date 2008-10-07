
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
    % (optimized version below)
    %channelSel = sum(audio(2:end,textWinStart:textWinEnd).^2, 2) > eps*1e15; 
    channelSel = activeChannels;
    channelSel = [false; channelSel]; %also ignore mix channel
    channelSelIdx = [1:1:size(channelSel,1)];
    refAudio = audio(channelSel,textWinStart:textWinEnd); 
  
    %make sure both audio vectors have the same length (check for PHASE PROBLEMS [!])
    audioLen = min(size(refAudio,2), size(resynthAudio,2));
    refAudio = refAudio(:,1:audioLen);
    resynthAudio = resynthAudio(1:numActiveNotes,1:audioLen);
          
    %compute SDR for current texture window
    correspondence = [];
    [SDRresults, correspondence] = computeSDR(refAudio, resynthAudio);
    SDRTextWinds = [SDRTextWinds, SDRresults];
    
    %place clustered audio into the corresponding audio channel
    temp = channelSelIdx(channelSel);
    temp = temp(correspondence);                                        
    segregatedAudio = zeros(size(audio, 1), size(resynthAudio,2));
    segregatedAudio(temp, :) = resynthAudio;
    
    %store results for entire file
    numActiveNotesTextWinds = [numActiveNotesTextWinds, numActiveNotes];
    %
    refAudioTextWinds = [refAudioTextWinds, audio(:,textWinStart:textWinStart+audioLen-1)]; %, refAudio];
    %
    segregatedAudioTextWinds = [segregatedAudioTextWinds, segregatedAudio];
        
    resynthAudioTextWinds = [resynthAudioTextWinds, resynthAudio];
    
    %plotClusteredAudio
 
    % init vars for next texture window
    numActiveNotes = 0;
    refFramesCount = 0;
    resynthFramesCount = 0;
    resynthAudio = [];
    activeChannels = false(numChannels,1);
else 
    %keep accumulating resynthesized audio frame from PlotSink MarSystem.
    %each row has the audio from each cluster 
    %(i.e. only the first numActiveNotes rows contain audio... the others
    %are silence).
    resynthAudio = [resynthAudio, PlotSink_send2MATLAB_indata];
end





