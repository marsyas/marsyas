function [Eigenvectors,Eigenvalues] = ncut(W,nbEigenValues,dataNcut);
% function [Eigenvectors,Eigenvalues] = ncut(W,nbEigenValues,dataNcut);
% 
% Input:
%     W= symmetric similarity matrix
%     nbEigenValues=  number of Ncut eigenvectors computed
%     dataNcut= optional parameters
%
%     default parameters for dataNcut:
%     dataNcut.offset = 5e-1; offset in the diagonal of W
%     dataNcut.verbose = 0; 0 for verbose off mode, 1,2,3 for verbose on modes
%     dataNcut.maxiterations = 100; max number of iterations in eigensolver
%     dataNcut.eigsErrorTolerance = 1e-6; error tolerance in eigensolver
%     dataNcut.valeurMin=1e-6; % truncates any values in W less than valeurMin
% 
% Output: 
%    Eigenvectors= continuouse Ncut eigenvectos, size = length(W) x nbEigenValues
%    Eigenvalues= Ncut eigenvalues, size = 1x nbEigenValues
%
% Timothee Cour, Stella Yu, Jianbo Shi, 2004.

if nargin < 2
    nbEigenValues = 8;
end
if nargin < 3
    dataNcut.offset = 5e-1;
    dataNcut.verbose = 1; %0;
    dataNcut.maxiterations = 100;
    dataNcut.eigsErrorTolerance = 1e-6;
    dataNcut.valeurMin=1e-6;
end

% make W matrix sparse
W = sparsifyc(W,dataNcut.valeurMin);


% check for matrix symmetry
if max(max(abs(W-W'))) > 1e-10 %voir (-12) 
    %disp(max(max(abs(W-W'))));
    error('W not symmetric');
end

n = size(W,1);
nbEigenValues = min(nbEigenValues,n);
offset = dataNcut.offset;


% degrees and regularization
d = sum(abs(W),2);  % sum each row
dr = 0.5 * (d - sum(W,2));
d = d + offset * 2;
dr = dr + offset;
W = W + spdiags(dr,0,n,n); % usually adds 0.5 to the main diagonal

Dinvsqrt = 1./sqrt(d+eps);
P = spmtimesd(W,Dinvsqrt,Dinvsqrt); % reg. version of W (non-symmetric)
clear W;

options.issym = 1;
     
if dataNcut.verbose
    options.disp = 3; 
else
    options.disp = 0; 
end
options.maxit = dataNcut.maxiterations;
options.tol = dataNcut.eigsErrorTolerance;

options.v0 = ones(size(P,1),1);
options.p = max(35,2*nbEigenValues); %voir
options.p = min(options.p,n);



%warning off                

% vbar        = columns of eigenvectors         (V)
% s           = diagonal matrix of eigenvalues  (D)
% convergence = flag showing if all eigenvalues converged {0,1} (FLAG)
[vbar,s,convergence] = eigs2(@mex_w_times_x_symmetric,size(P,1),nbEigenValues,'LA',options,tril(P)); 
% @mex_w_times_x_symmetric  = W*X               (AFUN)
% size(P,1)                 = size( W*X )       (N)
% nbEigenValues             = num Eigenvalues   (K)
% 'LA'                      = Largest Algebraic eigenvalues (SIGMA)
% options                   = options of eigs2  (OPTS)
% tril(P)                   = Param of AFUN : lower triangle of P    (P1)

%warning on

s = real(diag(s)); 
% Create a vector of (+ve) eigenvalues from diagonal matrix
% eigenvalues are usually in descending order in s

[x,y] = sort(-s); 
% x are the (-ve) eigenvalues sorted in ascending order
% y are the indicies of the sort

Eigenvalues = -x; % make eigenvalues (+ve) again
vbar = vbar(:,y); % sort eigenvectors according to sort indicies y
Eigenvectors = spdiags(Dinvsqrt,0,n,n) * vbar; % diag. matrix of 1/d*eps * eigenvectors
   % n x nbEigenValues
   
for  i=1:size(Eigenvectors,2) % ie. 1:nbEigenValues
    Eigenvectors(:,i) = (Eigenvectors(:,i) / norm(Eigenvectors(:,i))  )*norm(ones(n,1));
    if Eigenvectors(1,i)~=0
        Eigenvectors(:,i) = - Eigenvectors(:,i) * sign(Eigenvectors(1,i));
    end
end
