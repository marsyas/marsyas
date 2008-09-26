function maxSdr = computeSDR(ref, syn)

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
if nbRefs < nbSyn
    ref = [ref; zeros(nbSyn-nbRefs, frameLength)];
elseif nbRefs > nbSyn
    syn = [syn; zeros(nbRefs-nbSyn, frameLength)];
end

nbRefs = size(ref, 1);
nbSyn = size(syn, 1);

permRefs = perms((1:nbRefs));
permSyn = perms((1:nbSyn));

maxSdr = -80;
for i=1:size(permRefs, 1)
    for j=1:size(permSyn, 1)
        sdr=0;
        for k=1:nbRefs
            sdr = sdr + computeVectorSDR(ref(permRefs(i, k), :), syn(permSyn(j, k), :));
        end
        sdr = sdr/nbRefs;
        if sdr > maxSdr
            maxSdr = sdr;
        end
    end
end

function sdr = computeVectorSDR(ref, syn)
% Signal to Noise Ratio computation between 2 vectors

sdr = 20*log10((sum(abs(ref))/sum(abs(ref-syn)))+eps);