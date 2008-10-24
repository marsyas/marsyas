
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%get the resynthesized and segregated audio texture window
resAudio = PlotSink_send2MATLAB_indata;
    
% some inits
numFrames = size(activeChannels, 2); % = size(resAudio,2)/hopSize = size(refAudio,2)/hopSize
SDR = zeros(1,numFrames);
segAudio = zeros(size(audio, 1), size(resAudio,2));
                      
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% compute SDR in a frame-by-frame basis
for f=1:numFrames        
    channelSel = activeChannels(:,f);
    channelSel = [false; channelSel]; %also ignore mix channel
    channelSelIdx = [1:1:size(channelSel,1)]; 

    %get a frame of audio
    fsize = hopSize;    
    s = (f-1)*fsize+1;
    e = s + fsize-1;
    %if e > size(resAudio,2)
    %    e = size(resAudio,2);
    %end
    refAudioFrame = refAudio(channelSel, s:e); 
    resAudioFrame = resAudio(1:numActiveNotes,s:e);

    %consolidate sample lengths (if necessary...)
    %minLength = min(size(refAudioFrame, 2), size(resAudioFrame, 2));
    %refAudioFrame = refAudioFrame(:,1:minLength);
    %resAudioFrame = resAudioFrame(:,1:minLength);

    %compute SDR for current frame
    %correspondence = [];
    [SDRresults, correspondence] = computeSDR(refAudioFrame, resAudioFrame);
    SDR(f) = SDRresults;

    %% CORRESPONDENCE !!!!
    %place clustered audio into the corresponding audio channel
    temp = channelSelIdx(channelSel);
    temp = temp(correspondence);  
    if isempty(temp)
        segAudio(1:size(resAudioFrame,1), s:e) = resAudioFrame; % [!]
    else
        segAudio(temp, s:e) = resAudioFrame; 
    end
end
        
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% store results for entire file
numActiveNotesTextWinds = [numActiveNotesTextWinds, numActiveNotes];
refAudioTextWinds = [refAudioTextWinds, refAudio];
resAudioTextWinds = [resAudioTextWinds, resAudio];
segAudioTextWinds = [segAudioTextWinds, segAudio];
SDRTextWinds = [SDRTextWinds, mean(SDR(:))];

%plotClusteredAudio

% init vars for next texture window
numActiveNotes = 0;
activeChannels = [];
refAudio = [];
    






