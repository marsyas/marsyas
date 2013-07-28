
input = load("late-test-input.txt");
output = load("late-test-output.txt")';

myout = info_histogram(60, input, 0.05);
myout = myout(1:end-1);

%myout
%output

delta = myout - output



