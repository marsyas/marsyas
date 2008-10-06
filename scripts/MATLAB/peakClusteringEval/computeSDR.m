function [maxSdr, correspondence] = computeSDR(ref, syn)

frameLength = size(ref,2);

if size(ref, 2) ~= frameLength
    error('The reference should be a matrix of line vectors, each of size frameLength');
end

if size(syn, 2) ~= frameLength
    error('The synthesis should be a matrix of line vectors, each of size frameLength');
end

nbRefs = size(ref, 1)
nbSyn = size(syn, 1);

% handle different numbers by padding zeros
if nbRefs < nbSyn
    ref = [ref; zeros(nbSyn-nbRefs, frameLength)];
elseif nbRefs > nbSyn
    syn = [syn; zeros(nbRefs-nbSyn, frameLength)];
end

nbRefs = size(ref, 1);
nbSyn = size(syn, 1);

permRefs = perms((1:nbRefs));
refPower = sum(abs(ref),2);%abs(sum(ref, 2)); ????

maxSdr = -80;
for i=1:size(permRefs, 1)
    permRefMat = ref(permRefs(i, :), :);
    permDiffMat = permRefMat-syn;

    permDiffVec = sum(abs(permDiffMat),2);%abs(sum(permDiffMat, 2)); ????
    permRefVec = refPower(permRefs(i, :));

    sdr = mean(20*log10((permRefVec./permDiffVec)+eps));

    if sdr > maxSdr
        maxSdr = sdr;
        correspondence = permRefs(i, :);
    end
end

