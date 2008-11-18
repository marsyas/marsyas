% clear
% filename = 'z:\MIDI\RWC_MIDI\JazzMusic\RWC-MDB-J-2001.SMF\RWC-MDB-J-2001\RM-J002.MID';%'z:\MIDI\syncTest.mid' 
% fs = 44100;
% startSeg = 0;
% endSeg = 10;
% sigNewTextWin = 1;

%% INITS
numChannels = 0;
audioLength = 0;

%% read MIDI file and pre-process MIDI info
nmat = readmidi(filename);

if endSeg ~= 0
	nmat = onsetwindow(nmat, startSeg, endSeg, 'sec');
end

if isempty(nmat)
    return;
end

%Removal of leading silence (trim)
nmat = trim(nmat);

%get playing MIDI channels
channels = mchannels(nmat);
numChannels = length(channels);

% limit number of MIDI channels
if numChannels > 5
    numChannels = 5;
end

%get individual MIDI tracks, and make them monophonic if necessary
MIDItracks = cell(numChannels, 1);
for i=1:numChannels
	MIDItracks{i} = getmidich(nmat,channels(i));
	%if ~ismonophonic(MIDItracks{i})
        MIDItracks{i} = extreme(MIDItracks{i}); %make it monophonic!
	%end
    
    %avoid intersections and reassign MIDI channels between 1:numChannels
    for n = 1:size(MIDItracks{i},1)-1
        nEnd = MIDItracks{i}(n,6) +  MIDItracks{i}(n,7);
        if nEnd >= MIDItracks{i}(n+1,6)
            MIDItracks{i}(n,7) = MIDItracks{i}(n,7)*0.9; %reduce in 10% the length of the note
        end 
        MIDItracks{i}(n,3) = i;
    end
    MIDItracks{i}(end,3) = i;
    
    %drop short notes (i.e. < 100ms)
    MIDItracks{i} = dropshortnotes(MIDItracks{i}, 'sec', 0.100);
    %pianoroll(MIDItracks{i});
    %pause;
end

nmat = vertcat(MIDItracks{:});
%pianoroll(nmat);

%% synthesize audio tracks for each MIDI channel
AUDIOtracks = cell(numChannels,1);
trackLengths = zeros(numChannels,1);
for i=1:numChannels
	AUDIOtracks{i} = nmat2snd(MIDItracks{i},'fm', fs); 
	%soundsc(AUDIOtracks{i}, fs)
	trackLengths(i) = length(AUDIOtracks{i});
end

% Pad shorter tracks with trailing zeros
audioLength = max(trackLengths);%in samples
for i=1:numChannels
    l = length(AUDIOtracks{i});
	AUDIOtracks{i} = [AUDIOtracks{i}, zeros(1,audioLength-l)];
end

%% convert audio into a matrix
audio=zeros(numChannels+1,audioLength);
for i=1:numChannels
	%All MIDI channels mixed
	audio(1,:) = audio(1,:)+(AUDIOtracks{i}(1,:));
	%separate audio for each MIDI channel
	audio(i+1,:) = AUDIOtracks{i}(1,:);
end
%audio(1,:) = audio(1,:)/numChannels;

%save some memory by getting rid of unnecessary audio data
clear AUDIOtracks;

%% componsate MIDI2audio delay (coming from MIDI Toolbox...)
MIDI2audioDelay = 0.017; %in secods - hardcoded delay that MIDI Toolbox puts when synthesizing the MIDI audio...
nmat(:,6) = nmat(:,6) - MIDI2audioDelay;

%% get list of onsets from all MIDI channels
if(sigNewTextWin > 0)
    allOnsets = nmat(:,6);
    allOnsets = sort(allOnsets);
    allOnsets = allOnsets(allOnsets > 0);
    %filter onsets that are too close...
    minLen = 0.050; %50ms is the minimum length for a texture window
    prevOnset = 0;
    onsets = [];
    for i=1:length(allOnsets)
        if allOnsets(i)-prevOnset > minLen
            onsets = [onsets, allOnsets(i)];
            prevOnset = allOnsets(i);
        end
    end
end

%% init some vars
numActiveNotes = 0;
onsetIndex = 1;

textWinStart = 1;
textWinEnd = 0;

refAudio = [];

refAudioTextWinds = cell(1,0);
resAudioTextWinds = cell(1,0);
segAudioTextWinds = cell(1,0);

SDRTextWinds = [];
numActiveNotesTextWinds = [];

activeChannels = [];
activeChannelsTextWinds = cell(1,0);

MIDInotes = cell(numChannels,1);





    









