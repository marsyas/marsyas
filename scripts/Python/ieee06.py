import os
from copy import copy 
from glob import glob

bin_path = "/Users/jennifermurdoch/src/marsyas_svn/marsyas/trunk/xcode/peakClustering/Debug_MAD_MATLAB_MRSASSERT_MRSAUDIO_MRSMIDI_MRSWARNING_READLINE/"
out_path = "/Users/jennifermurdoch/src/marsyas_svn/marsyas/trunk/src/batch_stuff/output"
wav_path = "/Users/jennifermurdoch/Documents/MSC_Work/spectral_learning/svn/soundcut/_data_set/mix/base/";


#endCommand = " -a -s -p 2 -c 3 -o output -N music  ";
endCommand = " -a -s -p 1 -c 3 -o " + out_path + " -t "
beginCommand = bin_path + "peakClustering "


type = ['a','f','h']
norm = ['o','n','b','l']
wav_primary = ["saxo.wav","violin.wav"]
wav_interfere = ["noise.wav","sweep.wav"]

n = len(type)

# Compute type combinations
type_params = [];
for k in range(1,n+1): # length of string
   t = range(0,k)    # t = [ 1...k ]   
   type_params.append(copy(t));   
   while k < n:
      t[k-1] += 1;     # t = t + 1      
      type_params.append(copy(t));      
      r = k-1
      while t[r] == n - (k-r) and r>0 and t[r-1] < n-r:
         t[r-1] += 1
         for c in range(r,k):
            t[c] = t[c-1] + 1;            
         type_params.append(copy(t));            
         r = r-1;
      if t[0]==n-k:
         break  
         
#print type_params                       

# Compute normalization combinations
norm_params = [];
for k in range(len(type)):
   if k > 0:
      next = [];   
      for j in range(len(norm_params[k-1])):
         for n in range(len(norm)):
            t = norm_params[k-1][j] + [n]
            next.append( copy(t) ) 
      norm_params.append(copy(next))
   else:
      next = []
      for k in range(len(norm)):
         next.append(copy([k]))
      norm_params.append(next);   
      
#print norm_params
   
# Combine the two and compute all possible values of -t arg
params_full = [];
for j in range(len(type_params)):
   l = len(type_params[j])
   for k in range(len(norm_params[l-1])):
      cur = "" 
      for n in range(len(type_params[j])):
         cur += type[type_params[j][n]]     
         cur += norm[norm_params[l-1][k][n]]
      params_full.append(copy(cur));
      
#print params_full      
      
# Compute all possible pairs of sound files, 1 from wav_primary, 1 from wav_interfere
wav_full = [];
for i in range(len(wav_primary)):
   for j in range(len(wav_interfere)):
      t = [ wav_primary[i] , wav_interfere[j] ]
      wav_full.append(copy(t))
      
#print wav_full
      
#for name in wav_full:
#   for param in params_full:
name = wav_full[0]
param = params_full[0]
command = beginCommand+endCommand+param+" "+wav_path+name[0]+" -N "+wav_path+name[1]
print command
c = str(command)
os.system(c)
      
