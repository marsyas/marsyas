function [] = plotGpPeaks(P, l)

maxGp = max(P(:, 7))+1;

if(l==-2)
    for i=0:maxGp
        M=P;
        index = find(P(:, 7) ~= i);
        M(index, 7) = -1;
        cla
    plotPeaks(M);
    i
    pause(1);
    end
    plotPeaks(P);
else
       M=P;
        index = find(P(:, 7) ~= l);
        M(index, 7) = -1;
        M(1:2,:)
    plotPeaks(M); 
end

 