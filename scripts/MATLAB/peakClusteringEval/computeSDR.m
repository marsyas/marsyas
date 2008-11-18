function [maxSdr, correspondence] = computeSDR(ref, syn)

frameLength = size(ref,2);

if size(ref, 2) ~= frameLength
    error('The reference should be a matrix of line vectors, each of size frameLength');
end

if size(syn, 2) ~= frameLength
    error('The synthesis should be a matrix of line vectors, each of size frameLength');
end

nbRefs = size(ref, 1);
nbSyn = size(syn, 1);

% handle different numbers by padding zeros
%if nbRefs < nbSyn %this situation is not expected to happen... [!]
%    ref = [ref; zeros(nbSyn-nbRefs, frameLength)];
%elseif nbRefs > nbSyn
%    syn = [syn; zeros(nbRefs-nbSyn, frameLength)];
%end

%nbRefs = size(ref, 1);
%nbSyn = size(syn, 1);

correspondence = [1:nbSyn]; %by default...

%permRefs = perms((1:nbRefs));
permSyns = perms((1:nbSyn));

refPower = sum(abs(ref),2);%abs(sum(ref, 2)); ????

maxSdr = -80;
for i=1:size(permSyns,1)%:size(permRefs, 1)
    %permRefMat = ref(permRefs(i, :), :);
    permSynMat = syn(permSyns(i, :), :);
    
    %silence extra resynthesis tracks, so they do no hurt SDR
    if nbRefs < nbSyn
        %permSynMat(nbRefs+1:end,:) = permSynMat(nbRefs+1:end,:)*0;
        permSynMat = permSynMat(1:nbRefs,:);
    end
    
    %permDiffMat = permRefMat-syn;
    permDiffMat = ref - permSynMat;

    permDiffPower = sum(abs(permDiffMat),2);%abs(sum(permDiffMat, 2)); ????
    
    %permRefVec = refPower(permRefs(i, :));
    %sdr = mean(20*log10((permRefVec./permDiffVec)+eps));
    sdr = mean(20*log10((refPower./permDiffPower)+eps));

    if sdr > maxSdr
        maxSdr = sdr;
        %correspondence = permRefs(i, :);
        correspondence = permSyns(i,:);
    end
end

