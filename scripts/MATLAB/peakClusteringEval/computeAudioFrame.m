
%% Get audio frame (with hopSize samples) from MIDI mix to send to output
audioFrame = zeros(1,hopSize);
endPos = pos+hopSize-1;
endPos2 = endPos; %does not include zero padding (see below)
if endPos > audioLength
    audioFrame(1:audioLength-pos+1) = audio(1,pos:audioLength);
    lastFrame = true;
    endPos2 = audioLength; %does not include zero padding
else
    audioFrame = audio(1,pos:endPos);
    lastFrame = false;
end

refFramesCount = refFramesCount+1;

%% determine corresponding analysis window (due to overlapping)
frameStart = (endPos-winSize+1)/fs; %in secods
frameEnd = endPos/fs; %in seconds

%% compute number of active notes in current audio frame
% for all the MIDI notes, count the ones 
% active in the current audio frame
count  = 0;
for n=1:size(nmat,1)
    noteStart = nmat(n,6); %in seconds
    noteEnd = noteStart + nmat(n,7); % in seconds
    if ((noteStart < frameStart) && (noteEnd > frameEnd))% || ((noteStart >= frameStart) && (noteEnd <= frameEnd))
        count = count+1;
        activeChannels(nmat(n,3)) = true; %this channel is playing a note in this frame (and texturew window)!
        %we could also store their f0s...
    end
end
%get maximum number of active notes in current texture window so far
numActiveNotes = max(numActiveNotes, count);

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
        nextFrameStart = frameStart + hopSize/fs;
        if nextFrameStart + (winSize/2)/fs > onsets(onsetIndex)
            onsetIndex = onsetIndex+1;
            newTextWin = 1; %i.e. set it to true
            textWinStart = textWinEnd+1; %in samples
            textWinEnd = endPos2; %in samples
        end
    end
end
