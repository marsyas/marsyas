clf
subplot(2, 1, 1);
hold on
text(f1, a1*1.2, num2str((1:length(f1))'-1));
stem(f1, a1);
text(f2, a2*1.4, num2str((1:length(f2))'-1));
stem(f2, a2);
stem(sf1, max(a1), 'rd');
stem(sf2, max(a2), 'rd');
subplot(2, 1, 2);
hold on
text(F1, A1*1.2, num2str((1:length(F1))'-1), 'Color', 'r');
stem(F1, A1, 'r*');
text(F2, A2*1.4, num2str((1:length(F2))'-1), 'Color','k');
stem(F2, A2, 'kd');