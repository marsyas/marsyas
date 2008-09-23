%clear

%filename = 'z:\MIDI\test.mid' %'RM-C002.mid';
%fs = 44100;
%frameSize = 2048; % size in samples
% 
%startSeg = 0;
%endSeg = 10;


%% read MIDI file and pre-process MIDI info
nmat = readmidi(filename);

if endSeg ~= 0
	nmat = onsetwindow(nmat, startSeg, endSeg, 'sec');
end

%Removal of leading silence (trim)
nmat = trim(nmat);

%get playing MIDI channels
channels = mchannels(nmat);
numChannels = length(channels);

%get individual MIDI tracks, and make them monophonic if necessary
MIDItracks = cell(numChannels, 1);
for i=1:numChannels
	MIDItracks{i} = getmidich(nmat,channels(i));
	if ~ismonophonic(MIDItracks{i})
		MIDItracks{i} = extreme(MIDItracks{i}); %make it monophonic!
	end
end

%% synthesize MIDI audio tracks
%synthesize audio of each MIDI track
AUDIOtracks = cell(numChannels,1);
trackLengths = zeros(numChannels,1);
for i=1:numChannels
	AUDIOtracks{i} = nmat2snd(MIDItracks{i},'fm', fs); 
	%soundsc(AUDIOtracks{i}, fs)
	trackLengths(i) = length(AUDIOtracks{i});
end

%get shorted track length, which will be the overall audio length (in samples)
audioLength = min(trackLengths);%in samples

%trim MIDI info to shortest track duration
nmat = onsetwindow(nmat, 0, audioLength/fs, 'sec');

%convert audio into a matrix
audio=zeros(numChannels+1,audioLength);
for i=1:numChannels
	%All MIDI channels mixed
	audio(1,:) = audio(1,:)+(AUDIOtracks{i}(1,1:audioLength));%/numChannels);
	%separate audio for each MIDI channel
	audio(i+1,:) = AUDIOtracks{i}(1,1:audioLength);
end

%save some memory by getting rid of unnecessary audio data
clear AUDIOtracks;


%% get list of onsets from all MIDI channels
if(sigNewTextWin > 0)
    onsets = nmat(:,6); %in seconds
    onsets = onsets*fs; %in samples
    
    %filter onsets that are too close...
    % XXX
end

%% init some vars
numActiveNotes = 0;
onsetIndex = 1;
textWinStart = 1;
textWinEnd = 0;
    









