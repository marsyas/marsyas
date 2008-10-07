%% A test for PlotSink: plot all 10 tracks from the synth bank
for c=1:10
    subplot(10,1,c)
    plot(PlotSink_send2MATLAB_indata(c,:));
    title(['num active notes: ' num2str(numActiveNotes)]);
    %sound(PlotSink_send2MATLAB_indata(c,:));
end