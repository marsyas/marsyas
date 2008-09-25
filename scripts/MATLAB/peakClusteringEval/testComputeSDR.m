function testComputeSDR()

nbRefs=2;
nbSyn=1;
frameLength = 1024;

ref = rand(nbRefs, 1024);
noise = rand(nbSyn, 1024)*0.000001;
syn = ref(nbSyn, :)+noise;

% permute syn vectors
nbSyn = size(syn, 1);
permSyn = perms((1:nbSyn));
index = ceil(rand(1, 1)*size(permSyn, 2));
syn = syn(permSyn(index, :), :);

sdr = computeSDR(ref, syn, frameLength)