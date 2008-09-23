%% compute SDR for all frames in current TextureWindow

%get resynthesized audio frame from PlotSink MarSystem.
%each row has the audio from each cluster 
%(i.e. only the first numActiveNotes rows contain audio... the others
%should be silence).
clustersAudio = PlotSink_send2MATLAB_indata; % just a rename...

TODO TODO TODO

%% init vars for next texture window
numActiveNotes = 0;
