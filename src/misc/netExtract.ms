# featureNetwork 
create Series featureNetwork
create NetworkTCPSource src
create Hamming ham 
create Spectrum spk 
create PowerSpectrum pspk 
add src > featureNetwork
add ham > featureNetwork
add spk > featureNetwork
add pspk > featureNetwork

# spectralFeatures 
create Fanout spectralFeatures
create Centroid cntrd
create Rolloff rlf
create Flux flx
create MFCC mfcc
add cntrd > spectralFeatures
add rlf > spectralFeatures
add flx > spectralFeatures
add mfcc > spectralFeatures

add spectralFeatures > featureNetwork

create Memory mem
add mem > featureNetwork

# textureStatistics
create Fanout textureStatistics 
create Mean textureMean 
create StandardDeviation textureStd
add textureMean > textureStatistics
add textureStd > textureStatistics

add textureStatistics > featureNetwork

# top level Series
create Series total 
create Accumulator acc 
updctrl acc mrs_natural/nTimes 1292
add featureNetwork > acc 
add acc > total 

# totalStatistics
create Fanout totalStatistics 
create Mean totalMean 
create StandardDeviation totalStd
add totalMean > totalStatistics
add totalStd > totalStatistics
add totalStatistics > total

# add the Annotator
create Annotator ann
updctrl ann mrs_natural/label 5
add ann > total 

# add a Network Sink
create NetworkTCPSink nsink
updctrl nsink string/host localhost 
updctrl nsink mrs_natural/dataPort 5011 
updctrl nsink mrs_natural/controlsPort 5012
add nsink > total

link total bool/hasData Accumulator/acc/Series/featureNetwork/NetworkTCPSource/src/mrs_bool/hasData
