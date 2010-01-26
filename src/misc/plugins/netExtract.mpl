# MarSystemComposite
# Type = Series
# Name = extractNet

# MarControls = 8
# /Series/extractNet/mrs_bool/debug = 0
# /Series/extractNet/mrs_bool/mute = 0
# /Series/extractNet/mrs_natural/inObservations = 1
# /Series/extractNet/mrs_natural/inSamples = 512
# /Series/extractNet/mrs_natural/onObservations = 1
# /Series/extractNet/mrs_natural/onSamples = 1
# /Series/extractNet/mrs_real/israte = 22050
# /Series/extractNet/mrs_real/osrate = 43.0664

# Number of links = 1
# Synonyms of /Series/extractNet/mrs_bool/hasData = 
# Number of synonyms = 1
# /Series/extractNet/NetworkTCPSource/src/mrs_bool/hasData

# nComponents = 6

# MarSystem
# Type = NetworkTCPSource
# Name = src

# MarControls = 12
# /NetworkTCPSource/src/mrs_bool/debug = 0
# /NetworkTCPSource/src/mrs_bool/mute = 0
# /NetworkTCPSource/src/mrs_bool/hasData = 1
# /NetworkTCPSource/src/mrs_natural/controlsPort = 5010
# /NetworkTCPSource/src/mrs_natural/dataPort = 5009
# /NetworkTCPSource/src/mrs_natural/inObservations = 1
# /NetworkTCPSource/src/mrs_natural/inSamples = 512
# /NetworkTCPSource/src/mrs_natural/onObservations = 1
# /NetworkTCPSource/src/mrs_natural/onSamples = 512
# /NetworkTCPSource/src/mrs_real/gain = 1
# /NetworkTCPSource/src/mrs_real/israte = 22050
# /NetworkTCPSource/src/mrs_real/osrate = 22050

# Number of links = 0

# MarSystem
# Type = Hamming
# Name = ham

# MarControls = 8
# /Hamming/ham/mrs_bool/debug = 0
# /Hamming/ham/mrs_bool/mute = 0
# /Hamming/ham/mrs_natural/inObservations = 1
# /Hamming/ham/mrs_natural/inSamples = 512
# /Hamming/ham/mrs_natural/onObservations = 1
# /Hamming/ham/mrs_natural/onSamples = 512
# /Hamming/ham/mrs_real/israte = 22050
# /Hamming/ham/mrs_real/osrate = 22050

# Number of links = 0

# MarSystem
# Type = Spectrum
# Name = spectrum

# MarControls = 8
# /Spectrum/spectrum/mrs_bool/debug = 0
# /Spectrum/spectrum/mrs_bool/mute = 0
# /Spectrum/spectrum/mrs_natural/inObservations = 1
# /Spectrum/spectrum/mrs_natural/inSamples = 512
# /Spectrum/spectrum/mrs_natural/onObservations = 512
# /Spectrum/spectrum/mrs_natural/onSamples = 1
# /Spectrum/spectrum/mrs_real/israte = 22050
# /Spectrum/spectrum/mrs_real/osrate = 43.0664

# Number of links = 0

# MarSystem
# Type = PowerSpectrum
# Name = psk

# MarControls = 9
# /PowerSpectrum/psk/mrs_bool/debug = 0
# /PowerSpectrum/psk/mrs_bool/mute = 0
# /PowerSpectrum/psk/mrs_natural/inObservations = 512
# /PowerSpectrum/psk/mrs_natural/inSamples = 1
# /PowerSpectrum/psk/mrs_natural/onObservations = 256
# /PowerSpectrum/psk/mrs_natural/onSamples = 1
# /PowerSpectrum/psk/mrs_real/israte = 43.0664
# /PowerSpectrum/psk/mrs_real/osrate = 43.0664
# /PowerSpectrum/psk/mrs_string/spectrumType = power

# Number of links = 0

# MarSystem
# Type = Centroid
# Name = cntrd

# MarControls = 8
# /Centroid/cntrd/mrs_bool/debug = 0
# /Centroid/cntrd/mrs_bool/mute = 0
# /Centroid/cntrd/mrs_natural/inObservations = 256
# /Centroid/cntrd/mrs_natural/inSamples = 1
# /Centroid/cntrd/mrs_natural/onObservations = 1
# /Centroid/cntrd/mrs_natural/onSamples = 1
# /Centroid/cntrd/mrs_real/israte = 43.0664
# /Centroid/cntrd/mrs_real/osrate = 43.0664

# Number of links = 0

# MarSystem
# Type = NetworkTCPSink
# Name = dest

# MarControls = 12
# /NetworkTCPSink/dest/mrs_bool/debug = 0
# /NetworkTCPSink/dest/mrs_bool/mute = 0
# /NetworkTCPSink/dest/mrs_natural/controlsPort = 5010
# /NetworkTCPSink/dest/mrs_natural/dataPort = 5009
# /NetworkTCPSink/dest/mrs_natural/inObservations = 1
# /NetworkTCPSink/dest/mrs_natural/inSamples = 1
# /NetworkTCPSink/dest/mrs_natural/onObservations = 1
# /NetworkTCPSink/dest/mrs_natural/onSamples = 1
# /NetworkTCPSink/dest/mrs_real/gain = 1
# /NetworkTCPSink/dest/mrs_real/israte = 43.0664
# /NetworkTCPSink/dest/mrs_real/osrate = 43.0664
# /NetworkTCPSink/dest/mrs_string/host = cyprus.cs.uvic.ca

# Number of links = 0

