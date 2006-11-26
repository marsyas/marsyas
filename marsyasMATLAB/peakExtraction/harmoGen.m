function harmoGen(f, nb, duration)

x=zeros(1, duration*44100);
t=(1:length(x));
for i=1:length(f)
   for j=1:nb
    x=x+(1/j+rand(1)*.1)*sin(2*pi*j*f(i)*t/44100);   
   end
end

x = x/(max(x)+.0001);

specgram(x, 2048);
sound(x, 44100);

wavwrite(x, 44100, '..\..\..\soundCut\_data_set\h.wav');