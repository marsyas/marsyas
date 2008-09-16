%clear

% filename = 'RM-C002.mid';
% fs = 44100;
% frameSize = 2048; % size in samples
% 
% startSeg = 0;
% endSeg = 10;


%% compute MIDI

nmat = readmidi(filename);
%nmat = midi2nmat(filename);

if endSeg ~= 0
	nmat = onsetwindow(nmat, startSeg, endSeg, 'sec');
end

nmat = trim(nmat);

channels = mchannels(nmat);
nChannels = length(channels);

allMIDItracks = [];

%get individual MIDI tracks, and make them monophonic if necessary
MIDItracks = cell(nChannels, 1);
for i=1:nChannels
	MIDItracks{i} = getmidich(nmat,channels(i));
	if ~ismonophonic(MIDItracks{i})
		MIDItracks{i} = extreme(MIDItracks{i}); %make it monophonic!
	end
	allMIDItracks = [allMIDItracks; MIDItracks{i}];
end


%% compute Audio tracks %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

%allAUDIOtracks = nmat2snd(allMIDItracks, 'fm', fs);
%soundsc(allAUDIOtracks, fs)

AUDIOtracks = cell(nChannels,1);
durations = zeros(nChannels,1);
for i=1:nChannels
	AUDIOtracks{i} = nmat2snd(MIDItracks{i},'fm', fs); 
	%soundsc(AUDIOtracks{i}, fs)
	durations(i) = length(AUDIOtracks{i});
end

dur = min(durations);%in samples

AUDIOout=zeros(nChannels+1,dur);
for i=1:nChannels
	%add to mix channel
	AUDIOout(1,:) = AUDIOout(1,:)+(AUDIOtracks{i}(1,1:dur)/nChannels);
	%separate channel
	AUDIOout(i+1,:) = AUDIOtracks{i}(1,1:dur);
end


% plot(allAUDIOtracks(1:dur)/max(allAUDIOtracks(1:dur)), 'g');
% hold on
% plot(AUDIOout(1,:)/max(AUDIOout(1,:)), 'r');
% hold off

dur = dur/fs; %in seconds

%% compute active notes at each frame

fLength = frameSize/fs;

fStart = 0;
fEnd = fStart + fLength;

activeNotes = [];

%Sliding frames
while fStart < dur
	count = 0;
	%iterate over all MIDI notes
	for n=1: size(allMIDItracks, 1)
		s = allMIDItracks(n,6);
		e = s + allMIDItracks(n,7); 
		if  (s <= fStart) && (e >= fEnd)
		%if (s < fEnd) && (e > fStart)
			count = count +1;
		end
	end
	activeNotes = [activeNotes count];
	
	% slide frame
	fStart = fStart + fLength;
	fEnd = fStart + fLength;
end


