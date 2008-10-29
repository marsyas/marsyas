
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%get the resynthesized and segregated audio texture window
resAudio = PlotSink_send2MATLAB_indata;
    
% some inits
frameSize = hopSize;
numFrames = size(activeChannels, 2); % = size(resAudio,2)/hopSize = size(refAudio,2)/hopSize
segAudio = zeros(size(audio, 1), size(resAudio,2));

%check for any onsets in texture window
%(this is used for fixed size texture windows)
margin = 0.000; %margin in seconds
tws = textWinStart + margin*fs; %in samples
twe = textWinEnd  - margin*fs; %in samples
onsetsSamples = onsets*fs; % convert to samples
os = onsetsSamples > tws & onsetsSamples < twe;
onsetsSamples = onsetsSamples(os); %in samples

%convert onsets to frame boundaries
onsetsFrames = ceil(onsetsSamples/frameSize);%in frames
onsetsFrames = onsetsFrames - ceil(tws/frameSize)+1;%in frames, and relative to current text wind

onsetsFrames = [1 onsetsFrames numFrames+1]; 

numRegions = length(onsetsFrames)-1;
SDR = zeros(1,numRegions);
                      
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% compute SDR for inter-onset region in the current texture window
for r=1:numRegions
    sf = onsetsFrames(r);
    ef = onsetsFrames(r+1)-1;
    s = (sf-1)*frameSize+1;
    e = ef*frameSize;
    
    %Channel Selection for this region
    channelSel = logical(sum(activeChannels(:,sf:ef),2));
    channelSel = [false; channelSel]; %also ignore mix channel
    channelSelIdx = [1:1:size(channelSel,1)]; 
    
    refAudioFrame = refAudio(channelSel, s:e); 
    resAudioFrame = resAudio(1:numActiveNotes,s:e);

    %consolidate sample lengths (if necessary...)
    %minLength = min(size(refAudioFrame, 2), size(resAudioFrame, 2));
    %refAudioFrame = refAudioFrame(:,1:minLength);
    %resAudioFrame = resAudioFrame(:,1:minLength);

    %compute SDR for current frame
    %correspondence = [];
    [SDRresults, correspondence] = computeSDR(refAudioFrame, resAudioFrame);
    SDR(r) = SDRresults;

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
    






