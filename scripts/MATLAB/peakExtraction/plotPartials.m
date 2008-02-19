function [] = plotPartials(partials, fs)

cm = colormap ;
nb_coul = 64 ;
size_pt = 1 ;
inc_x = 1 ;
inc_y = 1 ;

hold on

nb_partial = length (partials);
for i=1:nb_partial
    pos = partials(i).pos ;
    freq = partials(i).freq*fs ;
    amp =  partials(i).amp ;

    coul = cm (mod(ceil(rand(1, 1)*63),64), :) ;

    last = length(pos) ;
    linewidth = 1;%+(log10(mean(partials(i).amp))+4)*size_pt
    userdata = [i] ;

    plot ((pos(1)-1)*inc_x, freq(1)*inc_y, 'ro', ...
        'MarkerSize', 15) ;
    line ((pos((1:last))-1)*inc_x, freq((1:last))*inc_y, 'Color', coul,...
        'LineWidth', linewidth, 'LineStyle', '-', ...
        'ButtonDownFcn', @plotPartialData, 'UserData', userdata) ... 
        ;
    plot ((pos(last)-1)*inc_x, freq(last)*inc_y, 'ro', ...
        'MarkerSize', 15) ;
end

hold off

         
function plotPartialData(src, evnt)


handles = guidata(gcf);

i = get(src, 'UserData');

if(handles.figure2 == 0)
    handles.figure2 = figure;
end

figure(handles.figure2)

% subplot(3, 1, 1)
% plot(handles.P(i).freq);
% xlabel('Time (frames)');
% ylabel('Frequency');
% subplot(3, 1, 2)

dampingValue = median(handles.P(i).damping) %sum(handles.P(i).damping.*handles.P(i).amp)/sum(handles.P(i).amp);

x=(1:length(handles.P(i).amp))'-1;
approx = exp(dampingValue*x*16*8)*max(handles.P(i).amp);

damping=handles.P(i).damping;
damping(damping>0)=0;
approx2 = exp(damping'.*x*16*8).*max(handles.P(i).amp);

plot(x, handles.P(i).amp, x, approx, x, approx2);

xlabel('Time (frames)');
ylabel('Amplitude');
legend('Measured', 'Median Damping', 'Instantaneous Damping');

% subplot(3, 1, 3)
% plot(handles.P(i).damping);
% xlabel('Time (frames)');
% ylabel('Damping');


guidata(handles.figure1, handles);
