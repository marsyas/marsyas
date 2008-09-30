function testComputeSDR()

nbRefs=5;
% nbSyn should be below or equal to nbRefs (only in this test)
nbSyn=5;
frameLength = 1024*40;

ref = rand(nbRefs, frameLength);
noise = rand(nbSyn, frameLength)*0.0000001;
syn = ref((1:nbSyn), :)+noise;

% permute syn vectors
%nbSyn = size(syn, 1);
%permSyn = perms((1:nbSyn));
%index = ceil(rand(1, 1)*size(permSyn, 2));
%syn = syn(permSyn(index, :), :);

sdr = computeSDR(ref, syn)