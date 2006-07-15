# MarSystemComposite
# Type = Series
# Name = featureNetwork

# MarControls = 20
# /Series/featureNetwork/mrs_bool/active = 1
# /Series/featureNetwork/mrs_bool/debug = 0
# /Series/featureNetwork/mrs_bool/mute = 0
# /Series/featureNetwork/mrs_bool/probe = 0
# /Series/featureNetwork/mrs_natural/inObservations = 1
# /Series/featureNetwork/mrs_natural/inSamples = 512
# /Series/featureNetwork/mrs_natural/onObservations = 2
# /Series/featureNetwork/mrs_natural/onSamples = 1
# /Series/featureNetwork/mrs_real/israte = 22050
# /Series/featureNetwork/mrs_real/osrate = 43.0664
# /Series/featureNetwork/mrs_realvec/input0 = # MARSYAS mrs_realvec
# Size = 0


# type: matrix
# rows: 1
# columns: 0


# Size = 0
# MARSYAS mrs_realvec

# /Series/featureNetwork/mrs_realvec/input1 = # MARSYAS mrs_realvec
# Size = 0


# type: matrix
# rows: 1
# columns: 0


# Size = 0
# MARSYAS mrs_realvec

# /Series/featureNetwork/mrs_realvec/input2 = # MARSYAS mrs_realvec
# Size = 0


# type: matrix
# rows: 1
# columns: 0


# Size = 0
# MARSYAS mrs_realvec

# /Series/featureNetwork/mrs_realvec/input3 = # MARSYAS mrs_realvec
# Size = 0


# type: matrix
# rows: 1
# columns: 0


# Size = 0
# MARSYAS mrs_realvec

# /Series/featureNetwork/mrs_realvec/input4 = # MARSYAS mrs_realvec
# Size = 0


# type: matrix
# rows: 1
# columns: 0


# Size = 0
# MARSYAS mrs_realvec

# /Series/featureNetwork/mrs_realvec/input5 = # MARSYAS mrs_realvec
# Size = 0


# type: matrix
# rows: 1
# columns: 0


# Size = 0
# MARSYAS mrs_realvec

# /Series/featureNetwork/mrs_realvec/input6 = # MARSYAS mrs_realvec
# Size = 0


# type: matrix
# rows: 1
# columns: 0


# Size = 0
# MARSYAS mrs_realvec

# /Series/featureNetwork/mrs_realvec/input7 = # MARSYAS mrs_realvec
# Size = 0


# type: matrix
# rows: 1
# columns: 0


# Size = 0
# MARSYAS mrs_realvec

# /Series/featureNetwork/mrs_string/inObsNames = audio,
# /Series/featureNetwork/mrs_string/onObsNames = ,

# Number of links = 5
# Synonyms of /Series/featureNetwork/mrs_bool/notEmpty = 
# Number of synonyms = 1
# /Series/featureNetwork/SoundFileSource/src/mrs_bool/notEmpty
# Synonyms of /Series/featureNetwork/mrs_natural/nChannels = 
# Number of synonyms = 2
# /Series/featureNetwork/SoundFileSource/src/mrs_natural/nChannels
# /Series/featureNetwork/AudioSink/dest/mrs_natural/nChannels
# Synonyms of /Series/featureNetwork/mrs_natural/pos = 
# Number of synonyms = 1
# /Series/featureNetwork/SoundFileSource/src/mrs_natural/pos
# Synonyms of /Series/featureNetwork/mrs_real/israte = 
# Number of synonyms = 1
# /Series/featureNetwork/SoundFileSource/src/mrs_real/israte
# Synonyms of /Series/featureNetwork/mrs_string/filename = 
# Number of synonyms = 1
# /Series/featureNetwork/SoundFileSource/src/mrs_string/filename

# nComponents = 9

# MarSystem
# Type = SoundFileSource
# Name = src

# MarControls = 27
# /SoundFileSource/src/mrs_bool/active = 1
# /SoundFileSource/src/mrs_bool/advance = 0
# /SoundFileSource/src/mrs_bool/debug = 0
# /SoundFileSource/src/mrs_bool/mute = 0
# /SoundFileSource/src/mrs_bool/notEmpty = 0
# /SoundFileSource/src/mrs_bool/noteon = 0
# /SoundFileSource/src/mrs_bool/shuffle = 0
# /SoundFileSource/src/mrs_natural/cindex = 0
# /SoundFileSource/src/mrs_natural/inObservations = 1
# /SoundFileSource/src/mrs_natural/inSamples = 512
# /SoundFileSource/src/mrs_natural/loopPos = 0
# /SoundFileSource/src/mrs_natural/nChannels = 1
# /SoundFileSource/src/mrs_natural/numFiles = 1
# /SoundFileSource/src/mrs_natural/onObservations = 1
# /SoundFileSource/src/mrs_natural/onSamples = 512
# /SoundFileSource/src/mrs_natural/pos = 661500
# /SoundFileSource/src/mrs_natural/size = 661500
# /SoundFileSource/src/mrs_real/duration = -1
# /SoundFileSource/src/mrs_real/frequency = 0
# /SoundFileSource/src/mrs_real/israte = 22050
# /SoundFileSource/src/mrs_real/osrate = 22050
# /SoundFileSource/src/mrs_real/repetitions = 1
# /SoundFileSource/src/mrs_string/allfilenames = ,
# /SoundFileSource/src/mrs_string/currentlyPlaying = daufile
# /SoundFileSource/src/mrs_string/filename = /home/gtzan/data/sound/music_speech/speech/shannon.au
# /SoundFileSource/src/mrs_string/inObsNames = audio,
# /SoundFileSource/src/mrs_string/onObsNames = audio,

# Number of links = 0

# MarSystem
# Type = AudioSink
# Name = dest

# MarControls = 14
# /AudioSink/dest/mrs_bool/active = 1
# /AudioSink/dest/mrs_bool/debug = 0
# /AudioSink/dest/mrs_bool/init = 0
# /AudioSink/dest/mrs_bool/mute = 0
# /AudioSink/dest/mrs_natural/bufferSize = 512
# /AudioSink/dest/mrs_natural/inObservations = 1
# /AudioSink/dest/mrs_natural/inSamples = 512
# /AudioSink/dest/mrs_natural/nChannels = 1
# /AudioSink/dest/mrs_natural/onObservations = 1
# /AudioSink/dest/mrs_natural/onSamples = 512
# /AudioSink/dest/mrs_real/israte = 22050
# /AudioSink/dest/mrs_real/osrate = 22050
# /AudioSink/dest/mrs_string/inObsNames = audio,
# /AudioSink/dest/mrs_string/onObsNames = audio,

# Number of links = 0

# MarSystemComposite
# Type = Fanout
# Name = features

# MarControls = 13
# /Fanout/features/mrs_bool/active = 1
# /Fanout/features/mrs_bool/debug = 0
# /Fanout/features/mrs_bool/mute = 0
# /Fanout/features/mrs_bool/probe = 0
# /Fanout/features/mrs_natural/disable = -1
# /Fanout/features/mrs_natural/inObservations = 1
# /Fanout/features/mrs_natural/inSamples = 512
# /Fanout/features/mrs_natural/onObservations = 17
# /Fanout/features/mrs_natural/onSamples = 1
# /Fanout/features/mrs_real/israte = 22050
# /Fanout/features/mrs_real/osrate = 43.0664
# /Fanout/features/mrs_string/inObsNames = audio,
# /Fanout/features/mrs_string/onObsNames = Centroid,Kurtosis,Rolloff,Flux,MFCC_0,MFCC_1,MFCC_2,MFCC_3,MFCC_4,MFCC_5,MFCC_6,MFCC_7,MFCC_8,MFCC_9,MFCC_10,MFCC_11,MFCC_12,

# Number of links = 0

# nComponents = 1

# MarSystemComposite
# Type = Series
# Name = SpectralShape

# MarControls = 15
# /Series/SpectralShape/mrs_bool/active = 1
# /Series/SpectralShape/mrs_bool/debug = 0
# /Series/SpectralShape/mrs_bool/mute = 0
# /Series/SpectralShape/mrs_bool/probe = 0
# /Series/SpectralShape/mrs_natural/inObservations = 1
# /Series/SpectralShape/mrs_natural/inSamples = 512
# /Series/SpectralShape/mrs_natural/onObservations = 17
# /Series/SpectralShape/mrs_natural/onSamples = 1
# /Series/SpectralShape/mrs_real/israte = 22050
# /Series/SpectralShape/mrs_real/osrate = 43.0664
# /Series/SpectralShape/mrs_realvec/input0 = # MARSYAS mrs_realvec
# Size = 0


# type: matrix
# rows: 1
# columns: 0


# Size = 0
# MARSYAS mrs_realvec

# /Series/SpectralShape/mrs_realvec/input1 = # MARSYAS mrs_realvec
# Size = 0


# type: matrix
# rows: 1
# columns: 0


# Size = 0
# MARSYAS mrs_realvec

# /Series/SpectralShape/mrs_realvec/input2 = # MARSYAS mrs_realvec
# Size = 0


# type: matrix
# rows: 1
# columns: 0


# Size = 0
# MARSYAS mrs_realvec

# /Series/SpectralShape/mrs_string/inObsNames = audio,
# /Series/SpectralShape/mrs_string/onObsNames = Centroid,Kurtosis,Rolloff,Flux,MFCC_0,MFCC_1,MFCC_2,MFCC_3,MFCC_4,MFCC_5,MFCC_6,MFCC_7,MFCC_8,MFCC_9,MFCC_10,MFCC_11,MFCC_12,

# Number of links = 0

# nComponents = 4

# MarSystem
# Type = Hamming
# Name = hamming

# MarControls = 11
# /Hamming/hamming/mrs_bool/active = 1
# /Hamming/hamming/mrs_bool/debug = 0
# /Hamming/hamming/mrs_bool/mute = 0
# /Hamming/hamming/mrs_natural/inObservations = 1
# /Hamming/hamming/mrs_natural/inSamples = 512
# /Hamming/hamming/mrs_natural/onObservations = 1
# /Hamming/hamming/mrs_natural/onSamples = 512
# /Hamming/hamming/mrs_real/israte = 22050
# /Hamming/hamming/mrs_real/osrate = 22050
# /Hamming/hamming/mrs_string/inObsNames = audio,
# /Hamming/hamming/mrs_string/onObsNames = audio,

# Number of links = 0

# MarSystem
# Type = Spectrum
# Name = spk

# MarControls = 13
# /Spectrum/spk/mrs_bool/active = 1
# /Spectrum/spk/mrs_bool/debug = 0
# /Spectrum/spk/mrs_bool/mute = 0
# /Spectrum/spk/mrs_natural/inObservations = 1
# /Spectrum/spk/mrs_natural/inSamples = 512
# /Spectrum/spk/mrs_natural/onObservations = 512
# /Spectrum/spk/mrs_natural/onSamples = 1
# /Spectrum/spk/mrs_real/cutoff = 1
# /Spectrum/spk/mrs_real/israte = 22050
# /Spectrum/spk/mrs_real/lowcutoff = 0
# /Spectrum/spk/mrs_real/osrate = 43.0664
# /Spectrum/spk/mrs_string/inObsNames = audio,
# /Spectrum/spk/mrs_string/onObsNames = rbin_0,ibin_0,rbin_1,ibin_1,rbin_2,ibin_2,rbin_3,ibin_3,rbin_4,ibin_4,rbin_5,ibin_5,rbin_6,ibin_6,rbin_7,ibin_7,rbin_8,ibin_8,rbin_9,ibin_9,rbin_10,ibin_10,rbin_11,ibin_11,rbin_12,ibin_12,rbin_13,ibin_13,rbin_14,ibin_14,rbin_15,ibin_15,rbin_16,ibin_16,rbin_17,ibin_17,rbin_18,ibin_18,rbin_19,ibin_19,rbin_20,ibin_20,rbin_21,ibin_21,rbin_22,ibin_22,rbin_23,ibin_23,rbin_24,ibin_24,rbin_25,ibin_25,rbin_26,ibin_26,rbin_27,ibin_27,rbin_28,ibin_28,rbin_29,ibin_29,rbin_30,ibin_30,rbin_31,ibin_31,rbin_32,ibin_32,rbin_33,ibin_33,rbin_34,ibin_34,rbin_35,ibin_35,rbin_36,ibin_36,rbin_37,ibin_37,rbin_38,ibin_38,rbin_39,ibin_39,rbin_40,ibin_40,rbin_41,ibin_41,rbin_42,ibin_42,rbin_43,ibin_43,rbin_44,ibin_44,rbin_45,ibin_45,rbin_46,ibin_46,rbin_47,ibin_47,rbin_48,ibin_48,rbin_49,ibin_49,rbin_50,ibin_50,rbin_51,ibin_51,rbin_52,ibin_52,rbin_53,ibin_53,rbin_54,ibin_54,rbin_55,ibin_55,rbin_56,ibin_56,rbin_57,ibin_57,rbin_58,ibin_58,rbin_59,ibin_59,rbin_60,ibin_60,rbin_61,ibin_61,rbin_62,ibin_62,rbin_63,ibin_63,rbin_64,ibin_64,rbin_65,ibin_65,rbin_66,ibin_66,rbin_67,ibin_67,rbin_68,ibin_68,rbin_69,ibin_69,rbin_70,ibin_70,rbin_71,ibin_71,rbin_72,ibin_72,rbin_73,ibin_73,rbin_74,ibin_74,rbin_75,ibin_75,rbin_76,ibin_76,rbin_77,ibin_77,rbin_78,ibin_78,rbin_79,ibin_79,rbin_80,ibin_80,rbin_81,ibin_81,rbin_82,ibin_82,rbin_83,ibin_83,rbin_84,ibin_84,rbin_85,ibin_85,rbin_86,ibin_86,rbin_87,ibin_87,rbin_88,ibin_88,rbin_89,ibin_89,rbin_90,ibin_90,rbin_91,ibin_91,rbin_92,ibin_92,rbin_93,ibin_93,rbin_94,ibin_94,rbin_95,ibin_95,rbin_96,ibin_96,rbin_97,ibin_97,rbin_98,ibin_98,rbin_99,ibin_99,rbin_100,ibin_100,rbin_101,ibin_101,rbin_102,ibin_102,rbin_103,ibin_103,rbin_104,ibin_104,rbin_105,ibin_105,rbin_106,ibin_106,rbin_107,ibin_107,rbin_108,ibin_108,rbin_109,ibin_109,rbin_110,ibin_110,rbin_111,ibin_111,rbin_112,ibin_112,rbin_113,ibin_113,rbin_114,ibin_114,rbin_115,ibin_115,rbin_116,ibin_116,rbin_117,ibin_117,rbin_118,ibin_118,rbin_119,ibin_119,rbin_120,ibin_120,rbin_121,ibin_121,rbin_122,ibin_122,rbin_123,ibin_123,rbin_124,ibin_124,rbin_125,ibin_125,rbin_126,ibin_126,rbin_127,ibin_127,rbin_128,ibin_128,rbin_129,ibin_129,rbin_130,ibin_130,rbin_131,ibin_131,rbin_132,ibin_132,rbin_133,ibin_133,rbin_134,ibin_134,rbin_135,ibin_135,rbin_136,ibin_136,rbin_137,ibin_137,rbin_138,ibin_138,rbin_139,ibin_139,rbin_140,ibin_140,rbin_141,ibin_141,rbin_142,ibin_142,rbin_143,ibin_143,rbin_144,ibin_144,rbin_145,ibin_145,rbin_146,ibin_146,rbin_147,ibin_147,rbin_148,ibin_148,rbin_149,ibin_149,rbin_150,ibin_150,rbin_151,ibin_151,rbin_152,ibin_152,rbin_153,ibin_153,rbin_154,ibin_154,rbin_155,ibin_155,rbin_156,ibin_156,rbin_157,ibin_157,rbin_158,ibin_158,rbin_159,ibin_159,rbin_160,ibin_160,rbin_161,ibin_161,rbin_162,ibin_162,rbin_163,ibin_163,rbin_164,ibin_164,rbin_165,ibin_165,rbin_166,ibin_166,rbin_167,ibin_167,rbin_168,ibin_168,rbin_169,ibin_169,rbin_170,ibin_170,rbin_171,ibin_171,rbin_172,ibin_172,rbin_173,ibin_173,rbin_174,ibin_174,rbin_175,ibin_175,rbin_176,ibin_176,rbin_177,ibin_177,rbin_178,ibin_178,rbin_179,ibin_179,rbin_180,ibin_180,rbin_181,ibin_181,rbin_182,ibin_182,rbin_183,ibin_183,rbin_184,ibin_184,rbin_185,ibin_185,rbin_186,ibin_186,rbin_187,ibin_187,rbin_188,ibin_188,rbin_189,ibin_189,rbin_190,ibin_190,rbin_191,ibin_191,rbin_192,ibin_192,rbin_193,ibin_193,rbin_194,ibin_194,rbin_195,ibin_195,rbin_196,ibin_196,rbin_197,ibin_197,rbin_198,ibin_198,rbin_199,ibin_199,rbin_200,ibin_200,rbin_201,ibin_201,rbin_202,ibin_202,rbin_203,ibin_203,rbin_204,ibin_204,rbin_205,ibin_205,rbin_206,ibin_206,rbin_207,ibin_207,rbin_208,ibin_208,rbin_209,ibin_209,rbin_210,ibin_210,rbin_211,ibin_211,rbin_212,ibin_212,rbin_213,ibin_213,rbin_214,ibin_214,rbin_215,ibin_215,rbin_216,ibin_216,rbin_217,ibin_217,rbin_218,ibin_218,rbin_219,ibin_219,rbin_220,ibin_220,rbin_221,ibin_221,rbin_222,ibin_222,rbin_223,ibin_223,rbin_224,ibin_224,rbin_225,ibin_225,rbin_226,ibin_226,rbin_227,ibin_227,rbin_228,ibin_228,rbin_229,ibin_229,rbin_230,ibin_230,rbin_231,ibin_231,rbin_232,ibin_232,rbin_233,ibin_233,rbin_234,ibin_234,rbin_235,ibin_235,rbin_236,ibin_236,rbin_237,ibin_237,rbin_238,ibin_238,rbin_239,ibin_239,rbin_240,ibin_240,rbin_241,ibin_241,rbin_242,ibin_242,rbin_243,ibin_243,rbin_244,ibin_244,rbin_245,ibin_245,rbin_246,ibin_246,rbin_247,ibin_247,rbin_248,ibin_248,rbin_249,ibin_249,rbin_250,ibin_250,rbin_251,ibin_251,rbin_252,ibin_252,rbin_253,ibin_253,rbin_254,ibin_254,rbin_255,ibin_255,

# Number of links = 0

# MarSystem
# Type = PowerSpectrum
# Name = pspk

# MarControls = 12
# /PowerSpectrum/pspk/mrs_bool/active = 1
# /PowerSpectrum/pspk/mrs_bool/debug = 0
# /PowerSpectrum/pspk/mrs_bool/mute = 0
# /PowerSpectrum/pspk/mrs_natural/inObservations = 512
# /PowerSpectrum/pspk/mrs_natural/inSamples = 1
# /PowerSpectrum/pspk/mrs_natural/onObservations = 256
# /PowerSpectrum/pspk/mrs_natural/onSamples = 1
# /PowerSpectrum/pspk/mrs_real/israte = 43.0664
# /PowerSpectrum/pspk/mrs_real/osrate = 43.0664
# /PowerSpectrum/pspk/mrs_string/inObsNames = rbin_0,ibin_0,rbin_1,ibin_1,rbin_2,ibin_2,rbin_3,ibin_3,rbin_4,ibin_4,rbin_5,ibin_5,rbin_6,ibin_6,rbin_7,ibin_7,rbin_8,ibin_8,rbin_9,ibin_9,rbin_10,ibin_10,rbin_11,ibin_11,rbin_12,ibin_12,rbin_13,ibin_13,rbin_14,ibin_14,rbin_15,ibin_15,rbin_16,ibin_16,rbin_17,ibin_17,rbin_18,ibin_18,rbin_19,ibin_19,rbin_20,ibin_20,rbin_21,ibin_21,rbin_22,ibin_22,rbin_23,ibin_23,rbin_24,ibin_24,rbin_25,ibin_25,rbin_26,ibin_26,rbin_27,ibin_27,rbin_28,ibin_28,rbin_29,ibin_29,rbin_30,ibin_30,rbin_31,ibin_31,rbin_32,ibin_32,rbin_33,ibin_33,rbin_34,ibin_34,rbin_35,ibin_35,rbin_36,ibin_36,rbin_37,ibin_37,rbin_38,ibin_38,rbin_39,ibin_39,rbin_40,ibin_40,rbin_41,ibin_41,rbin_42,ibin_42,rbin_43,ibin_43,rbin_44,ibin_44,rbin_45,ibin_45,rbin_46,ibin_46,rbin_47,ibin_47,rbin_48,ibin_48,rbin_49,ibin_49,rbin_50,ibin_50,rbin_51,ibin_51,rbin_52,ibin_52,rbin_53,ibin_53,rbin_54,ibin_54,rbin_55,ibin_55,rbin_56,ibin_56,rbin_57,ibin_57,rbin_58,ibin_58,rbin_59,ibin_59,rbin_60,ibin_60,rbin_61,ibin_61,rbin_62,ibin_62,rbin_63,ibin_63,rbin_64,ibin_64,rbin_65,ibin_65,rbin_66,ibin_66,rbin_67,ibin_67,rbin_68,ibin_68,rbin_69,ibin_69,rbin_70,ibin_70,rbin_71,ibin_71,rbin_72,ibin_72,rbin_73,ibin_73,rbin_74,ibin_74,rbin_75,ibin_75,rbin_76,ibin_76,rbin_77,ibin_77,rbin_78,ibin_78,rbin_79,ibin_79,rbin_80,ibin_80,rbin_81,ibin_81,rbin_82,ibin_82,rbin_83,ibin_83,rbin_84,ibin_84,rbin_85,ibin_85,rbin_86,ibin_86,rbin_87,ibin_87,rbin_88,ibin_88,rbin_89,ibin_89,rbin_90,ibin_90,rbin_91,ibin_91,rbin_92,ibin_92,rbin_93,ibin_93,rbin_94,ibin_94,rbin_95,ibin_95,rbin_96,ibin_96,rbin_97,ibin_97,rbin_98,ibin_98,rbin_99,ibin_99,rbin_100,ibin_100,rbin_101,ibin_101,rbin_102,ibin_102,rbin_103,ibin_103,rbin_104,ibin_104,rbin_105,ibin_105,rbin_106,ibin_106,rbin_107,ibin_107,rbin_108,ibin_108,rbin_109,ibin_109,rbin_110,ibin_110,rbin_111,ibin_111,rbin_112,ibin_112,rbin_113,ibin_113,rbin_114,ibin_114,rbin_115,ibin_115,rbin_116,ibin_116,rbin_117,ibin_117,rbin_118,ibin_118,rbin_119,ibin_119,rbin_120,ibin_120,rbin_121,ibin_121,rbin_122,ibin_122,rbin_123,ibin_123,rbin_124,ibin_124,rbin_125,ibin_125,rbin_126,ibin_126,rbin_127,ibin_127,rbin_128,ibin_128,rbin_129,ibin_129,rbin_130,ibin_130,rbin_131,ibin_131,rbin_132,ibin_132,rbin_133,ibin_133,rbin_134,ibin_134,rbin_135,ibin_135,rbin_136,ibin_136,rbin_137,ibin_137,rbin_138,ibin_138,rbin_139,ibin_139,rbin_140,ibin_140,rbin_141,ibin_141,rbin_142,ibin_142,rbin_143,ibin_143,rbin_144,ibin_144,rbin_145,ibin_145,rbin_146,ibin_146,rbin_147,ibin_147,rbin_148,ibin_148,rbin_149,ibin_149,rbin_150,ibin_150,rbin_151,ibin_151,rbin_152,ibin_152,rbin_153,ibin_153,rbin_154,ibin_154,rbin_155,ibin_155,rbin_156,ibin_156,rbin_157,ibin_157,rbin_158,ibin_158,rbin_159,ibin_159,rbin_160,ibin_160,rbin_161,ibin_161,rbin_162,ibin_162,rbin_163,ibin_163,rbin_164,ibin_164,rbin_165,ibin_165,rbin_166,ibin_166,rbin_167,ibin_167,rbin_168,ibin_168,rbin_169,ibin_169,rbin_170,ibin_170,rbin_171,ibin_171,rbin_172,ibin_172,rbin_173,ibin_173,rbin_174,ibin_174,rbin_175,ibin_175,rbin_176,ibin_176,rbin_177,ibin_177,rbin_178,ibin_178,rbin_179,ibin_179,rbin_180,ibin_180,rbin_181,ibin_181,rbin_182,ibin_182,rbin_183,ibin_183,rbin_184,ibin_184,rbin_185,ibin_185,rbin_186,ibin_186,rbin_187,ibin_187,rbin_188,ibin_188,rbin_189,ibin_189,rbin_190,ibin_190,rbin_191,ibin_191,rbin_192,ibin_192,rbin_193,ibin_193,rbin_194,ibin_194,rbin_195,ibin_195,rbin_196,ibin_196,rbin_197,ibin_197,rbin_198,ibin_198,rbin_199,ibin_199,rbin_200,ibin_200,rbin_201,ibin_201,rbin_202,ibin_202,rbin_203,ibin_203,rbin_204,ibin_204,rbin_205,ibin_205,rbin_206,ibin_206,rbin_207,ibin_207,rbin_208,ibin_208,rbin_209,ibin_209,rbin_210,ibin_210,rbin_211,ibin_211,rbin_212,ibin_212,rbin_213,ibin_213,rbin_214,ibin_214,rbin_215,ibin_215,rbin_216,ibin_216,rbin_217,ibin_217,rbin_218,ibin_218,rbin_219,ibin_219,rbin_220,ibin_220,rbin_221,ibin_221,rbin_222,ibin_222,rbin_223,ibin_223,rbin_224,ibin_224,rbin_225,ibin_225,rbin_226,ibin_226,rbin_227,ibin_227,rbin_228,ibin_228,rbin_229,ibin_229,rbin_230,ibin_230,rbin_231,ibin_231,rbin_232,ibin_232,rbin_233,ibin_233,rbin_234,ibin_234,rbin_235,ibin_235,rbin_236,ibin_236,rbin_237,ibin_237,rbin_238,ibin_238,rbin_239,ibin_239,rbin_240,ibin_240,rbin_241,ibin_241,rbin_242,ibin_242,rbin_243,ibin_243,rbin_244,ibin_244,rbin_245,ibin_245,rbin_246,ibin_246,rbin_247,ibin_247,rbin_248,ibin_248,rbin_249,ibin_249,rbin_250,ibin_250,rbin_251,ibin_251,rbin_252,ibin_252,rbin_253,ibin_253,rbin_254,ibin_254,rbin_255,ibin_255,
# /PowerSpectrum/pspk/mrs_string/onObsNames = mbin_0,mbin_1,mbin_2,mbin_3,mbin_4,mbin_5,mbin_6,mbin_7,mbin_8,mbin_9,mbin_10,mbin_11,mbin_12,mbin_13,mbin_14,mbin_15,mbin_16,mbin_17,mbin_18,mbin_19,mbin_20,mbin_21,mbin_22,mbin_23,mbin_24,mbin_25,mbin_26,mbin_27,mbin_28,mbin_29,mbin_30,mbin_31,mbin_32,mbin_33,mbin_34,mbin_35,mbin_36,mbin_37,mbin_38,mbin_39,mbin_40,mbin_41,mbin_42,mbin_43,mbin_44,mbin_45,mbin_46,mbin_47,mbin_48,mbin_49,mbin_50,mbin_51,mbin_52,mbin_53,mbin_54,mbin_55,mbin_56,mbin_57,mbin_58,mbin_59,mbin_60,mbin_61,mbin_62,mbin_63,mbin_64,mbin_65,mbin_66,mbin_67,mbin_68,mbin_69,mbin_70,mbin_71,mbin_72,mbin_73,mbin_74,mbin_75,mbin_76,mbin_77,mbin_78,mbin_79,mbin_80,mbin_81,mbin_82,mbin_83,mbin_84,mbin_85,mbin_86,mbin_87,mbin_88,mbin_89,mbin_90,mbin_91,mbin_92,mbin_93,mbin_94,mbin_95,mbin_96,mbin_97,mbin_98,mbin_99,mbin_100,mbin_101,mbin_102,mbin_103,mbin_104,mbin_105,mbin_106,mbin_107,mbin_108,mbin_109,mbin_110,mbin_111,mbin_112,mbin_113,mbin_114,mbin_115,mbin_116,mbin_117,mbin_118,mbin_119,mbin_120,mbin_121,mbin_122,mbin_123,mbin_124,mbin_125,mbin_126,mbin_127,mbin_128,mbin_129,mbin_130,mbin_131,mbin_132,mbin_133,mbin_134,mbin_135,mbin_136,mbin_137,mbin_138,mbin_139,mbin_140,mbin_141,mbin_142,mbin_143,mbin_144,mbin_145,mbin_146,mbin_147,mbin_148,mbin_149,mbin_150,mbin_151,mbin_152,mbin_153,mbin_154,mbin_155,mbin_156,mbin_157,mbin_158,mbin_159,mbin_160,mbin_161,mbin_162,mbin_163,mbin_164,mbin_165,mbin_166,mbin_167,mbin_168,mbin_169,mbin_170,mbin_171,mbin_172,mbin_173,mbin_174,mbin_175,mbin_176,mbin_177,mbin_178,mbin_179,mbin_180,mbin_181,mbin_182,mbin_183,mbin_184,mbin_185,mbin_186,mbin_187,mbin_188,mbin_189,mbin_190,mbin_191,mbin_192,mbin_193,mbin_194,mbin_195,mbin_196,mbin_197,mbin_198,mbin_199,mbin_200,mbin_201,mbin_202,mbin_203,mbin_204,mbin_205,mbin_206,mbin_207,mbin_208,mbin_209,mbin_210,mbin_211,mbin_212,mbin_213,mbin_214,mbin_215,mbin_216,mbin_217,mbin_218,mbin_219,mbin_220,mbin_221,mbin_222,mbin_223,mbin_224,mbin_225,mbin_226,mbin_227,mbin_228,mbin_229,mbin_230,mbin_231,mbin_232,mbin_233,mbin_234,mbin_235,mbin_236,mbin_237,mbin_238,mbin_239,mbin_240,mbin_241,mbin_242,mbin_243,mbin_244,mbin_245,mbin_246,mbin_247,mbin_248,mbin_249,mbin_250,mbin_251,mbin_252,mbin_253,mbin_254,mbin_255,
# /PowerSpectrum/pspk/mrs_string/spectrumType = power

# Number of links = 0

# MarSystemComposite
# Type = Fanout
# Name = spectrumFeatures

# MarControls = 13
# /Fanout/spectrumFeatures/mrs_bool/active = 1
# /Fanout/spectrumFeatures/mrs_bool/debug = 0
# /Fanout/spectrumFeatures/mrs_bool/mute = 0
# /Fanout/spectrumFeatures/mrs_bool/probe = 0
# /Fanout/spectrumFeatures/mrs_natural/disable = -1
# /Fanout/spectrumFeatures/mrs_natural/inObservations = 256
# /Fanout/spectrumFeatures/mrs_natural/inSamples = 1
# /Fanout/spectrumFeatures/mrs_natural/onObservations = 17
# /Fanout/spectrumFeatures/mrs_natural/onSamples = 1
# /Fanout/spectrumFeatures/mrs_real/israte = 43.0664
# /Fanout/spectrumFeatures/mrs_real/osrate = 43.0664
# /Fanout/spectrumFeatures/mrs_string/inObsNames = mbin_0,mbin_1,mbin_2,mbin_3,mbin_4,mbin_5,mbin_6,mbin_7,mbin_8,mbin_9,mbin_10,mbin_11,mbin_12,mbin_13,mbin_14,mbin_15,mbin_16,mbin_17,mbin_18,mbin_19,mbin_20,mbin_21,mbin_22,mbin_23,mbin_24,mbin_25,mbin_26,mbin_27,mbin_28,mbin_29,mbin_30,mbin_31,mbin_32,mbin_33,mbin_34,mbin_35,mbin_36,mbin_37,mbin_38,mbin_39,mbin_40,mbin_41,mbin_42,mbin_43,mbin_44,mbin_45,mbin_46,mbin_47,mbin_48,mbin_49,mbin_50,mbin_51,mbin_52,mbin_53,mbin_54,mbin_55,mbin_56,mbin_57,mbin_58,mbin_59,mbin_60,mbin_61,mbin_62,mbin_63,mbin_64,mbin_65,mbin_66,mbin_67,mbin_68,mbin_69,mbin_70,mbin_71,mbin_72,mbin_73,mbin_74,mbin_75,mbin_76,mbin_77,mbin_78,mbin_79,mbin_80,mbin_81,mbin_82,mbin_83,mbin_84,mbin_85,mbin_86,mbin_87,mbin_88,mbin_89,mbin_90,mbin_91,mbin_92,mbin_93,mbin_94,mbin_95,mbin_96,mbin_97,mbin_98,mbin_99,mbin_100,mbin_101,mbin_102,mbin_103,mbin_104,mbin_105,mbin_106,mbin_107,mbin_108,mbin_109,mbin_110,mbin_111,mbin_112,mbin_113,mbin_114,mbin_115,mbin_116,mbin_117,mbin_118,mbin_119,mbin_120,mbin_121,mbin_122,mbin_123,mbin_124,mbin_125,mbin_126,mbin_127,mbin_128,mbin_129,mbin_130,mbin_131,mbin_132,mbin_133,mbin_134,mbin_135,mbin_136,mbin_137,mbin_138,mbin_139,mbin_140,mbin_141,mbin_142,mbin_143,mbin_144,mbin_145,mbin_146,mbin_147,mbin_148,mbin_149,mbin_150,mbin_151,mbin_152,mbin_153,mbin_154,mbin_155,mbin_156,mbin_157,mbin_158,mbin_159,mbin_160,mbin_161,mbin_162,mbin_163,mbin_164,mbin_165,mbin_166,mbin_167,mbin_168,mbin_169,mbin_170,mbin_171,mbin_172,mbin_173,mbin_174,mbin_175,mbin_176,mbin_177,mbin_178,mbin_179,mbin_180,mbin_181,mbin_182,mbin_183,mbin_184,mbin_185,mbin_186,mbin_187,mbin_188,mbin_189,mbin_190,mbin_191,mbin_192,mbin_193,mbin_194,mbin_195,mbin_196,mbin_197,mbin_198,mbin_199,mbin_200,mbin_201,mbin_202,mbin_203,mbin_204,mbin_205,mbin_206,mbin_207,mbin_208,mbin_209,mbin_210,mbin_211,mbin_212,mbin_213,mbin_214,mbin_215,mbin_216,mbin_217,mbin_218,mbin_219,mbin_220,mbin_221,mbin_222,mbin_223,mbin_224,mbin_225,mbin_226,mbin_227,mbin_228,mbin_229,mbin_230,mbin_231,mbin_232,mbin_233,mbin_234,mbin_235,mbin_236,mbin_237,mbin_238,mbin_239,mbin_240,mbin_241,mbin_242,mbin_243,mbin_244,mbin_245,mbin_246,mbin_247,mbin_248,mbin_249,mbin_250,mbin_251,mbin_252,mbin_253,mbin_254,mbin_255,
# /Fanout/spectrumFeatures/mrs_string/onObsNames = Centroid,Kurtosis,Rolloff,Flux,MFCC_0,MFCC_1,MFCC_2,MFCC_3,MFCC_4,MFCC_5,MFCC_6,MFCC_7,MFCC_8,MFCC_9,MFCC_10,MFCC_11,MFCC_12,

# Number of links = 0

# nComponents = 5

# MarSystem
# Type = Centroid
# Name = cntrd

# MarControls = 11
# /Centroid/cntrd/mrs_bool/active = 1
# /Centroid/cntrd/mrs_bool/debug = 0
# /Centroid/cntrd/mrs_bool/mute = 0
# /Centroid/cntrd/mrs_natural/inObservations = 256
# /Centroid/cntrd/mrs_natural/inSamples = 1
# /Centroid/cntrd/mrs_natural/onObservations = 1
# /Centroid/cntrd/mrs_natural/onSamples = 1
# /Centroid/cntrd/mrs_real/israte = 43.0664
# /Centroid/cntrd/mrs_real/osrate = 43.0664
# /Centroid/cntrd/mrs_string/inObsNames = mbin_0,mbin_1,mbin_2,mbin_3,mbin_4,mbin_5,mbin_6,mbin_7,mbin_8,mbin_9,mbin_10,mbin_11,mbin_12,mbin_13,mbin_14,mbin_15,mbin_16,mbin_17,mbin_18,mbin_19,mbin_20,mbin_21,mbin_22,mbin_23,mbin_24,mbin_25,mbin_26,mbin_27,mbin_28,mbin_29,mbin_30,mbin_31,mbin_32,mbin_33,mbin_34,mbin_35,mbin_36,mbin_37,mbin_38,mbin_39,mbin_40,mbin_41,mbin_42,mbin_43,mbin_44,mbin_45,mbin_46,mbin_47,mbin_48,mbin_49,mbin_50,mbin_51,mbin_52,mbin_53,mbin_54,mbin_55,mbin_56,mbin_57,mbin_58,mbin_59,mbin_60,mbin_61,mbin_62,mbin_63,mbin_64,mbin_65,mbin_66,mbin_67,mbin_68,mbin_69,mbin_70,mbin_71,mbin_72,mbin_73,mbin_74,mbin_75,mbin_76,mbin_77,mbin_78,mbin_79,mbin_80,mbin_81,mbin_82,mbin_83,mbin_84,mbin_85,mbin_86,mbin_87,mbin_88,mbin_89,mbin_90,mbin_91,mbin_92,mbin_93,mbin_94,mbin_95,mbin_96,mbin_97,mbin_98,mbin_99,mbin_100,mbin_101,mbin_102,mbin_103,mbin_104,mbin_105,mbin_106,mbin_107,mbin_108,mbin_109,mbin_110,mbin_111,mbin_112,mbin_113,mbin_114,mbin_115,mbin_116,mbin_117,mbin_118,mbin_119,mbin_120,mbin_121,mbin_122,mbin_123,mbin_124,mbin_125,mbin_126,mbin_127,mbin_128,mbin_129,mbin_130,mbin_131,mbin_132,mbin_133,mbin_134,mbin_135,mbin_136,mbin_137,mbin_138,mbin_139,mbin_140,mbin_141,mbin_142,mbin_143,mbin_144,mbin_145,mbin_146,mbin_147,mbin_148,mbin_149,mbin_150,mbin_151,mbin_152,mbin_153,mbin_154,mbin_155,mbin_156,mbin_157,mbin_158,mbin_159,mbin_160,mbin_161,mbin_162,mbin_163,mbin_164,mbin_165,mbin_166,mbin_167,mbin_168,mbin_169,mbin_170,mbin_171,mbin_172,mbin_173,mbin_174,mbin_175,mbin_176,mbin_177,mbin_178,mbin_179,mbin_180,mbin_181,mbin_182,mbin_183,mbin_184,mbin_185,mbin_186,mbin_187,mbin_188,mbin_189,mbin_190,mbin_191,mbin_192,mbin_193,mbin_194,mbin_195,mbin_196,mbin_197,mbin_198,mbin_199,mbin_200,mbin_201,mbin_202,mbin_203,mbin_204,mbin_205,mbin_206,mbin_207,mbin_208,mbin_209,mbin_210,mbin_211,mbin_212,mbin_213,mbin_214,mbin_215,mbin_216,mbin_217,mbin_218,mbin_219,mbin_220,mbin_221,mbin_222,mbin_223,mbin_224,mbin_225,mbin_226,mbin_227,mbin_228,mbin_229,mbin_230,mbin_231,mbin_232,mbin_233,mbin_234,mbin_235,mbin_236,mbin_237,mbin_238,mbin_239,mbin_240,mbin_241,mbin_242,mbin_243,mbin_244,mbin_245,mbin_246,mbin_247,mbin_248,mbin_249,mbin_250,mbin_251,mbin_252,mbin_253,mbin_254,mbin_255,
# /Centroid/cntrd/mrs_string/onObsNames = Centroid,

# Number of links = 0

# MarSystem
# Type = Kurtosis
# Name = krt

# MarControls = 11
# /Kurtosis/krt/mrs_bool/active = 1
# /Kurtosis/krt/mrs_bool/debug = 0
# /Kurtosis/krt/mrs_bool/mute = 0
# /Kurtosis/krt/mrs_natural/inObservations = 256
# /Kurtosis/krt/mrs_natural/inSamples = 1
# /Kurtosis/krt/mrs_natural/onObservations = 1
# /Kurtosis/krt/mrs_natural/onSamples = 1
# /Kurtosis/krt/mrs_real/israte = 43.0664
# /Kurtosis/krt/mrs_real/osrate = 43.0664
# /Kurtosis/krt/mrs_string/inObsNames = mbin_0,mbin_1,mbin_2,mbin_3,mbin_4,mbin_5,mbin_6,mbin_7,mbin_8,mbin_9,mbin_10,mbin_11,mbin_12,mbin_13,mbin_14,mbin_15,mbin_16,mbin_17,mbin_18,mbin_19,mbin_20,mbin_21,mbin_22,mbin_23,mbin_24,mbin_25,mbin_26,mbin_27,mbin_28,mbin_29,mbin_30,mbin_31,mbin_32,mbin_33,mbin_34,mbin_35,mbin_36,mbin_37,mbin_38,mbin_39,mbin_40,mbin_41,mbin_42,mbin_43,mbin_44,mbin_45,mbin_46,mbin_47,mbin_48,mbin_49,mbin_50,mbin_51,mbin_52,mbin_53,mbin_54,mbin_55,mbin_56,mbin_57,mbin_58,mbin_59,mbin_60,mbin_61,mbin_62,mbin_63,mbin_64,mbin_65,mbin_66,mbin_67,mbin_68,mbin_69,mbin_70,mbin_71,mbin_72,mbin_73,mbin_74,mbin_75,mbin_76,mbin_77,mbin_78,mbin_79,mbin_80,mbin_81,mbin_82,mbin_83,mbin_84,mbin_85,mbin_86,mbin_87,mbin_88,mbin_89,mbin_90,mbin_91,mbin_92,mbin_93,mbin_94,mbin_95,mbin_96,mbin_97,mbin_98,mbin_99,mbin_100,mbin_101,mbin_102,mbin_103,mbin_104,mbin_105,mbin_106,mbin_107,mbin_108,mbin_109,mbin_110,mbin_111,mbin_112,mbin_113,mbin_114,mbin_115,mbin_116,mbin_117,mbin_118,mbin_119,mbin_120,mbin_121,mbin_122,mbin_123,mbin_124,mbin_125,mbin_126,mbin_127,mbin_128,mbin_129,mbin_130,mbin_131,mbin_132,mbin_133,mbin_134,mbin_135,mbin_136,mbin_137,mbin_138,mbin_139,mbin_140,mbin_141,mbin_142,mbin_143,mbin_144,mbin_145,mbin_146,mbin_147,mbin_148,mbin_149,mbin_150,mbin_151,mbin_152,mbin_153,mbin_154,mbin_155,mbin_156,mbin_157,mbin_158,mbin_159,mbin_160,mbin_161,mbin_162,mbin_163,mbin_164,mbin_165,mbin_166,mbin_167,mbin_168,mbin_169,mbin_170,mbin_171,mbin_172,mbin_173,mbin_174,mbin_175,mbin_176,mbin_177,mbin_178,mbin_179,mbin_180,mbin_181,mbin_182,mbin_183,mbin_184,mbin_185,mbin_186,mbin_187,mbin_188,mbin_189,mbin_190,mbin_191,mbin_192,mbin_193,mbin_194,mbin_195,mbin_196,mbin_197,mbin_198,mbin_199,mbin_200,mbin_201,mbin_202,mbin_203,mbin_204,mbin_205,mbin_206,mbin_207,mbin_208,mbin_209,mbin_210,mbin_211,mbin_212,mbin_213,mbin_214,mbin_215,mbin_216,mbin_217,mbin_218,mbin_219,mbin_220,mbin_221,mbin_222,mbin_223,mbin_224,mbin_225,mbin_226,mbin_227,mbin_228,mbin_229,mbin_230,mbin_231,mbin_232,mbin_233,mbin_234,mbin_235,mbin_236,mbin_237,mbin_238,mbin_239,mbin_240,mbin_241,mbin_242,mbin_243,mbin_244,mbin_245,mbin_246,mbin_247,mbin_248,mbin_249,mbin_250,mbin_251,mbin_252,mbin_253,mbin_254,mbin_255,
# /Kurtosis/krt/mrs_string/onObsNames = Kurtosis,

# Number of links = 0

# MarSystem
# Type = Rolloff
# Name = rlf

# MarControls = 12
# /Rolloff/rlf/mrs_bool/active = 1
# /Rolloff/rlf/mrs_bool/debug = 0
# /Rolloff/rlf/mrs_bool/mute = 0
# /Rolloff/rlf/mrs_natural/inObservations = 256
# /Rolloff/rlf/mrs_natural/inSamples = 1
# /Rolloff/rlf/mrs_natural/onObservations = 1
# /Rolloff/rlf/mrs_natural/onSamples = 1
# /Rolloff/rlf/mrs_real/israte = 43.0664
# /Rolloff/rlf/mrs_real/osrate = 43.0664
# /Rolloff/rlf/mrs_real/percentage = 0.9
# /Rolloff/rlf/mrs_string/inObsNames = mbin_0,mbin_1,mbin_2,mbin_3,mbin_4,mbin_5,mbin_6,mbin_7,mbin_8,mbin_9,mbin_10,mbin_11,mbin_12,mbin_13,mbin_14,mbin_15,mbin_16,mbin_17,mbin_18,mbin_19,mbin_20,mbin_21,mbin_22,mbin_23,mbin_24,mbin_25,mbin_26,mbin_27,mbin_28,mbin_29,mbin_30,mbin_31,mbin_32,mbin_33,mbin_34,mbin_35,mbin_36,mbin_37,mbin_38,mbin_39,mbin_40,mbin_41,mbin_42,mbin_43,mbin_44,mbin_45,mbin_46,mbin_47,mbin_48,mbin_49,mbin_50,mbin_51,mbin_52,mbin_53,mbin_54,mbin_55,mbin_56,mbin_57,mbin_58,mbin_59,mbin_60,mbin_61,mbin_62,mbin_63,mbin_64,mbin_65,mbin_66,mbin_67,mbin_68,mbin_69,mbin_70,mbin_71,mbin_72,mbin_73,mbin_74,mbin_75,mbin_76,mbin_77,mbin_78,mbin_79,mbin_80,mbin_81,mbin_82,mbin_83,mbin_84,mbin_85,mbin_86,mbin_87,mbin_88,mbin_89,mbin_90,mbin_91,mbin_92,mbin_93,mbin_94,mbin_95,mbin_96,mbin_97,mbin_98,mbin_99,mbin_100,mbin_101,mbin_102,mbin_103,mbin_104,mbin_105,mbin_106,mbin_107,mbin_108,mbin_109,mbin_110,mbin_111,mbin_112,mbin_113,mbin_114,mbin_115,mbin_116,mbin_117,mbin_118,mbin_119,mbin_120,mbin_121,mbin_122,mbin_123,mbin_124,mbin_125,mbin_126,mbin_127,mbin_128,mbin_129,mbin_130,mbin_131,mbin_132,mbin_133,mbin_134,mbin_135,mbin_136,mbin_137,mbin_138,mbin_139,mbin_140,mbin_141,mbin_142,mbin_143,mbin_144,mbin_145,mbin_146,mbin_147,mbin_148,mbin_149,mbin_150,mbin_151,mbin_152,mbin_153,mbin_154,mbin_155,mbin_156,mbin_157,mbin_158,mbin_159,mbin_160,mbin_161,mbin_162,mbin_163,mbin_164,mbin_165,mbin_166,mbin_167,mbin_168,mbin_169,mbin_170,mbin_171,mbin_172,mbin_173,mbin_174,mbin_175,mbin_176,mbin_177,mbin_178,mbin_179,mbin_180,mbin_181,mbin_182,mbin_183,mbin_184,mbin_185,mbin_186,mbin_187,mbin_188,mbin_189,mbin_190,mbin_191,mbin_192,mbin_193,mbin_194,mbin_195,mbin_196,mbin_197,mbin_198,mbin_199,mbin_200,mbin_201,mbin_202,mbin_203,mbin_204,mbin_205,mbin_206,mbin_207,mbin_208,mbin_209,mbin_210,mbin_211,mbin_212,mbin_213,mbin_214,mbin_215,mbin_216,mbin_217,mbin_218,mbin_219,mbin_220,mbin_221,mbin_222,mbin_223,mbin_224,mbin_225,mbin_226,mbin_227,mbin_228,mbin_229,mbin_230,mbin_231,mbin_232,mbin_233,mbin_234,mbin_235,mbin_236,mbin_237,mbin_238,mbin_239,mbin_240,mbin_241,mbin_242,mbin_243,mbin_244,mbin_245,mbin_246,mbin_247,mbin_248,mbin_249,mbin_250,mbin_251,mbin_252,mbin_253,mbin_254,mbin_255,
# /Rolloff/rlf/mrs_string/onObsNames = Rolloff,

# Number of links = 0

# MarSystem
# Type = Flux
# Name = flux

# MarControls = 11
# /Flux/flux/mrs_bool/active = 1
# /Flux/flux/mrs_bool/debug = 0
# /Flux/flux/mrs_bool/mute = 0
# /Flux/flux/mrs_natural/inObservations = 256
# /Flux/flux/mrs_natural/inSamples = 1
# /Flux/flux/mrs_natural/onObservations = 1
# /Flux/flux/mrs_natural/onSamples = 1
# /Flux/flux/mrs_real/israte = 43.0664
# /Flux/flux/mrs_real/osrate = 43.0664
# /Flux/flux/mrs_string/inObsNames = mbin_0,mbin_1,mbin_2,mbin_3,mbin_4,mbin_5,mbin_6,mbin_7,mbin_8,mbin_9,mbin_10,mbin_11,mbin_12,mbin_13,mbin_14,mbin_15,mbin_16,mbin_17,mbin_18,mbin_19,mbin_20,mbin_21,mbin_22,mbin_23,mbin_24,mbin_25,mbin_26,mbin_27,mbin_28,mbin_29,mbin_30,mbin_31,mbin_32,mbin_33,mbin_34,mbin_35,mbin_36,mbin_37,mbin_38,mbin_39,mbin_40,mbin_41,mbin_42,mbin_43,mbin_44,mbin_45,mbin_46,mbin_47,mbin_48,mbin_49,mbin_50,mbin_51,mbin_52,mbin_53,mbin_54,mbin_55,mbin_56,mbin_57,mbin_58,mbin_59,mbin_60,mbin_61,mbin_62,mbin_63,mbin_64,mbin_65,mbin_66,mbin_67,mbin_68,mbin_69,mbin_70,mbin_71,mbin_72,mbin_73,mbin_74,mbin_75,mbin_76,mbin_77,mbin_78,mbin_79,mbin_80,mbin_81,mbin_82,mbin_83,mbin_84,mbin_85,mbin_86,mbin_87,mbin_88,mbin_89,mbin_90,mbin_91,mbin_92,mbin_93,mbin_94,mbin_95,mbin_96,mbin_97,mbin_98,mbin_99,mbin_100,mbin_101,mbin_102,mbin_103,mbin_104,mbin_105,mbin_106,mbin_107,mbin_108,mbin_109,mbin_110,mbin_111,mbin_112,mbin_113,mbin_114,mbin_115,mbin_116,mbin_117,mbin_118,mbin_119,mbin_120,mbin_121,mbin_122,mbin_123,mbin_124,mbin_125,mbin_126,mbin_127,mbin_128,mbin_129,mbin_130,mbin_131,mbin_132,mbin_133,mbin_134,mbin_135,mbin_136,mbin_137,mbin_138,mbin_139,mbin_140,mbin_141,mbin_142,mbin_143,mbin_144,mbin_145,mbin_146,mbin_147,mbin_148,mbin_149,mbin_150,mbin_151,mbin_152,mbin_153,mbin_154,mbin_155,mbin_156,mbin_157,mbin_158,mbin_159,mbin_160,mbin_161,mbin_162,mbin_163,mbin_164,mbin_165,mbin_166,mbin_167,mbin_168,mbin_169,mbin_170,mbin_171,mbin_172,mbin_173,mbin_174,mbin_175,mbin_176,mbin_177,mbin_178,mbin_179,mbin_180,mbin_181,mbin_182,mbin_183,mbin_184,mbin_185,mbin_186,mbin_187,mbin_188,mbin_189,mbin_190,mbin_191,mbin_192,mbin_193,mbin_194,mbin_195,mbin_196,mbin_197,mbin_198,mbin_199,mbin_200,mbin_201,mbin_202,mbin_203,mbin_204,mbin_205,mbin_206,mbin_207,mbin_208,mbin_209,mbin_210,mbin_211,mbin_212,mbin_213,mbin_214,mbin_215,mbin_216,mbin_217,mbin_218,mbin_219,mbin_220,mbin_221,mbin_222,mbin_223,mbin_224,mbin_225,mbin_226,mbin_227,mbin_228,mbin_229,mbin_230,mbin_231,mbin_232,mbin_233,mbin_234,mbin_235,mbin_236,mbin_237,mbin_238,mbin_239,mbin_240,mbin_241,mbin_242,mbin_243,mbin_244,mbin_245,mbin_246,mbin_247,mbin_248,mbin_249,mbin_250,mbin_251,mbin_252,mbin_253,mbin_254,mbin_255,
# /Flux/flux/mrs_string/onObsNames = Flux,

# Number of links = 0

# MarSystem
# Type = MFCC
# Name = mfcc

# MarControls = 11
# /MFCC/mfcc/mrs_bool/active = 1
# /MFCC/mfcc/mrs_bool/debug = 0
# /MFCC/mfcc/mrs_bool/mute = 0
# /MFCC/mfcc/mrs_natural/inObservations = 256
# /MFCC/mfcc/mrs_natural/inSamples = 1
# /MFCC/mfcc/mrs_natural/onObservations = 13
# /MFCC/mfcc/mrs_natural/onSamples = 1
# /MFCC/mfcc/mrs_real/israte = 43.0664
# /MFCC/mfcc/mrs_real/osrate = 43.0664
# /MFCC/mfcc/mrs_string/inObsNames = mbin_0,mbin_1,mbin_2,mbin_3,mbin_4,mbin_5,mbin_6,mbin_7,mbin_8,mbin_9,mbin_10,mbin_11,mbin_12,mbin_13,mbin_14,mbin_15,mbin_16,mbin_17,mbin_18,mbin_19,mbin_20,mbin_21,mbin_22,mbin_23,mbin_24,mbin_25,mbin_26,mbin_27,mbin_28,mbin_29,mbin_30,mbin_31,mbin_32,mbin_33,mbin_34,mbin_35,mbin_36,mbin_37,mbin_38,mbin_39,mbin_40,mbin_41,mbin_42,mbin_43,mbin_44,mbin_45,mbin_46,mbin_47,mbin_48,mbin_49,mbin_50,mbin_51,mbin_52,mbin_53,mbin_54,mbin_55,mbin_56,mbin_57,mbin_58,mbin_59,mbin_60,mbin_61,mbin_62,mbin_63,mbin_64,mbin_65,mbin_66,mbin_67,mbin_68,mbin_69,mbin_70,mbin_71,mbin_72,mbin_73,mbin_74,mbin_75,mbin_76,mbin_77,mbin_78,mbin_79,mbin_80,mbin_81,mbin_82,mbin_83,mbin_84,mbin_85,mbin_86,mbin_87,mbin_88,mbin_89,mbin_90,mbin_91,mbin_92,mbin_93,mbin_94,mbin_95,mbin_96,mbin_97,mbin_98,mbin_99,mbin_100,mbin_101,mbin_102,mbin_103,mbin_104,mbin_105,mbin_106,mbin_107,mbin_108,mbin_109,mbin_110,mbin_111,mbin_112,mbin_113,mbin_114,mbin_115,mbin_116,mbin_117,mbin_118,mbin_119,mbin_120,mbin_121,mbin_122,mbin_123,mbin_124,mbin_125,mbin_126,mbin_127,mbin_128,mbin_129,mbin_130,mbin_131,mbin_132,mbin_133,mbin_134,mbin_135,mbin_136,mbin_137,mbin_138,mbin_139,mbin_140,mbin_141,mbin_142,mbin_143,mbin_144,mbin_145,mbin_146,mbin_147,mbin_148,mbin_149,mbin_150,mbin_151,mbin_152,mbin_153,mbin_154,mbin_155,mbin_156,mbin_157,mbin_158,mbin_159,mbin_160,mbin_161,mbin_162,mbin_163,mbin_164,mbin_165,mbin_166,mbin_167,mbin_168,mbin_169,mbin_170,mbin_171,mbin_172,mbin_173,mbin_174,mbin_175,mbin_176,mbin_177,mbin_178,mbin_179,mbin_180,mbin_181,mbin_182,mbin_183,mbin_184,mbin_185,mbin_186,mbin_187,mbin_188,mbin_189,mbin_190,mbin_191,mbin_192,mbin_193,mbin_194,mbin_195,mbin_196,mbin_197,mbin_198,mbin_199,mbin_200,mbin_201,mbin_202,mbin_203,mbin_204,mbin_205,mbin_206,mbin_207,mbin_208,mbin_209,mbin_210,mbin_211,mbin_212,mbin_213,mbin_214,mbin_215,mbin_216,mbin_217,mbin_218,mbin_219,mbin_220,mbin_221,mbin_222,mbin_223,mbin_224,mbin_225,mbin_226,mbin_227,mbin_228,mbin_229,mbin_230,mbin_231,mbin_232,mbin_233,mbin_234,mbin_235,mbin_236,mbin_237,mbin_238,mbin_239,mbin_240,mbin_241,mbin_242,mbin_243,mbin_244,mbin_245,mbin_246,mbin_247,mbin_248,mbin_249,mbin_250,mbin_251,mbin_252,mbin_253,mbin_254,mbin_255,
# /MFCC/mfcc/mrs_string/onObsNames = MFCC_0,MFCC_1,MFCC_2,MFCC_3,MFCC_4,MFCC_5,MFCC_6,MFCC_7,MFCC_8,MFCC_9,MFCC_10,MFCC_11,MFCC_12,

# Number of links = 0




# MarSystem
# Type = Memory
# Name = memory

# MarControls = 13
# /Memory/memory/mrs_bool/active = 1
# /Memory/memory/mrs_bool/debug = 0
# /Memory/memory/mrs_bool/mute = 0
# /Memory/memory/mrs_bool/reset = 0
# /Memory/memory/mrs_natural/inObservations = 17
# /Memory/memory/mrs_natural/inSamples = 1
# /Memory/memory/mrs_natural/memSize = 40
# /Memory/memory/mrs_natural/onObservations = 17
# /Memory/memory/mrs_natural/onSamples = 40
# /Memory/memory/mrs_real/israte = 43.0664
# /Memory/memory/mrs_real/osrate = 43.0664
# /Memory/memory/mrs_string/inObsNames = Centroid,Kurtosis,Rolloff,Flux,MFCC_0,MFCC_1,MFCC_2,MFCC_3,MFCC_4,MFCC_5,MFCC_6,MFCC_7,MFCC_8,MFCC_9,MFCC_10,MFCC_11,MFCC_12,
# /Memory/memory/mrs_string/onObsNames = Mem40_Centroid,Mem40_Kurtosis,Mem40_Rolloff,Mem40_Flux,Mem40_MFCC_0,Mem40_MFCC_1,Mem40_MFCC_2,Mem40_MFCC_3,Mem40_MFCC_4,Mem40_MFCC_5,Mem40_MFCC_6,Mem40_MFCC_7,Mem40_MFCC_8,Mem40_MFCC_9,Mem40_MFCC_10,Mem40_MFCC_11,Mem40_MFCC_12,

# Number of links = 0

# MarSystemComposite
# Type = Fanout
# Name = statistics

# MarControls = 13
# /Fanout/statistics/mrs_bool/active = 1
# /Fanout/statistics/mrs_bool/debug = 0
# /Fanout/statistics/mrs_bool/mute = 0
# /Fanout/statistics/mrs_bool/probe = 0
# /Fanout/statistics/mrs_natural/disable = -1
# /Fanout/statistics/mrs_natural/inObservations = 17
# /Fanout/statistics/mrs_natural/inSamples = 40
# /Fanout/statistics/mrs_natural/onObservations = 34
# /Fanout/statistics/mrs_natural/onSamples = 1
# /Fanout/statistics/mrs_real/israte = 43.0664
# /Fanout/statistics/mrs_real/osrate = 43.0664
# /Fanout/statistics/mrs_string/inObsNames = Mem40_Centroid,Mem40_Kurtosis,Mem40_Rolloff,Mem40_Flux,Mem40_MFCC_0,Mem40_MFCC_1,Mem40_MFCC_2,Mem40_MFCC_3,Mem40_MFCC_4,Mem40_MFCC_5,Mem40_MFCC_6,Mem40_MFCC_7,Mem40_MFCC_8,Mem40_MFCC_9,Mem40_MFCC_10,Mem40_MFCC_11,Mem40_MFCC_12,
# /Fanout/statistics/mrs_string/onObsNames = Mean_Mem40_Centroid,Mean_Mem40_Kurtosis,Mean_Mem40_Rolloff,Mean_Mem40_Flux,Mean_Mem40_MFCC_0,Mean_Mem40_MFCC_1,Mean_Mem40_MFCC_2,Mean_Mem40_MFCC_3,Mean_Mem40_MFCC_4,Mean_Mem40_MFCC_5,Mean_Mem40_MFCC_6,Mean_Mem40_MFCC_7,Mean_Mem40_MFCC_8,Mean_Mem40_MFCC_9,Mean_Mem40_MFCC_10,Mean_Mem40_MFCC_11,Mean_Mem40_MFCC_12,Std_Mem40_Centroid,Std_Mem40_Kurtosis,Std_Mem40_Rolloff,Std_Mem40_Flux,Std_Mem40_MFCC_0,Std_Mem40_MFCC_1,Std_Mem40_MFCC_2,Std_Mem40_MFCC_3,Std_Mem40_MFCC_4,Std_Mem40_MFCC_5,Std_Mem40_MFCC_6,Std_Mem40_MFCC_7,Std_Mem40_MFCC_8,Std_Mem40_MFCC_9,Std_Mem40_MFCC_10,Std_Mem40_MFCC_11,Std_Mem40_MFCC_12,

# Number of links = 0

# nComponents = 2

# MarSystem
# Type = Mean
# Name = mn

# MarControls = 11
# /Mean/mn/mrs_bool/active = 1
# /Mean/mn/mrs_bool/debug = 0
# /Mean/mn/mrs_bool/mute = 0
# /Mean/mn/mrs_natural/inObservations = 17
# /Mean/mn/mrs_natural/inSamples = 40
# /Mean/mn/mrs_natural/onObservations = 17
# /Mean/mn/mrs_natural/onSamples = 1
# /Mean/mn/mrs_real/israte = 43.0664
# /Mean/mn/mrs_real/osrate = 43.0664
# /Mean/mn/mrs_string/inObsNames = Mem40_Centroid,Mem40_Kurtosis,Mem40_Rolloff,Mem40_Flux,Mem40_MFCC_0,Mem40_MFCC_1,Mem40_MFCC_2,Mem40_MFCC_3,Mem40_MFCC_4,Mem40_MFCC_5,Mem40_MFCC_6,Mem40_MFCC_7,Mem40_MFCC_8,Mem40_MFCC_9,Mem40_MFCC_10,Mem40_MFCC_11,Mem40_MFCC_12,
# /Mean/mn/mrs_string/onObsNames = Mean_Mem40_Centroid,Mean_Mem40_Kurtosis,Mean_Mem40_Rolloff,Mean_Mem40_Flux,Mean_Mem40_MFCC_0,Mean_Mem40_MFCC_1,Mean_Mem40_MFCC_2,Mean_Mem40_MFCC_3,Mean_Mem40_MFCC_4,Mean_Mem40_MFCC_5,Mean_Mem40_MFCC_6,Mean_Mem40_MFCC_7,Mean_Mem40_MFCC_8,Mean_Mem40_MFCC_9,Mean_Mem40_MFCC_10,Mean_Mem40_MFCC_11,Mean_Mem40_MFCC_12,

# Number of links = 0

# MarSystem
# Type = StandardDeviation
# Name = std

# MarControls = 11
# /StandardDeviation/std/mrs_bool/active = 1
# /StandardDeviation/std/mrs_bool/debug = 0
# /StandardDeviation/std/mrs_bool/mute = 0
# /StandardDeviation/std/mrs_natural/inObservations = 17
# /StandardDeviation/std/mrs_natural/inSamples = 40
# /StandardDeviation/std/mrs_natural/onObservations = 17
# /StandardDeviation/std/mrs_natural/onSamples = 1
# /StandardDeviation/std/mrs_real/israte = 43.0664
# /StandardDeviation/std/mrs_real/osrate = 43.0664
# /StandardDeviation/std/mrs_string/inObsNames = Mem40_Centroid,Mem40_Kurtosis,Mem40_Rolloff,Mem40_Flux,Mem40_MFCC_0,Mem40_MFCC_1,Mem40_MFCC_2,Mem40_MFCC_3,Mem40_MFCC_4,Mem40_MFCC_5,Mem40_MFCC_6,Mem40_MFCC_7,Mem40_MFCC_8,Mem40_MFCC_9,Mem40_MFCC_10,Mem40_MFCC_11,Mem40_MFCC_12,
# /StandardDeviation/std/mrs_string/onObsNames = Std_Mem40_Centroid,Std_Mem40_Kurtosis,Std_Mem40_Rolloff,Std_Mem40_Flux,Std_Mem40_MFCC_0,Std_Mem40_MFCC_1,Std_Mem40_MFCC_2,Std_Mem40_MFCC_3,Std_Mem40_MFCC_4,Std_Mem40_MFCC_5,Std_Mem40_MFCC_6,Std_Mem40_MFCC_7,Std_Mem40_MFCC_8,Std_Mem40_MFCC_9,Std_Mem40_MFCC_10,Std_Mem40_MFCC_11,Std_Mem40_MFCC_12,

# Number of links = 0


# MarSystem
# Type = Annotator
# Name = annotator

# MarControls = 12
# /Annotator/annotator/mrs_bool/active = 1
# /Annotator/annotator/mrs_bool/debug = 0
# /Annotator/annotator/mrs_bool/mute = 0
# /Annotator/annotator/mrs_natural/inObservations = 34
# /Annotator/annotator/mrs_natural/inSamples = 1
# /Annotator/annotator/mrs_natural/label = 1
# /Annotator/annotator/mrs_natural/onObservations = 35
# /Annotator/annotator/mrs_natural/onSamples = 1
# /Annotator/annotator/mrs_real/israte = 43.0664
# /Annotator/annotator/mrs_real/osrate = 43.0664
# /Annotator/annotator/mrs_string/inObsNames = Mean_Mem40_Centroid,Mean_Mem40_Kurtosis,Mean_Mem40_Rolloff,Mean_Mem40_Flux,Mean_Mem40_MFCC_0,Mean_Mem40_MFCC_1,Mean_Mem40_MFCC_2,Mean_Mem40_MFCC_3,Mean_Mem40_MFCC_4,Mean_Mem40_MFCC_5,Mean_Mem40_MFCC_6,Mean_Mem40_MFCC_7,Mean_Mem40_MFCC_8,Mean_Mem40_MFCC_9,Mean_Mem40_MFCC_10,Mean_Mem40_MFCC_11,Mean_Mem40_MFCC_12,Std_Mem40_Centroid,Std_Mem40_Kurtosis,Std_Mem40_Rolloff,Std_Mem40_Flux,Std_Mem40_MFCC_0,Std_Mem40_MFCC_1,Std_Mem40_MFCC_2,Std_Mem40_MFCC_3,Std_Mem40_MFCC_4,Std_Mem40_MFCC_5,Std_Mem40_MFCC_6,Std_Mem40_MFCC_7,Std_Mem40_MFCC_8,Std_Mem40_MFCC_9,Std_Mem40_MFCC_10,Std_Mem40_MFCC_11,Std_Mem40_MFCC_12,
# /Annotator/annotator/mrs_string/onObsNames = Mean_Mem40_Centroid,Mean_Mem40_Kurtosis,Mean_Mem40_Rolloff,Mean_Mem40_Flux,Mean_Mem40_MFCC_0,Mean_Mem40_MFCC_1,Mean_Mem40_MFCC_2,Mean_Mem40_MFCC_3,Mean_Mem40_MFCC_4,Mean_Mem40_MFCC_5,Mean_Mem40_MFCC_6,Mean_Mem40_MFCC_7,Mean_Mem40_MFCC_8,Mean_Mem40_MFCC_9,Mean_Mem40_MFCC_10,Mean_Mem40_MFCC_11,Mean_Mem40_MFCC_12,Std_Mem40_Centroid,Std_Mem40_Kurtosis,Std_Mem40_Rolloff,Std_Mem40_Flux,Std_Mem40_MFCC_0,Std_Mem40_MFCC_1,Std_Mem40_MFCC_2,Std_Mem40_MFCC_3,Std_Mem40_MFCC_4,Std_Mem40_MFCC_5,Std_Mem40_MFCC_6,Std_Mem40_MFCC_7,Std_Mem40_MFCC_8,Std_Mem40_MFCC_9,Std_Mem40_MFCC_10,Std_Mem40_MFCC_11,Std_Mem40_MFCC_12,

# Number of links = 0

# MarSystem
# Type = WekaSink
# Name = wsink

# MarControls = 16
# /WekaSink/wsink/mrs_bool/active = 1
# /WekaSink/wsink/mrs_bool/debug = 0
# /WekaSink/wsink/mrs_bool/mute = 1
# /WekaSink/wsink/mrs_natural/downsample = 40
# /WekaSink/wsink/mrs_natural/inObservations = 35
# /WekaSink/wsink/mrs_natural/inSamples = 1
# /WekaSink/wsink/mrs_natural/nLabels = 2
# /WekaSink/wsink/mrs_natural/onObservations = 35
# /WekaSink/wsink/mrs_natural/onSamples = 1
# /WekaSink/wsink/mrs_natural/precision = 6
# /WekaSink/wsink/mrs_real/israte = 43.0664
# /WekaSink/wsink/mrs_real/osrate = 43.0664
# /WekaSink/wsink/mrs_string/filename = weka.arff
# /WekaSink/wsink/mrs_string/inObsNames = Mean_Mem40_Centroid,Mean_Mem40_Kurtosis,Mean_Mem40_Rolloff,Mean_Mem40_Flux,Mean_Mem40_MFCC_0,Mean_Mem40_MFCC_1,Mean_Mem40_MFCC_2,Mean_Mem40_MFCC_3,Mean_Mem40_MFCC_4,Mean_Mem40_MFCC_5,Mean_Mem40_MFCC_6,Mean_Mem40_MFCC_7,Mean_Mem40_MFCC_8,Mean_Mem40_MFCC_9,Mean_Mem40_MFCC_10,Mean_Mem40_MFCC_11,Mean_Mem40_MFCC_12,Std_Mem40_Centroid,Std_Mem40_Kurtosis,Std_Mem40_Rolloff,Std_Mem40_Flux,Std_Mem40_MFCC_0,Std_Mem40_MFCC_1,Std_Mem40_MFCC_2,Std_Mem40_MFCC_3,Std_Mem40_MFCC_4,Std_Mem40_MFCC_5,Std_Mem40_MFCC_6,Std_Mem40_MFCC_7,Std_Mem40_MFCC_8,Std_Mem40_MFCC_9,Std_Mem40_MFCC_10,Std_Mem40_MFCC_11,Std_Mem40_MFCC_12,
# /WekaSink/wsink/mrs_string/labelNames = music,speech,
# /WekaSink/wsink/mrs_string/onObsNames = Mean_Mem40_Centroid,Mean_Mem40_Kurtosis,Mean_Mem40_Rolloff,Mean_Mem40_Flux,Mean_Mem40_MFCC_0,Mean_Mem40_MFCC_1,Mean_Mem40_MFCC_2,Mean_Mem40_MFCC_3,Mean_Mem40_MFCC_4,Mean_Mem40_MFCC_5,Mean_Mem40_MFCC_6,Mean_Mem40_MFCC_7,Mean_Mem40_MFCC_8,Mean_Mem40_MFCC_9,Mean_Mem40_MFCC_10,Mean_Mem40_MFCC_11,Mean_Mem40_MFCC_12,Std_Mem40_Centroid,Std_Mem40_Kurtosis,Std_Mem40_Rolloff,Std_Mem40_Flux,Std_Mem40_MFCC_0,Std_Mem40_MFCC_1,Std_Mem40_MFCC_2,Std_Mem40_MFCC_3,Std_Mem40_MFCC_4,Std_Mem40_MFCC_5,Std_Mem40_MFCC_6,Std_Mem40_MFCC_7,Std_Mem40_MFCC_8,Std_Mem40_MFCC_9,Std_Mem40_MFCC_10,Std_Mem40_MFCC_11,Std_Mem40_MFCC_12,

# Number of links = 0

# MarSystem
# Type = GaussianClassifier
# Name = gaussian

# MarControls = 16
# /GaussianClassifier/gaussian/mrs_bool/active = 1
# /GaussianClassifier/gaussian/mrs_bool/debug = 0
# /GaussianClassifier/gaussian/mrs_bool/done = 0
# /GaussianClassifier/gaussian/mrs_bool/mute = 0
# /GaussianClassifier/gaussian/mrs_natural/inObservations = 35
# /GaussianClassifier/gaussian/mrs_natural/inSamples = 1
# /GaussianClassifier/gaussian/mrs_natural/nLabels = 2
# /GaussianClassifier/gaussian/mrs_natural/onObservations = 2
# /GaussianClassifier/gaussian/mrs_natural/onSamples = 1
# /GaussianClassifier/gaussian/mrs_real/israte = 43.0664
# /GaussianClassifier/gaussian/mrs_real/osrate = 43.0664
# /GaussianClassifier/gaussian/mrs_realvec/covars = # MARSYAS mrs_realvec
# Size = 70


# type: matrix
# rows: 2
# columns: 35
672.465 1.47844e-08 89.3536 280622 0.023434 0.232253 0.513474 0.930575 1.87475 3.13977 3.59707 3.29198 4.22421 4.22828 4.65422 4.00283 5.17756 2626.81 1.34454e-07 105.216 770582 0.186219 4.96398 11.9005 19.2427 34.5306 50.6671 56.7696 49.6571 42.83 41.5965 37.4944 37.7197 35.686 0 
548.521 1.50495e-08 56.4738 247656 0.0283223 0.466129 0.664828 1.22268 3.11712 3.43149 3.91019 5.90307 7.67176 8.4053 11.4916 7.93621 15.9553 472.536 1.27875e-07 79.5833 310570 0.116638 1.40734 5.24602 5.30676 9.4811 23.2459 22.3676 29.6922 45.1464 50.3509 57.1407 61.8475 59.6692 0 

# Size = 70
# MARSYAS mrs_realvec

# /GaussianClassifier/gaussian/mrs_realvec/means = # MARSYAS mrs_realvec
# Size = 70


# type: matrix
# rows: 2
# columns: 35
0.0637268 27146.6 0.1521 0.00823383 -70.6726 5.63511 0.0304776 1.93409 0.358098 0.658199 0.312481 0.395602 0.31318 0.338987 0.306532 0.317066 0.314806 0.0256035 10399.8 0.0935429 0.00299166 3.16447 0.986811 0.751719 0.631266 0.55481 0.514335 0.498958 0.50015 0.501091 0.49834 0.495339 0.481519 0.48127 0 
0.0864711 30468.4 0.278539 0.00814711 -76.5047 4.75276 0.152347 2.26264 0.601497 0.437217 0.507748 0.365702 0.475505 0.454779 0.380358 0.40043 0.447341 0.0724043 13876.3 0.240453 0.00401182 6.64419 2.14029 1.29787 1.33418 0.99655 0.770833 0.775057 0.711951 0.620028 0.587816 0.548158 0.519979 0.493539 0 

# Size = 70
# MARSYAS mrs_realvec

# /GaussianClassifier/gaussian/mrs_string/inObsNames = Mean_Mem40_Centroid,Mean_Mem40_Kurtosis,Mean_Mem40_Rolloff,Mean_Mem40_Flux,Mean_Mem40_MFCC_0,Mean_Mem40_MFCC_1,Mean_Mem40_MFCC_2,Mean_Mem40_MFCC_3,Mean_Mem40_MFCC_4,Mean_Mem40_MFCC_5,Mean_Mem40_MFCC_6,Mean_Mem40_MFCC_7,Mean_Mem40_MFCC_8,Mean_Mem40_MFCC_9,Mean_Mem40_MFCC_10,Mean_Mem40_MFCC_11,Mean_Mem40_MFCC_12,Std_Mem40_Centroid,Std_Mem40_Kurtosis,Std_Mem40_Rolloff,Std_Mem40_Flux,Std_Mem40_MFCC_0,Std_Mem40_MFCC_1,Std_Mem40_MFCC_2,Std_Mem40_MFCC_3,Std_Mem40_MFCC_4,Std_Mem40_MFCC_5,Std_Mem40_MFCC_6,Std_Mem40_MFCC_7,Std_Mem40_MFCC_8,Std_Mem40_MFCC_9,Std_Mem40_MFCC_10,Std_Mem40_MFCC_11,Std_Mem40_MFCC_12,
# /GaussianClassifier/gaussian/mrs_string/mode = predict
# /GaussianClassifier/gaussian/mrs_string/onObsNames = ,

# Number of links = 0

# MarSystem
# Type = Confidence
# Name = confidence

# MarControls = 16
# /Confidence/confidence/mrs_bool/active = 1
# /Confidence/confidence/mrs_bool/debug = 0
# /Confidence/confidence/mrs_bool/forcePrint = 0
# /Confidence/confidence/mrs_bool/mute = 0
# /Confidence/confidence/mrs_bool/print = 1
# /Confidence/confidence/mrs_natural/inObservations = 2
# /Confidence/confidence/mrs_natural/inSamples = 1
# /Confidence/confidence/mrs_natural/memSize = 40
# /Confidence/confidence/mrs_natural/nLabels = 2
# /Confidence/confidence/mrs_natural/onObservations = 2
# /Confidence/confidence/mrs_natural/onSamples = 1
# /Confidence/confidence/mrs_real/israte = 43.0664
# /Confidence/confidence/mrs_real/osrate = 43.0664
# /Confidence/confidence/mrs_string/inObsNames = ,
# /Confidence/confidence/mrs_string/labelNames = music,speech,
# /Confidence/confidence/mrs_string/onObsNames = ,

# Number of links = 0


