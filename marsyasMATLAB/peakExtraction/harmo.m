function harmo()

w = 0.1;
%   f=(1:100);
%    plot(hFunction(f, 10, w));
%   return

% first set with four harmonics and two noisy peaks
f=[440, 511, 880, 1320, 1760, 2173];
a=[1, .2, .8, .6, .4, .9];
% second set with two sets of harmonics
f=[440, 550, 880, 1100, 1320, 1650, 1760, 2200, 2750];
a=[1, 1, .8, .8, .6, .6, .4, .4, .4];

% f=[f (1:4)*445];
% a=[a ones(1, 4)];
% 
% add noise to the parameters estimates
f=f+rand(1, length(f))*5;
a=a+rand(1, length(f))*0.1;

% f = (f-mean(f))/std(f);
% a = (a-mean(a))/std(a);

debug=1;
m = ones(length(f));

for i=1:length(f)
    for j=1:i-1
        f(i)
        f(j)
        %         weighting
        f1=f;f2=f;
        % fundamental frequency estimate for modulus
        hF = min([f(i), f(j)]);
%         hF = divisor(f(i), f(j));
        % weighting function considering minimal ff estimate
        A1=hFunction(f1, hF, w);
        A2=hFunction(f2, hF, w);
        % aligning the spectra of the two peaks
        f1 = f-f(i);
        f2 = f-(f(j));
hF = min([f(i), f(j)]);
        % modulus apply (core idea to correlate spectra relevantly)
        f1 = f1./f(j);
        f2 = f2./f(i);
        f1=mod(f1, 1);
        f2= mod(f2, 1);
        % plotting
        if(debug)
            clf
            subplot(2, 1, 1);
            hold on
            text(f, a+.3, num2str((1:length(f))'-1));
            stem(f, a);
            stem(f(i), a(i), 'rd');
            stem(f(j), a(j), 'rd');
            subplot(2, 1, 2);
            hold on
            text(f1, (A1.*a)*1.2, num2str((1:length(f))'-1), 'Color', 'r');
            stem(f1, (A1.*a), 'r*');
            text(f2, (A2.*a)*1.4, num2str((1:length(f))'-1), 'Color','k');
            stem(f2, (A2.*a), 'kd');
        end
        % compute spectra correlation
        val = computeCorrelation(f1, f2, A1.*a, A2.*a)
        % fill similarity matrix
        m(i,j)=val;
        m(j,i)=val;
    end
end

imagesc(m);

function res = computeCorrelation(f1, f2, a1, a2)

% activity flags for peaks
d1 = ones(1, length(f1));
d2 = ones(1, length(f2));

res =[];
Ind1=[];
Ind2=[];
for k=1:length(f1)
    % look for the closest peaks couple in frequency
    minDiff = 44100*2;
    ind2=0;
    ind1=0;
    for i=1:length(f2)
        for j=1:length(f1)
            if(d2(i) == 1 && d1(j) == 1)
                % the spectrum is cyclic due to the application of the
                % modulus
                df = max(f1(j),f2(i))-min(f1(j),f2(i)); % inside Case
                if (df>(1-max(f1(j),f2(i))+min(f1(j),f2(i)))) % outside Case
                    df = (1-max(f1(j),f2(i))+min(f1(j),f2(i)));
                end
                % select the closest couple in frequency
                if(df < minDiff)
                    minDiff = df;
                    ind1 = j;
                    ind2 = i;
                end
            end
        end
    end
    % compute similarity
    val = a1(ind1)*a2(ind2)/... % contribution of the correlate
        (minDiff^2/std([f1 f2])+0.000000000000000000001); % weighted by the frequency proximity

    % store datas
    Ind1 = [Ind1 ind1];
    Ind2 = [Ind2 ind2];
    res = [res val];
    % remove the two peaks from sets
    d1(ind1) = 0;
    d2(ind2) = 0;
end

% Ind1
% Ind2
% res

res = mean(res);

function A = hFunction(F, f, w)
% weighting function
% may be set to 1 when F<f
indexes = find(F<f);
A(indexes)=1;
indexes = find(F>=f);
% indexes=(1:length(F));
A(indexes) = bFunction(F(indexes), f).^(-w/log(bFunction(1,f)));

function A = bFunction(F, f)
% basic weighting function
A = (1+cos(2*pi*F/f))/2;


