function testComputeSDR()

nbRefs=3;
% nbSyn should be below nbRefs (only in this test)
nbSyn=3;
frameLength = 1024;

ref = rand(nbRefs, 1024);
noise = rand(nbSyn, 1024)*0.000001;
syn = ref((1:nbSyn), :)+noise;

% permute syn vectors
nbSyn = size(syn, 1);
permSyn = perms((1:nbSyn));
index = ceil(rand(1, 1)*size(permSyn, 2));
syn = syn(permSyn(index, :), :);

sdr = computeSDR(ref, syn, frameLength)