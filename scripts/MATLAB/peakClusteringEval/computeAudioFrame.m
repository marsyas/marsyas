
%% Get audio frame (with hopSize samples) from MIDI mix to send to output
audioFrame = zeros(1,hopSize);
endPos = pos+hopSize-1;
endPos2 = endPos; 
if endPos > audioLength
    audioFrame(1:audioLength-pos+1) = audio(1,pos:audioLength);
    lastFrame = true;
    endPos2 = audioLength; %does not include zero padding
    %
    refAudio = [refAudio audio(:,pos:audioLength)];
else
    audioFrame = audio(1,pos:endPos);
    lastFrame = false;
    %
    refAudio = [refAudio audio(:,pos:endPos)];
end

%% determine corresponding analysis window (due to overlapping)
winStart = (endPos-winSize+1)/fs; %in seconds
winEnd = endPos/fs; %in seconds

%% compute number of active notes in current audio window
% for all the MIDI notes, count the ones 
% active in the current audio frame
count  = 0;
activeChWin = false(numChannels,1);
for n=1:size(nmat,1)
    noteStart = nmat(n,6); %in seconds
    noteEnd = noteStart + nmat(n,7); % in seconds
    noteChannel = nmat(n,3);
    noteMIDIpitch = nmat(n,4);
    
    if ((noteStart <= winStart) && (noteEnd >= winEnd)) || ...
            ((noteStart <= (winEnd-winStart)/2+winStart) && (noteEnd >= winEnd)) || ...
            ((noteStart <= winStart) && (noteEnd >= (winEnd-winStart)/2+winStart ))
        
        count = count+1;
        activeChWin(noteChannel) = true; %this channel is playing a note in this frame
        
        pitches = MIDInotes{noteChannel,1};
        if isempty(pitches)
            pitches = [pitches noteMIDIpitch];
        elseif pitches(end) ~= noteMIDIpitch
            pitches = [pitches noteMIDIpitch];
        end
        MIDInotes{noteChannel,1} = pitches; 
    end
end

activeChannels = logical([activeChannels activeChWin]);

%get number of active notes in current texture window so far
numActiveNotes = 0;
for c=1:numChannels
    numActiveNotes = numActiveNotes + length(MIDInotes{c,1});
end


%% check if this is the last frame of current texture window
newTextWin = 0;
if(sigNewTextWin > 0)
    if lastFrame
        newTextWin = 1;%i.e. set it to true
        textWinStart = textWinEnd+1; %in samples
        textWinEnd = endPos2; %in samples
    elseif onsetIndex <= length(onsets)
        %if next frame is after the next onset (i.e at least half of it is 
        %after the onset it means the current frame is the last one in the 
        %current texture window
        nextWinStart = winStart + hopSize/fs;
        if nextWinStart + (winSize/2)/fs > onsets(onsetIndex)
            onsetIndex = onsetIndex+1;
            newTextWin = 1; %i.e. set it to true
            textWinStart = textWinEnd+1; %in samples
            textWinEnd = endPos2; %in samples
        end
    end
end
