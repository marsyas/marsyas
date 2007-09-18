function []  = stemPeaks(P)

[Min, indMin] = min(P((2:end), 8))
indMin=indMin+1;
[Max, indMax] = max(P((2:end), 8));
indMax=indMax+1;
subplot(2, 1, 1);

stem(P(:, 1), P(:, 2));
hold on
stem(P(indMin, 1), P(indMin, 2), 'r');
stem(P(indMax, 1), P(indMax, 2), 'k');
hold off

subplot(2, 1, 2);
plot(P((2:end), 1), log10(P((2:end), 8)), '*');

