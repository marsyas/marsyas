
%% Get audio frame (with hopSize samples) from MIDI mix to send to output
audioFrame = zeros(1,hopSize);
endPos = pos+hopSize-1;
endPos2 = endPos;
if endPos > audioLength
    audioFrame(1:audioLength-pos+1) = audio(1,pos:audioLength);
    lastFrame = true;
    endPos2 = audioLength;
else
    audioFrame = audio(1,pos:endPos);
    lastFrame = false;
end

%% determine corresponding analysis window (due to overlapping)
winStart = (endPos-winSize+1)/fs; %in secods
winEnd = endPos/fs; %in seconds

%% compute number of active notes in current analysis window
% for all the MIDI notes, count the ones 
% active in the current analysis window
count  = 0;
for n=1:size(nmat,1)
    noteStart = nmat(n,6); %in seconds
    noteEnd = noteStart + nmat(n,7); % in seconds
    if (noteStart <= winStart) && (noteEnd >= winEnd)
        count = count+1;
        %we could also store their f0s...
    end
end
%get maximum number of active notes in current texture window so far
numActiveNotes = max(numActiveNotes, count);

%% check if this is the last analysis window of current texture window
newTextWin = 0;
if(sigNewTextWin > 0)
    if lastFrame
        newTextWin = 1;%i.e. set it to true
        textWinStart = textWinEnd+1; %in samples
        textWinEnd = endPos2; %in samples
    elseif onsetIndex < length(onsets)
        if (winEnd-winStart)/2 > onsets(onsetIndex)
            onsetIndex = onsetIndex+1;
            newTextWin = 1; %i.e. set it to true
            textWinStart = textWinEnd+1; %in samples
            textWinEnd = endPos2; %in samples
        end
    end
end
