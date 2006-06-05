# MarSystemComposite
# Type = Series
# Name = featureNetwork

# MarControls = 19
# /Series/featureNetwork/bool/debug = 0
# /Series/featureNetwork/bool/mute = 0
# /Series/featureNetwork/bool/probe = 0
# /Series/featureNetwork/natural/inObservations = 1
# /Series/featureNetwork/natural/inSamples = 512
# /Series/featureNetwork/natural/onObservations = 2
# /Series/featureNetwork/natural/onSamples = 1
# /Series/featureNetwork/real/israte = 22050
# /Series/featureNetwork/real/osrate = 43.0664
# /Series/featureNetwork/realvec/input0 = # MARSYAS realvec
# Size = 0


# type: matrix
# rows: 1
# columns: 0


# Size = 0
# MARSYAS realvec

# /Series/featureNetwork/realvec/input1 = # MARSYAS realvec
# Size = 0


# type: matrix
# rows: 1
# columns: 0


# Size = 0
# MARSYAS realvec

# /Series/featureNetwork/realvec/input2 = # MARSYAS realvec
# Size = 0


# type: matrix
# rows: 1
# columns: 0


# Size = 0
# MARSYAS realvec

# /Series/featureNetwork/realvec/input3 = # MARSYAS realvec
# Size = 0


# type: matrix
# rows: 1
# columns: 0


# Size = 0
# MARSYAS realvec

# /Series/featureNetwork/realvec/input4 = # MARSYAS realvec
# Size = 0


# type: matrix
# rows: 1
# columns: 0


# Size = 0
# MARSYAS realvec

# /Series/featureNetwork/realvec/input5 = # MARSYAS realvec
# Size = 0


# type: matrix
# rows: 1
# columns: 0


# Size = 0
# MARSYAS realvec

# /Series/featureNetwork/realvec/input6 = # MARSYAS realvec
# Size = 0


# type: matrix
# rows: 1
# columns: 0


# Size = 0
# MARSYAS realvec

# /Series/featureNetwork/realvec/input7 = # MARSYAS realvec
# Size = 0


# type: matrix
# rows: 1
# columns: 0


# Size = 0
# MARSYAS realvec

# /Series/featureNetwork/string/inObsNames = audio,
# /Series/featureNetwork/string/onObsNames = ,

# Number of links = 6
# Synonyms of /Series/featureNetwork/bool/mute = 
# Number of synonyms = 1
# /Series/featureNetwork/Gain/gt/bool/mute
# Synonyms of /Series/featureNetwork/bool/notEmpty = 
# Number of synonyms = 1
# /Series/featureNetwork/SoundFileSource/src/bool/notEmpty
# Synonyms of /Series/featureNetwork/natural/nChannels = 
# Number of synonyms = 2
# /Series/featureNetwork/SoundFileSource/src/natural/nChannels
# /Series/featureNetwork/AudioSink/dest/natural/nChannels
# Synonyms of /Series/featureNetwork/natural/pos = 
# Number of synonyms = 1
# /Series/featureNetwork/SoundFileSource/src/natural/pos
# Synonyms of /Series/featureNetwork/real/israte = 
# Number of synonyms = 1
# /Series/featureNetwork/SoundFileSource/src/real/israte
# Synonyms of /Series/featureNetwork/string/filename = 
# Number of synonyms = 1
# /Series/featureNetwork/SoundFileSource/src/string/filename

# nComponents = 9

# MarSystem
# Type = SoundFileSource
# Name = src

# MarControls = 23
# /SoundFileSource/src/bool/advance = 0
# /SoundFileSource/src/bool/debug = 0
# /SoundFileSource/src/bool/mute = 0
# /SoundFileSource/src/bool/notEmpty = 0
# /SoundFileSource/src/bool/noteon = 0
# /SoundFileSource/src/natural/inObservations = 1
# /SoundFileSource/src/natural/inSamples = 512
# /SoundFileSource/src/natural/loopPos = 0
# /SoundFileSource/src/natural/nChannels = 1
# /SoundFileSource/src/natural/numFiles = 1
# /SoundFileSource/src/natural/onObservations = 1
# /SoundFileSource/src/natural/onSamples = 512
# /SoundFileSource/src/natural/pos = 661500
# /SoundFileSource/src/natural/size = 661500
# /SoundFileSource/src/real/duration = -1
# /SoundFileSource/src/real/frequency = 0
# /SoundFileSource/src/real/israte = 22050
# /SoundFileSource/src/real/osrate = 22050
# /SoundFileSource/src/real/repetitions = 1
# /SoundFileSource/src/string/allfilenames = ,
# /SoundFileSource/src/string/filename = /home/gtzan/data/sound/music_speech/speech/voices.au
# /SoundFileSource/src/string/inObsNames = audio,
# /SoundFileSource/src/string/onObsNames = audio,

# Number of links = 0

# MarSystem
# Type = AudioSink
# Name = dest

# MarControls = 13
# /AudioSink/dest/bool/debug = 0
# /AudioSink/dest/bool/init = 0
# /AudioSink/dest/bool/mute = 0
# /AudioSink/dest/natural/bufferSize = 512
# /AudioSink/dest/natural/inObservations = 1
# /AudioSink/dest/natural/inSamples = 512
# /AudioSink/dest/natural/nChannels = 1
# /AudioSink/dest/natural/onObservations = 1
# /AudioSink/dest/natural/onSamples = 512
# /AudioSink/dest/real/israte = 22050
# /AudioSink/dest/real/osrate = 22050
# /AudioSink/dest/string/inObsNames = audio,
# /AudioSink/dest/string/onObsNames = audio,

# Number of links = 0

# MarSystemComposite
# Type = Fanout
# Name = features

# MarControls = 10
# /Fanout/features/bool/debug = 0
# /Fanout/features/bool/mute = 0
# /Fanout/features/natural/inObservations = 1
# /Fanout/features/natural/inSamples = 512
# /Fanout/features/natural/onObservations = 17
# /Fanout/features/natural/onSamples = 1
# /Fanout/features/real/israte = 22050
# /Fanout/features/real/osrate = 43.0664
# /Fanout/features/string/inObsNames = audio,
# /Fanout/features/string/onObsNames = Centroid,Kurtosis,Rolloff,Flux,MFCC_0,MFCC_1,MFCC_2,MFCC_3,MFCC_4,MFCC_5,MFCC_6,MFCC_7,MFCC_8,MFCC_9,MFCC_10,MFCC_11,MFCC_12,

# Number of links = 0

# nComponents = 1

# MarSystemComposite
# Type = Series
# Name = SpectralShape

# MarControls = 14
# /Series/SpectralShape/bool/debug = 0
# /Series/SpectralShape/bool/mute = 0
# /Series/SpectralShape/bool/probe = 0
# /Series/SpectralShape/natural/inObservations = 1
# /Series/SpectralShape/natural/inSamples = 512
# /Series/SpectralShape/natural/onObservations = 17
# /Series/SpectralShape/natural/onSamples = 1
# /Series/SpectralShape/real/israte = 22050
# /Series/SpectralShape/real/osrate = 43.0664
# /Series/SpectralShape/realvec/input0 = # MARSYAS realvec
# Size = 0


# type: matrix
# rows: 1
# columns: 0


# Size = 0
# MARSYAS realvec

# /Series/SpectralShape/realvec/input1 = # MARSYAS realvec
# Size = 0


# type: matrix
# rows: 1
# columns: 0


# Size = 0
# MARSYAS realvec

# /Series/SpectralShape/realvec/input2 = # MARSYAS realvec
# Size = 0


# type: matrix
# rows: 1
# columns: 0


# Size = 0
# MARSYAS realvec

# /Series/SpectralShape/string/inObsNames = audio,
# /Series/SpectralShape/string/onObsNames = Centroid,Kurtosis,Rolloff,Flux,MFCC_0,MFCC_1,MFCC_2,MFCC_3,MFCC_4,MFCC_5,MFCC_6,MFCC_7,MFCC_8,MFCC_9,MFCC_10,MFCC_11,MFCC_12,

# Number of links = 0

# nComponents = 4

# MarSystem
# Type = Hamming
# Name = hamming

# MarControls = 10
# /Hamming/hamming/bool/debug = 0
# /Hamming/hamming/bool/mute = 0
# /Hamming/hamming/natural/inObservations = 1
# /Hamming/hamming/natural/inSamples = 512
# /Hamming/hamming/natural/onObservations = 1
# /Hamming/hamming/natural/onSamples = 512
# /Hamming/hamming/real/israte = 22050
# /Hamming/hamming/real/osrate = 22050
# /Hamming/hamming/string/inObsNames = audio,
# /Hamming/hamming/string/onObsNames = audio,

# Number of links = 0

# MarSystem
# Type = Spectrum
# Name = spk

# MarControls = 12
# /Spectrum/spk/bool/debug = 0
# /Spectrum/spk/bool/mute = 0
# /Spectrum/spk/natural/inObservations = 1
# /Spectrum/spk/natural/inSamples = 512
# /Spectrum/spk/natural/onObservations = 512
# /Spectrum/spk/natural/onSamples = 1
# /Spectrum/spk/real/cutoff = 1
# /Spectrum/spk/real/israte = 22050
# /Spectrum/spk/real/lowcutoff = 0
# /Spectrum/spk/real/osrate = 43.0664
# /Spectrum/spk/string/inObsNames = audio,
# /Spectrum/spk/string/onObsNames = rbin_0,ibin_0,rbin_1,ibin_1,rbin_2,ibin_2,rbin_3,ibin_3,rbin_4,ibin_4,rbin_5,ibin_5,rbin_6,ibin_6,rbin_7,ibin_7,rbin_8,ibin_8,rbin_9,ibin_9,rbin_10,ibin_10,rbin_11,ibin_11,rbin_12,ibin_12,rbin_13,ibin_13,rbin_14,ibin_14,rbin_15,ibin_15,rbin_16,ibin_16,rbin_17,ibin_17,rbin_18,ibin_18,rbin_19,ibin_19,rbin_20,ibin_20,rbin_21,ibin_21,rbin_22,ibin_22,rbin_23,ibin_23,rbin_24,ibin_24,rbin_25,ibin_25,rbin_26,ibin_26,rbin_27,ibin_27,rbin_28,ibin_28,rbin_29,ibin_29,rbin_30,ibin_30,rbin_31,ibin_31,rbin_32,ibin_32,rbin_33,ibin_33,rbin_34,ibin_34,rbin_35,ibin_35,rbin_36,ibin_36,rbin_37,ibin_37,rbin_38,ibin_38,rbin_39,ibin_39,rbin_40,ibin_40,rbin_41,ibin_41,rbin_42,ibin_42,rbin_43,ibin_43,rbin_44,ibin_44,rbin_45,ibin_45,rbin_46,ibin_46,rbin_47,ibin_47,rbin_48,ibin_48,rbin_49,ibin_49,rbin_50,ibin_50,rbin_51,ibin_51,rbin_52,ibin_52,rbin_53,ibin_53,rbin_54,ibin_54,rbin_55,ibin_55,rbin_56,ibin_56,rbin_57,ibin_57,rbin_58,ibin_58,rbin_59,ibin_59,rbin_60,ibin_60,rbin_61,ibin_61,rbin_62,ibin_62,rbin_63,ibin_63,rbin_64,ibin_64,rbin_65,ibin_65,rbin_66,ibin_66,rbin_67,ibin_67,rbin_68,ibin_68,rbin_69,ibin_69,rbin_70,ibin_70,rbin_71,ibin_71,rbin_72,ibin_72,rbin_73,ibin_73,rbin_74,ibin_74,rbin_75,ibin_75,rbin_76,ibin_76,rbin_77,ibin_77,rbin_78,ibin_78,rbin_79,ibin_79,rbin_80,ibin_80,rbin_81,ibin_81,rbin_82,ibin_82,rbin_83,ibin_83,rbin_84,ibin_84,rbin_85,ibin_85,rbin_86,ibin_86,rbin_87,ibin_87,rbin_88,ibin_88,rbin_89,ibin_89,rbin_90,ibin_90,rbin_91,ibin_91,rbin_92,ibin_92,rbin_93,ibin_93,rbin_94,ibin_94,rbin_95,ibin_95,rbin_96,ibin_96,rbin_97,ibin_97,rbin_98,ibin_98,rbin_99,ibin_99,rbin_100,ibin_100,rbin_101,ibin_101,rbin_102,ibin_102,rbin_103,ibin_103,rbin_104,ibin_104,rbin_105,ibin_105,rbin_106,ibin_106,rbin_107,ibin_107,rbin_108,ibin_108,rbin_109,ibin_109,rbin_110,ibin_110,rbin_111,ibin_111,rbin_112,ibin_112,rbin_113,ibin_113,rbin_114,ibin_114,rbin_115,ibin_115,rbin_116,ibin_116,rbin_117,ibin_117,rbin_118,ibin_118,rbin_119,ibin_119,rbin_120,ibin_120,rbin_121,ibin_121,rbin_122,ibin_122,rbin_123,ibin_123,rbin_124,ibin_124,rbin_125,ibin_125,rbin_126,ibin_126,rbin_127,ibin_127,rbin_128,ibin_128,rbin_129,ibin_129,rbin_130,ibin_130,rbin_131,ibin_131,rbin_132,ibin_132,rbin_133,ibin_133,rbin_134,ibin_134,rbin_135,ibin_135,rbin_136,ibin_136,rbin_137,ibin_137,rbin_138,ibin_138,rbin_139,ibin_139,rbin_140,ibin_140,rbin_141,ibin_141,rbin_142,ibin_142,rbin_143,ibin_143,rbin_144,ibin_144,rbin_145,ibin_145,rbin_146,ibin_146,rbin_147,ibin_147,rbin_148,ibin_148,rbin_149,ibin_149,rbin_150,ibin_150,rbin_151,ibin_151,rbin_152,ibin_152,rbin_153,ibin_153,rbin_154,ibin_154,rbin_155,ibin_155,rbin_156,ibin_156,rbin_157,ibin_157,rbin_158,ibin_158,rbin_159,ibin_159,rbin_160,ibin_160,rbin_161,ibin_161,rbin_162,ibin_162,rbin_163,ibin_163,rbin_164,ibin_164,rbin_165,ibin_165,rbin_166,ibin_166,rbin_167,ibin_167,rbin_168,ibin_168,rbin_169,ibin_169,rbin_170,ibin_170,rbin_171,ibin_171,rbin_172,ibin_172,rbin_173,ibin_173,rbin_174,ibin_174,rbin_175,ibin_175,rbin_176,ibin_176,rbin_177,ibin_177,rbin_178,ibin_178,rbin_179,ibin_179,rbin_180,ibin_180,rbin_181,ibin_181,rbin_182,ibin_182,rbin_183,ibin_183,rbin_184,ibin_184,rbin_185,ibin_185,rbin_186,ibin_186,rbin_187,ibin_187,rbin_188,ibin_188,rbin_189,ibin_189,rbin_190,ibin_190,rbin_191,ibin_191,rbin_192,ibin_192,rbin_193,ibin_193,rbin_194,ibin_194,rbin_195,ibin_195,rbin_196,ibin_196,rbin_197,ibin_197,rbin_198,ibin_198,rbin_199,ibin_199,rbin_200,ibin_200,rbin_201,ibin_201,rbin_202,ibin_202,rbin_203,ibin_203,rbin_204,ibin_204,rbin_205,ibin_205,rbin_206,ibin_206,rbin_207,ibin_207,rbin_208,ibin_208,rbin_209,ibin_209,rbin_210,ibin_210,rbin_211,ibin_211,rbin_212,ibin_212,rbin_213,ibin_213,rbin_214,ibin_214,rbin_215,ibin_215,rbin_216,ibin_216,rbin_217,ibin_217,rbin_218,ibin_218,rbin_219,ibin_219,rbin_220,ibin_220,rbin_221,ibin_221,rbin_222,ibin_222,rbin_223,ibin_223,rbin_224,ibin_224,rbin_225,ibin_225,rbin_226,ibin_226,rbin_227,ibin_227,rbin_228,ibin_228,rbin_229,ibin_229,rbin_230,ibin_230,rbin_231,ibin_231,rbin_232,ibin_232,rbin_233,ibin_233,rbin_234,ibin_234,rbin_235,ibin_235,rbin_236,ibin_236,rbin_237,ibin_237,rbin_238,ibin_238,rbin_239,ibin_239,rbin_240,ibin_240,rbin_241,ibin_241,rbin_242,ibin_242,rbin_243,ibin_243,rbin_244,ibin_244,rbin_245,ibin_245,rbin_246,ibin_246,rbin_247,ibin_247,rbin_248,ibin_248,rbin_249,ibin_249,rbin_250,ibin_250,rbin_251,ibin_251,rbin_252,ibin_252,rbin_253,ibin_253,rbin_254,ibin_254,rbin_255,ibin_255,

# Number of links = 0

# MarSystem
# Type = PowerSpectrum
# Name = pspk

# MarControls = 11
# /PowerSpectrum/pspk/bool/debug = 0
# /PowerSpectrum/pspk/bool/mute = 0
# /PowerSpectrum/pspk/natural/inObservations = 512
# /PowerSpectrum/pspk/natural/inSamples = 1
# /PowerSpectrum/pspk/natural/onObservations = 256
# /PowerSpectrum/pspk/natural/onSamples = 1
# /PowerSpectrum/pspk/real/israte = 43.0664
# /PowerSpectrum/pspk/real/osrate = 43.0664
# /PowerSpectrum/pspk/string/inObsNames = rbin_0,ibin_0,rbin_1,ibin_1,rbin_2,ibin_2,rbin_3,ibin_3,rbin_4,ibin_4,rbin_5,ibin_5,rbin_6,ibin_6,rbin_7,ibin_7,rbin_8,ibin_8,rbin_9,ibin_9,rbin_10,ibin_10,rbin_11,ibin_11,rbin_12,ibin_12,rbin_13,ibin_13,rbin_14,ibin_14,rbin_15,ibin_15,rbin_16,ibin_16,rbin_17,ibin_17,rbin_18,ibin_18,rbin_19,ibin_19,rbin_20,ibin_20,rbin_21,ibin_21,rbin_22,ibin_22,rbin_23,ibin_23,rbin_24,ibin_24,rbin_25,ibin_25,rbin_26,ibin_26,rbin_27,ibin_27,rbin_28,ibin_28,rbin_29,ibin_29,rbin_30,ibin_30,rbin_31,ibin_31,rbin_32,ibin_32,rbin_33,ibin_33,rbin_34,ibin_34,rbin_35,ibin_35,rbin_36,ibin_36,rbin_37,ibin_37,rbin_38,ibin_38,rbin_39,ibin_39,rbin_40,ibin_40,rbin_41,ibin_41,rbin_42,ibin_42,rbin_43,ibin_43,rbin_44,ibin_44,rbin_45,ibin_45,rbin_46,ibin_46,rbin_47,ibin_47,rbin_48,ibin_48,rbin_49,ibin_49,rbin_50,ibin_50,rbin_51,ibin_51,rbin_52,ibin_52,rbin_53,ibin_53,rbin_54,ibin_54,rbin_55,ibin_55,rbin_56,ibin_56,rbin_57,ibin_57,rbin_58,ibin_58,rbin_59,ibin_59,rbin_60,ibin_60,rbin_61,ibin_61,rbin_62,ibin_62,rbin_63,ibin_63,rbin_64,ibin_64,rbin_65,ibin_65,rbin_66,ibin_66,rbin_67,ibin_67,rbin_68,ibin_68,rbin_69,ibin_69,rbin_70,ibin_70,rbin_71,ibin_71,rbin_72,ibin_72,rbin_73,ibin_73,rbin_74,ibin_74,rbin_75,ibin_75,rbin_76,ibin_76,rbin_77,ibin_77,rbin_78,ibin_78,rbin_79,ibin_79,rbin_80,ibin_80,rbin_81,ibin_81,rbin_82,ibin_82,rbin_83,ibin_83,rbin_84,ibin_84,rbin_85,ibin_85,rbin_86,ibin_86,rbin_87,ibin_87,rbin_88,ibin_88,rbin_89,ibin_89,rbin_90,ibin_90,rbin_91,ibin_91,rbin_92,ibin_92,rbin_93,ibin_93,rbin_94,ibin_94,rbin_95,ibin_95,rbin_96,ibin_96,rbin_97,ibin_97,rbin_98,ibin_98,rbin_99,ibin_99,rbin_100,ibin_100,rbin_101,ibin_101,rbin_102,ibin_102,rbin_103,ibin_103,rbin_104,ibin_104,rbin_105,ibin_105,rbin_106,ibin_106,rbin_107,ibin_107,rbin_108,ibin_108,rbin_109,ibin_109,rbin_110,ibin_110,rbin_111,ibin_111,rbin_112,ibin_112,rbin_113,ibin_113,rbin_114,ibin_114,rbin_115,ibin_115,rbin_116,ibin_116,rbin_117,ibin_117,rbin_118,ibin_118,rbin_119,ibin_119,rbin_120,ibin_120,rbin_121,ibin_121,rbin_122,ibin_122,rbin_123,ibin_123,rbin_124,ibin_124,rbin_125,ibin_125,rbin_126,ibin_126,rbin_127,ibin_127,rbin_128,ibin_128,rbin_129,ibin_129,rbin_130,ibin_130,rbin_131,ibin_131,rbin_132,ibin_132,rbin_133,ibin_133,rbin_134,ibin_134,rbin_135,ibin_135,rbin_136,ibin_136,rbin_137,ibin_137,rbin_138,ibin_138,rbin_139,ibin_139,rbin_140,ibin_140,rbin_141,ibin_141,rbin_142,ibin_142,rbin_143,ibin_143,rbin_144,ibin_144,rbin_145,ibin_145,rbin_146,ibin_146,rbin_147,ibin_147,rbin_148,ibin_148,rbin_149,ibin_149,rbin_150,ibin_150,rbin_151,ibin_151,rbin_152,ibin_152,rbin_153,ibin_153,rbin_154,ibin_154,rbin_155,ibin_155,rbin_156,ibin_156,rbin_157,ibin_157,rbin_158,ibin_158,rbin_159,ibin_159,rbin_160,ibin_160,rbin_161,ibin_161,rbin_162,ibin_162,rbin_163,ibin_163,rbin_164,ibin_164,rbin_165,ibin_165,rbin_166,ibin_166,rbin_167,ibin_167,rbin_168,ibin_168,rbin_169,ibin_169,rbin_170,ibin_170,rbin_171,ibin_171,rbin_172,ibin_172,rbin_173,ibin_173,rbin_174,ibin_174,rbin_175,ibin_175,rbin_176,ibin_176,rbin_177,ibin_177,rbin_178,ibin_178,rbin_179,ibin_179,rbin_180,ibin_180,rbin_181,ibin_181,rbin_182,ibin_182,rbin_183,ibin_183,rbin_184,ibin_184,rbin_185,ibin_185,rbin_186,ibin_186,rbin_187,ibin_187,rbin_188,ibin_188,rbin_189,ibin_189,rbin_190,ibin_190,rbin_191,ibin_191,rbin_192,ibin_192,rbin_193,ibin_193,rbin_194,ibin_194,rbin_195,ibin_195,rbin_196,ibin_196,rbin_197,ibin_197,rbin_198,ibin_198,rbin_199,ibin_199,rbin_200,ibin_200,rbin_201,ibin_201,rbin_202,ibin_202,rbin_203,ibin_203,rbin_204,ibin_204,rbin_205,ibin_205,rbin_206,ibin_206,rbin_207,ibin_207,rbin_208,ibin_208,rbin_209,ibin_209,rbin_210,ibin_210,rbin_211,ibin_211,rbin_212,ibin_212,rbin_213,ibin_213,rbin_214,ibin_214,rbin_215,ibin_215,rbin_216,ibin_216,rbin_217,ibin_217,rbin_218,ibin_218,rbin_219,ibin_219,rbin_220,ibin_220,rbin_221,ibin_221,rbin_222,ibin_222,rbin_223,ibin_223,rbin_224,ibin_224,rbin_225,ibin_225,rbin_226,ibin_226,rbin_227,ibin_227,rbin_228,ibin_228,rbin_229,ibin_229,rbin_230,ibin_230,rbin_231,ibin_231,rbin_232,ibin_232,rbin_233,ibin_233,rbin_234,ibin_234,rbin_235,ibin_235,rbin_236,ibin_236,rbin_237,ibin_237,rbin_238,ibin_238,rbin_239,ibin_239,rbin_240,ibin_240,rbin_241,ibin_241,rbin_242,ibin_242,rbin_243,ibin_243,rbin_244,ibin_244,rbin_245,ibin_245,rbin_246,ibin_246,rbin_247,ibin_247,rbin_248,ibin_248,rbin_249,ibin_249,rbin_250,ibin_250,rbin_251,ibin_251,rbin_252,ibin_252,rbin_253,ibin_253,rbin_254,ibin_254,rbin_255,ibin_255,
# /PowerSpectrum/pspk/string/onObsNames = mbin_0,mbin_1,mbin_2,mbin_3,mbin_4,mbin_5,mbin_6,mbin_7,mbin_8,mbin_9,mbin_10,mbin_11,mbin_12,mbin_13,mbin_14,mbin_15,mbin_16,mbin_17,mbin_18,mbin_19,mbin_20,mbin_21,mbin_22,mbin_23,mbin_24,mbin_25,mbin_26,mbin_27,mbin_28,mbin_29,mbin_30,mbin_31,mbin_32,mbin_33,mbin_34,mbin_35,mbin_36,mbin_37,mbin_38,mbin_39,mbin_40,mbin_41,mbin_42,mbin_43,mbin_44,mbin_45,mbin_46,mbin_47,mbin_48,mbin_49,mbin_50,mbin_51,mbin_52,mbin_53,mbin_54,mbin_55,mbin_56,mbin_57,mbin_58,mbin_59,mbin_60,mbin_61,mbin_62,mbin_63,mbin_64,mbin_65,mbin_66,mbin_67,mbin_68,mbin_69,mbin_70,mbin_71,mbin_72,mbin_73,mbin_74,mbin_75,mbin_76,mbin_77,mbin_78,mbin_79,mbin_80,mbin_81,mbin_82,mbin_83,mbin_84,mbin_85,mbin_86,mbin_87,mbin_88,mbin_89,mbin_90,mbin_91,mbin_92,mbin_93,mbin_94,mbin_95,mbin_96,mbin_97,mbin_98,mbin_99,mbin_100,mbin_101,mbin_102,mbin_103,mbin_104,mbin_105,mbin_106,mbin_107,mbin_108,mbin_109,mbin_110,mbin_111,mbin_112,mbin_113,mbin_114,mbin_115,mbin_116,mbin_117,mbin_118,mbin_119,mbin_120,mbin_121,mbin_122,mbin_123,mbin_124,mbin_125,mbin_126,mbin_127,mbin_128,mbin_129,mbin_130,mbin_131,mbin_132,mbin_133,mbin_134,mbin_135,mbin_136,mbin_137,mbin_138,mbin_139,mbin_140,mbin_141,mbin_142,mbin_143,mbin_144,mbin_145,mbin_146,mbin_147,mbin_148,mbin_149,mbin_150,mbin_151,mbin_152,mbin_153,mbin_154,mbin_155,mbin_156,mbin_157,mbin_158,mbin_159,mbin_160,mbin_161,mbin_162,mbin_163,mbin_164,mbin_165,mbin_166,mbin_167,mbin_168,mbin_169,mbin_170,mbin_171,mbin_172,mbin_173,mbin_174,mbin_175,mbin_176,mbin_177,mbin_178,mbin_179,mbin_180,mbin_181,mbin_182,mbin_183,mbin_184,mbin_185,mbin_186,mbin_187,mbin_188,mbin_189,mbin_190,mbin_191,mbin_192,mbin_193,mbin_194,mbin_195,mbin_196,mbin_197,mbin_198,mbin_199,mbin_200,mbin_201,mbin_202,mbin_203,mbin_204,mbin_205,mbin_206,mbin_207,mbin_208,mbin_209,mbin_210,mbin_211,mbin_212,mbin_213,mbin_214,mbin_215,mbin_216,mbin_217,mbin_218,mbin_219,mbin_220,mbin_221,mbin_222,mbin_223,mbin_224,mbin_225,mbin_226,mbin_227,mbin_228,mbin_229,mbin_230,mbin_231,mbin_232,mbin_233,mbin_234,mbin_235,mbin_236,mbin_237,mbin_238,mbin_239,mbin_240,mbin_241,mbin_242,mbin_243,mbin_244,mbin_245,mbin_246,mbin_247,mbin_248,mbin_249,mbin_250,mbin_251,mbin_252,mbin_253,mbin_254,mbin_255,
# /PowerSpectrum/pspk/string/spectrumType = power

# Number of links = 0

# MarSystemComposite
# Type = Fanout
# Name = spectrumFeatures

# MarControls = 10
# /Fanout/spectrumFeatures/bool/debug = 0
# /Fanout/spectrumFeatures/bool/mute = 0
# /Fanout/spectrumFeatures/natural/inObservations = 256
# /Fanout/spectrumFeatures/natural/inSamples = 1
# /Fanout/spectrumFeatures/natural/onObservations = 17
# /Fanout/spectrumFeatures/natural/onSamples = 1
# /Fanout/spectrumFeatures/real/israte = 43.0664
# /Fanout/spectrumFeatures/real/osrate = 43.0664
# /Fanout/spectrumFeatures/string/inObsNames = mbin_0,mbin_1,mbin_2,mbin_3,mbin_4,mbin_5,mbin_6,mbin_7,mbin_8,mbin_9,mbin_10,mbin_11,mbin_12,mbin_13,mbin_14,mbin_15,mbin_16,mbin_17,mbin_18,mbin_19,mbin_20,mbin_21,mbin_22,mbin_23,mbin_24,mbin_25,mbin_26,mbin_27,mbin_28,mbin_29,mbin_30,mbin_31,mbin_32,mbin_33,mbin_34,mbin_35,mbin_36,mbin_37,mbin_38,mbin_39,mbin_40,mbin_41,mbin_42,mbin_43,mbin_44,mbin_45,mbin_46,mbin_47,mbin_48,mbin_49,mbin_50,mbin_51,mbin_52,mbin_53,mbin_54,mbin_55,mbin_56,mbin_57,mbin_58,mbin_59,mbin_60,mbin_61,mbin_62,mbin_63,mbin_64,mbin_65,mbin_66,mbin_67,mbin_68,mbin_69,mbin_70,mbin_71,mbin_72,mbin_73,mbin_74,mbin_75,mbin_76,mbin_77,mbin_78,mbin_79,mbin_80,mbin_81,mbin_82,mbin_83,mbin_84,mbin_85,mbin_86,mbin_87,mbin_88,mbin_89,mbin_90,mbin_91,mbin_92,mbin_93,mbin_94,mbin_95,mbin_96,mbin_97,mbin_98,mbin_99,mbin_100,mbin_101,mbin_102,mbin_103,mbin_104,mbin_105,mbin_106,mbin_107,mbin_108,mbin_109,mbin_110,mbin_111,mbin_112,mbin_113,mbin_114,mbin_115,mbin_116,mbin_117,mbin_118,mbin_119,mbin_120,mbin_121,mbin_122,mbin_123,mbin_124,mbin_125,mbin_126,mbin_127,mbin_128,mbin_129,mbin_130,mbin_131,mbin_132,mbin_133,mbin_134,mbin_135,mbin_136,mbin_137,mbin_138,mbin_139,mbin_140,mbin_141,mbin_142,mbin_143,mbin_144,mbin_145,mbin_146,mbin_147,mbin_148,mbin_149,mbin_150,mbin_151,mbin_152,mbin_153,mbin_154,mbin_155,mbin_156,mbin_157,mbin_158,mbin_159,mbin_160,mbin_161,mbin_162,mbin_163,mbin_164,mbin_165,mbin_166,mbin_167,mbin_168,mbin_169,mbin_170,mbin_171,mbin_172,mbin_173,mbin_174,mbin_175,mbin_176,mbin_177,mbin_178,mbin_179,mbin_180,mbin_181,mbin_182,mbin_183,mbin_184,mbin_185,mbin_186,mbin_187,mbin_188,mbin_189,mbin_190,mbin_191,mbin_192,mbin_193,mbin_194,mbin_195,mbin_196,mbin_197,mbin_198,mbin_199,mbin_200,mbin_201,mbin_202,mbin_203,mbin_204,mbin_205,mbin_206,mbin_207,mbin_208,mbin_209,mbin_210,mbin_211,mbin_212,mbin_213,mbin_214,mbin_215,mbin_216,mbin_217,mbin_218,mbin_219,mbin_220,mbin_221,mbin_222,mbin_223,mbin_224,mbin_225,mbin_226,mbin_227,mbin_228,mbin_229,mbin_230,mbin_231,mbin_232,mbin_233,mbin_234,mbin_235,mbin_236,mbin_237,mbin_238,mbin_239,mbin_240,mbin_241,mbin_242,mbin_243,mbin_244,mbin_245,mbin_246,mbin_247,mbin_248,mbin_249,mbin_250,mbin_251,mbin_252,mbin_253,mbin_254,mbin_255,
# /Fanout/spectrumFeatures/string/onObsNames = Centroid,Kurtosis,Rolloff,Flux,MFCC_0,MFCC_1,MFCC_2,MFCC_3,MFCC_4,MFCC_5,MFCC_6,MFCC_7,MFCC_8,MFCC_9,MFCC_10,MFCC_11,MFCC_12,

# Number of links = 0

# nComponents = 5

# MarSystem
# Type = Centroid
# Name = cntrd

# MarControls = 10
# /Centroid/cntrd/bool/debug = 0
# /Centroid/cntrd/bool/mute = 0
# /Centroid/cntrd/natural/inObservations = 256
# /Centroid/cntrd/natural/inSamples = 1
# /Centroid/cntrd/natural/onObservations = 1
# /Centroid/cntrd/natural/onSamples = 1
# /Centroid/cntrd/real/israte = 43.0664
# /Centroid/cntrd/real/osrate = 43.0664
# /Centroid/cntrd/string/inObsNames = mbin_0,mbin_1,mbin_2,mbin_3,mbin_4,mbin_5,mbin_6,mbin_7,mbin_8,mbin_9,mbin_10,mbin_11,mbin_12,mbin_13,mbin_14,mbin_15,mbin_16,mbin_17,mbin_18,mbin_19,mbin_20,mbin_21,mbin_22,mbin_23,mbin_24,mbin_25,mbin_26,mbin_27,mbin_28,mbin_29,mbin_30,mbin_31,mbin_32,mbin_33,mbin_34,mbin_35,mbin_36,mbin_37,mbin_38,mbin_39,mbin_40,mbin_41,mbin_42,mbin_43,mbin_44,mbin_45,mbin_46,mbin_47,mbin_48,mbin_49,mbin_50,mbin_51,mbin_52,mbin_53,mbin_54,mbin_55,mbin_56,mbin_57,mbin_58,mbin_59,mbin_60,mbin_61,mbin_62,mbin_63,mbin_64,mbin_65,mbin_66,mbin_67,mbin_68,mbin_69,mbin_70,mbin_71,mbin_72,mbin_73,mbin_74,mbin_75,mbin_76,mbin_77,mbin_78,mbin_79,mbin_80,mbin_81,mbin_82,mbin_83,mbin_84,mbin_85,mbin_86,mbin_87,mbin_88,mbin_89,mbin_90,mbin_91,mbin_92,mbin_93,mbin_94,mbin_95,mbin_96,mbin_97,mbin_98,mbin_99,mbin_100,mbin_101,mbin_102,mbin_103,mbin_104,mbin_105,mbin_106,mbin_107,mbin_108,mbin_109,mbin_110,mbin_111,mbin_112,mbin_113,mbin_114,mbin_115,mbin_116,mbin_117,mbin_118,mbin_119,mbin_120,mbin_121,mbin_122,mbin_123,mbin_124,mbin_125,mbin_126,mbin_127,mbin_128,mbin_129,mbin_130,mbin_131,mbin_132,mbin_133,mbin_134,mbin_135,mbin_136,mbin_137,mbin_138,mbin_139,mbin_140,mbin_141,mbin_142,mbin_143,mbin_144,mbin_145,mbin_146,mbin_147,mbin_148,mbin_149,mbin_150,mbin_151,mbin_152,mbin_153,mbin_154,mbin_155,mbin_156,mbin_157,mbin_158,mbin_159,mbin_160,mbin_161,mbin_162,mbin_163,mbin_164,mbin_165,mbin_166,mbin_167,mbin_168,mbin_169,mbin_170,mbin_171,mbin_172,mbin_173,mbin_174,mbin_175,mbin_176,mbin_177,mbin_178,mbin_179,mbin_180,mbin_181,mbin_182,mbin_183,mbin_184,mbin_185,mbin_186,mbin_187,mbin_188,mbin_189,mbin_190,mbin_191,mbin_192,mbin_193,mbin_194,mbin_195,mbin_196,mbin_197,mbin_198,mbin_199,mbin_200,mbin_201,mbin_202,mbin_203,mbin_204,mbin_205,mbin_206,mbin_207,mbin_208,mbin_209,mbin_210,mbin_211,mbin_212,mbin_213,mbin_214,mbin_215,mbin_216,mbin_217,mbin_218,mbin_219,mbin_220,mbin_221,mbin_222,mbin_223,mbin_224,mbin_225,mbin_226,mbin_227,mbin_228,mbin_229,mbin_230,mbin_231,mbin_232,mbin_233,mbin_234,mbin_235,mbin_236,mbin_237,mbin_238,mbin_239,mbin_240,mbin_241,mbin_242,mbin_243,mbin_244,mbin_245,mbin_246,mbin_247,mbin_248,mbin_249,mbin_250,mbin_251,mbin_252,mbin_253,mbin_254,mbin_255,
# /Centroid/cntrd/string/onObsNames = Centroid,

# Number of links = 0

# MarSystem
# Type = Kurtosis
# Name = krt

# MarControls = 10
# /Kurtosis/krt/bool/debug = 0
# /Kurtosis/krt/bool/mute = 0
# /Kurtosis/krt/natural/inObservations = 256
# /Kurtosis/krt/natural/inSamples = 1
# /Kurtosis/krt/natural/onObservations = 1
# /Kurtosis/krt/natural/onSamples = 1
# /Kurtosis/krt/real/israte = 43.0664
# /Kurtosis/krt/real/osrate = 43.0664
# /Kurtosis/krt/string/inObsNames = mbin_0,mbin_1,mbin_2,mbin_3,mbin_4,mbin_5,mbin_6,mbin_7,mbin_8,mbin_9,mbin_10,mbin_11,mbin_12,mbin_13,mbin_14,mbin_15,mbin_16,mbin_17,mbin_18,mbin_19,mbin_20,mbin_21,mbin_22,mbin_23,mbin_24,mbin_25,mbin_26,mbin_27,mbin_28,mbin_29,mbin_30,mbin_31,mbin_32,mbin_33,mbin_34,mbin_35,mbin_36,mbin_37,mbin_38,mbin_39,mbin_40,mbin_41,mbin_42,mbin_43,mbin_44,mbin_45,mbin_46,mbin_47,mbin_48,mbin_49,mbin_50,mbin_51,mbin_52,mbin_53,mbin_54,mbin_55,mbin_56,mbin_57,mbin_58,mbin_59,mbin_60,mbin_61,mbin_62,mbin_63,mbin_64,mbin_65,mbin_66,mbin_67,mbin_68,mbin_69,mbin_70,mbin_71,mbin_72,mbin_73,mbin_74,mbin_75,mbin_76,mbin_77,mbin_78,mbin_79,mbin_80,mbin_81,mbin_82,mbin_83,mbin_84,mbin_85,mbin_86,mbin_87,mbin_88,mbin_89,mbin_90,mbin_91,mbin_92,mbin_93,mbin_94,mbin_95,mbin_96,mbin_97,mbin_98,mbin_99,mbin_100,mbin_101,mbin_102,mbin_103,mbin_104,mbin_105,mbin_106,mbin_107,mbin_108,mbin_109,mbin_110,mbin_111,mbin_112,mbin_113,mbin_114,mbin_115,mbin_116,mbin_117,mbin_118,mbin_119,mbin_120,mbin_121,mbin_122,mbin_123,mbin_124,mbin_125,mbin_126,mbin_127,mbin_128,mbin_129,mbin_130,mbin_131,mbin_132,mbin_133,mbin_134,mbin_135,mbin_136,mbin_137,mbin_138,mbin_139,mbin_140,mbin_141,mbin_142,mbin_143,mbin_144,mbin_145,mbin_146,mbin_147,mbin_148,mbin_149,mbin_150,mbin_151,mbin_152,mbin_153,mbin_154,mbin_155,mbin_156,mbin_157,mbin_158,mbin_159,mbin_160,mbin_161,mbin_162,mbin_163,mbin_164,mbin_165,mbin_166,mbin_167,mbin_168,mbin_169,mbin_170,mbin_171,mbin_172,mbin_173,mbin_174,mbin_175,mbin_176,mbin_177,mbin_178,mbin_179,mbin_180,mbin_181,mbin_182,mbin_183,mbin_184,mbin_185,mbin_186,mbin_187,mbin_188,mbin_189,mbin_190,mbin_191,mbin_192,mbin_193,mbin_194,mbin_195,mbin_196,mbin_197,mbin_198,mbin_199,mbin_200,mbin_201,mbin_202,mbin_203,mbin_204,mbin_205,mbin_206,mbin_207,mbin_208,mbin_209,mbin_210,mbin_211,mbin_212,mbin_213,mbin_214,mbin_215,mbin_216,mbin_217,mbin_218,mbin_219,mbin_220,mbin_221,mbin_222,mbin_223,mbin_224,mbin_225,mbin_226,mbin_227,mbin_228,mbin_229,mbin_230,mbin_231,mbin_232,mbin_233,mbin_234,mbin_235,mbin_236,mbin_237,mbin_238,mbin_239,mbin_240,mbin_241,mbin_242,mbin_243,mbin_244,mbin_245,mbin_246,mbin_247,mbin_248,mbin_249,mbin_250,mbin_251,mbin_252,mbin_253,mbin_254,mbin_255,
# /Kurtosis/krt/string/onObsNames = Kurtosis,

# Number of links = 0

# MarSystem
# Type = Rolloff
# Name = rlf

# MarControls = 11
# /Rolloff/rlf/bool/debug = 0
# /Rolloff/rlf/bool/mute = 0
# /Rolloff/rlf/natural/inObservations = 256
# /Rolloff/rlf/natural/inSamples = 1
# /Rolloff/rlf/natural/onObservations = 1
# /Rolloff/rlf/natural/onSamples = 1
# /Rolloff/rlf/real/israte = 43.0664
# /Rolloff/rlf/real/osrate = 43.0664
# /Rolloff/rlf/real/percentage = 0.9
# /Rolloff/rlf/string/inObsNames = mbin_0,mbin_1,mbin_2,mbin_3,mbin_4,mbin_5,mbin_6,mbin_7,mbin_8,mbin_9,mbin_10,mbin_11,mbin_12,mbin_13,mbin_14,mbin_15,mbin_16,mbin_17,mbin_18,mbin_19,mbin_20,mbin_21,mbin_22,mbin_23,mbin_24,mbin_25,mbin_26,mbin_27,mbin_28,mbin_29,mbin_30,mbin_31,mbin_32,mbin_33,mbin_34,mbin_35,mbin_36,mbin_37,mbin_38,mbin_39,mbin_40,mbin_41,mbin_42,mbin_43,mbin_44,mbin_45,mbin_46,mbin_47,mbin_48,mbin_49,mbin_50,mbin_51,mbin_52,mbin_53,mbin_54,mbin_55,mbin_56,mbin_57,mbin_58,mbin_59,mbin_60,mbin_61,mbin_62,mbin_63,mbin_64,mbin_65,mbin_66,mbin_67,mbin_68,mbin_69,mbin_70,mbin_71,mbin_72,mbin_73,mbin_74,mbin_75,mbin_76,mbin_77,mbin_78,mbin_79,mbin_80,mbin_81,mbin_82,mbin_83,mbin_84,mbin_85,mbin_86,mbin_87,mbin_88,mbin_89,mbin_90,mbin_91,mbin_92,mbin_93,mbin_94,mbin_95,mbin_96,mbin_97,mbin_98,mbin_99,mbin_100,mbin_101,mbin_102,mbin_103,mbin_104,mbin_105,mbin_106,mbin_107,mbin_108,mbin_109,mbin_110,mbin_111,mbin_112,mbin_113,mbin_114,mbin_115,mbin_116,mbin_117,mbin_118,mbin_119,mbin_120,mbin_121,mbin_122,mbin_123,mbin_124,mbin_125,mbin_126,mbin_127,mbin_128,mbin_129,mbin_130,mbin_131,mbin_132,mbin_133,mbin_134,mbin_135,mbin_136,mbin_137,mbin_138,mbin_139,mbin_140,mbin_141,mbin_142,mbin_143,mbin_144,mbin_145,mbin_146,mbin_147,mbin_148,mbin_149,mbin_150,mbin_151,mbin_152,mbin_153,mbin_154,mbin_155,mbin_156,mbin_157,mbin_158,mbin_159,mbin_160,mbin_161,mbin_162,mbin_163,mbin_164,mbin_165,mbin_166,mbin_167,mbin_168,mbin_169,mbin_170,mbin_171,mbin_172,mbin_173,mbin_174,mbin_175,mbin_176,mbin_177,mbin_178,mbin_179,mbin_180,mbin_181,mbin_182,mbin_183,mbin_184,mbin_185,mbin_186,mbin_187,mbin_188,mbin_189,mbin_190,mbin_191,mbin_192,mbin_193,mbin_194,mbin_195,mbin_196,mbin_197,mbin_198,mbin_199,mbin_200,mbin_201,mbin_202,mbin_203,mbin_204,mbin_205,mbin_206,mbin_207,mbin_208,mbin_209,mbin_210,mbin_211,mbin_212,mbin_213,mbin_214,mbin_215,mbin_216,mbin_217,mbin_218,mbin_219,mbin_220,mbin_221,mbin_222,mbin_223,mbin_224,mbin_225,mbin_226,mbin_227,mbin_228,mbin_229,mbin_230,mbin_231,mbin_232,mbin_233,mbin_234,mbin_235,mbin_236,mbin_237,mbin_238,mbin_239,mbin_240,mbin_241,mbin_242,mbin_243,mbin_244,mbin_245,mbin_246,mbin_247,mbin_248,mbin_249,mbin_250,mbin_251,mbin_252,mbin_253,mbin_254,mbin_255,
# /Rolloff/rlf/string/onObsNames = Rolloff,

# Number of links = 0

# MarSystem
# Type = Flux
# Name = flux

# MarControls = 10
# /Flux/flux/bool/debug = 0
# /Flux/flux/bool/mute = 0
# /Flux/flux/natural/inObservations = 256
# /Flux/flux/natural/inSamples = 1
# /Flux/flux/natural/onObservations = 1
# /Flux/flux/natural/onSamples = 1
# /Flux/flux/real/israte = 43.0664
# /Flux/flux/real/osrate = 43.0664
# /Flux/flux/string/inObsNames = mbin_0,mbin_1,mbin_2,mbin_3,mbin_4,mbin_5,mbin_6,mbin_7,mbin_8,mbin_9,mbin_10,mbin_11,mbin_12,mbin_13,mbin_14,mbin_15,mbin_16,mbin_17,mbin_18,mbin_19,mbin_20,mbin_21,mbin_22,mbin_23,mbin_24,mbin_25,mbin_26,mbin_27,mbin_28,mbin_29,mbin_30,mbin_31,mbin_32,mbin_33,mbin_34,mbin_35,mbin_36,mbin_37,mbin_38,mbin_39,mbin_40,mbin_41,mbin_42,mbin_43,mbin_44,mbin_45,mbin_46,mbin_47,mbin_48,mbin_49,mbin_50,mbin_51,mbin_52,mbin_53,mbin_54,mbin_55,mbin_56,mbin_57,mbin_58,mbin_59,mbin_60,mbin_61,mbin_62,mbin_63,mbin_64,mbin_65,mbin_66,mbin_67,mbin_68,mbin_69,mbin_70,mbin_71,mbin_72,mbin_73,mbin_74,mbin_75,mbin_76,mbin_77,mbin_78,mbin_79,mbin_80,mbin_81,mbin_82,mbin_83,mbin_84,mbin_85,mbin_86,mbin_87,mbin_88,mbin_89,mbin_90,mbin_91,mbin_92,mbin_93,mbin_94,mbin_95,mbin_96,mbin_97,mbin_98,mbin_99,mbin_100,mbin_101,mbin_102,mbin_103,mbin_104,mbin_105,mbin_106,mbin_107,mbin_108,mbin_109,mbin_110,mbin_111,mbin_112,mbin_113,mbin_114,mbin_115,mbin_116,mbin_117,mbin_118,mbin_119,mbin_120,mbin_121,mbin_122,mbin_123,mbin_124,mbin_125,mbin_126,mbin_127,mbin_128,mbin_129,mbin_130,mbin_131,mbin_132,mbin_133,mbin_134,mbin_135,mbin_136,mbin_137,mbin_138,mbin_139,mbin_140,mbin_141,mbin_142,mbin_143,mbin_144,mbin_145,mbin_146,mbin_147,mbin_148,mbin_149,mbin_150,mbin_151,mbin_152,mbin_153,mbin_154,mbin_155,mbin_156,mbin_157,mbin_158,mbin_159,mbin_160,mbin_161,mbin_162,mbin_163,mbin_164,mbin_165,mbin_166,mbin_167,mbin_168,mbin_169,mbin_170,mbin_171,mbin_172,mbin_173,mbin_174,mbin_175,mbin_176,mbin_177,mbin_178,mbin_179,mbin_180,mbin_181,mbin_182,mbin_183,mbin_184,mbin_185,mbin_186,mbin_187,mbin_188,mbin_189,mbin_190,mbin_191,mbin_192,mbin_193,mbin_194,mbin_195,mbin_196,mbin_197,mbin_198,mbin_199,mbin_200,mbin_201,mbin_202,mbin_203,mbin_204,mbin_205,mbin_206,mbin_207,mbin_208,mbin_209,mbin_210,mbin_211,mbin_212,mbin_213,mbin_214,mbin_215,mbin_216,mbin_217,mbin_218,mbin_219,mbin_220,mbin_221,mbin_222,mbin_223,mbin_224,mbin_225,mbin_226,mbin_227,mbin_228,mbin_229,mbin_230,mbin_231,mbin_232,mbin_233,mbin_234,mbin_235,mbin_236,mbin_237,mbin_238,mbin_239,mbin_240,mbin_241,mbin_242,mbin_243,mbin_244,mbin_245,mbin_246,mbin_247,mbin_248,mbin_249,mbin_250,mbin_251,mbin_252,mbin_253,mbin_254,mbin_255,
# /Flux/flux/string/onObsNames = Flux,

# Number of links = 0

# MarSystem
# Type = MFCC
# Name = mfcc

# MarControls = 10
# /MFCC/mfcc/bool/debug = 0
# /MFCC/mfcc/bool/mute = 0
# /MFCC/mfcc/natural/inObservations = 256
# /MFCC/mfcc/natural/inSamples = 1
# /MFCC/mfcc/natural/onObservations = 13
# /MFCC/mfcc/natural/onSamples = 1
# /MFCC/mfcc/real/israte = 43.0664
# /MFCC/mfcc/real/osrate = 43.0664
# /MFCC/mfcc/string/inObsNames = mbin_0,mbin_1,mbin_2,mbin_3,mbin_4,mbin_5,mbin_6,mbin_7,mbin_8,mbin_9,mbin_10,mbin_11,mbin_12,mbin_13,mbin_14,mbin_15,mbin_16,mbin_17,mbin_18,mbin_19,mbin_20,mbin_21,mbin_22,mbin_23,mbin_24,mbin_25,mbin_26,mbin_27,mbin_28,mbin_29,mbin_30,mbin_31,mbin_32,mbin_33,mbin_34,mbin_35,mbin_36,mbin_37,mbin_38,mbin_39,mbin_40,mbin_41,mbin_42,mbin_43,mbin_44,mbin_45,mbin_46,mbin_47,mbin_48,mbin_49,mbin_50,mbin_51,mbin_52,mbin_53,mbin_54,mbin_55,mbin_56,mbin_57,mbin_58,mbin_59,mbin_60,mbin_61,mbin_62,mbin_63,mbin_64,mbin_65,mbin_66,mbin_67,mbin_68,mbin_69,mbin_70,mbin_71,mbin_72,mbin_73,mbin_74,mbin_75,mbin_76,mbin_77,mbin_78,mbin_79,mbin_80,mbin_81,mbin_82,mbin_83,mbin_84,mbin_85,mbin_86,mbin_87,mbin_88,mbin_89,mbin_90,mbin_91,mbin_92,mbin_93,mbin_94,mbin_95,mbin_96,mbin_97,mbin_98,mbin_99,mbin_100,mbin_101,mbin_102,mbin_103,mbin_104,mbin_105,mbin_106,mbin_107,mbin_108,mbin_109,mbin_110,mbin_111,mbin_112,mbin_113,mbin_114,mbin_115,mbin_116,mbin_117,mbin_118,mbin_119,mbin_120,mbin_121,mbin_122,mbin_123,mbin_124,mbin_125,mbin_126,mbin_127,mbin_128,mbin_129,mbin_130,mbin_131,mbin_132,mbin_133,mbin_134,mbin_135,mbin_136,mbin_137,mbin_138,mbin_139,mbin_140,mbin_141,mbin_142,mbin_143,mbin_144,mbin_145,mbin_146,mbin_147,mbin_148,mbin_149,mbin_150,mbin_151,mbin_152,mbin_153,mbin_154,mbin_155,mbin_156,mbin_157,mbin_158,mbin_159,mbin_160,mbin_161,mbin_162,mbin_163,mbin_164,mbin_165,mbin_166,mbin_167,mbin_168,mbin_169,mbin_170,mbin_171,mbin_172,mbin_173,mbin_174,mbin_175,mbin_176,mbin_177,mbin_178,mbin_179,mbin_180,mbin_181,mbin_182,mbin_183,mbin_184,mbin_185,mbin_186,mbin_187,mbin_188,mbin_189,mbin_190,mbin_191,mbin_192,mbin_193,mbin_194,mbin_195,mbin_196,mbin_197,mbin_198,mbin_199,mbin_200,mbin_201,mbin_202,mbin_203,mbin_204,mbin_205,mbin_206,mbin_207,mbin_208,mbin_209,mbin_210,mbin_211,mbin_212,mbin_213,mbin_214,mbin_215,mbin_216,mbin_217,mbin_218,mbin_219,mbin_220,mbin_221,mbin_222,mbin_223,mbin_224,mbin_225,mbin_226,mbin_227,mbin_228,mbin_229,mbin_230,mbin_231,mbin_232,mbin_233,mbin_234,mbin_235,mbin_236,mbin_237,mbin_238,mbin_239,mbin_240,mbin_241,mbin_242,mbin_243,mbin_244,mbin_245,mbin_246,mbin_247,mbin_248,mbin_249,mbin_250,mbin_251,mbin_252,mbin_253,mbin_254,mbin_255,
# /MFCC/mfcc/string/onObsNames = MFCC_0,MFCC_1,MFCC_2,MFCC_3,MFCC_4,MFCC_5,MFCC_6,MFCC_7,MFCC_8,MFCC_9,MFCC_10,MFCC_11,MFCC_12,

# Number of links = 0




# MarSystem
# Type = Memory
# Name = memory

# MarControls = 12
# /Memory/memory/bool/debug = 0
# /Memory/memory/bool/mute = 0
# /Memory/memory/bool/reset = 0
# /Memory/memory/natural/inObservations = 17
# /Memory/memory/natural/inSamples = 1
# /Memory/memory/natural/memSize = 40
# /Memory/memory/natural/onObservations = 17
# /Memory/memory/natural/onSamples = 40
# /Memory/memory/real/israte = 43.0664
# /Memory/memory/real/osrate = 43.0664
# /Memory/memory/string/inObsNames = Centroid,Kurtosis,Rolloff,Flux,MFCC_0,MFCC_1,MFCC_2,MFCC_3,MFCC_4,MFCC_5,MFCC_6,MFCC_7,MFCC_8,MFCC_9,MFCC_10,MFCC_11,MFCC_12,
# /Memory/memory/string/onObsNames = Mem40_Centroid,Mem40_Kurtosis,Mem40_Rolloff,Mem40_Flux,Mem40_MFCC_0,Mem40_MFCC_1,Mem40_MFCC_2,Mem40_MFCC_3,Mem40_MFCC_4,Mem40_MFCC_5,Mem40_MFCC_6,Mem40_MFCC_7,Mem40_MFCC_8,Mem40_MFCC_9,Mem40_MFCC_10,Mem40_MFCC_11,Mem40_MFCC_12,

# Number of links = 0

# MarSystemComposite
# Type = Fanout
# Name = statistics

# MarControls = 10
# /Fanout/statistics/bool/debug = 0
# /Fanout/statistics/bool/mute = 0
# /Fanout/statistics/natural/inObservations = 17
# /Fanout/statistics/natural/inSamples = 40
# /Fanout/statistics/natural/onObservations = 34
# /Fanout/statistics/natural/onSamples = 1
# /Fanout/statistics/real/israte = 43.0664
# /Fanout/statistics/real/osrate = 43.0664
# /Fanout/statistics/string/inObsNames = Mem40_Centroid,Mem40_Kurtosis,Mem40_Rolloff,Mem40_Flux,Mem40_MFCC_0,Mem40_MFCC_1,Mem40_MFCC_2,Mem40_MFCC_3,Mem40_MFCC_4,Mem40_MFCC_5,Mem40_MFCC_6,Mem40_MFCC_7,Mem40_MFCC_8,Mem40_MFCC_9,Mem40_MFCC_10,Mem40_MFCC_11,Mem40_MFCC_12,
# /Fanout/statistics/string/onObsNames = Mean_Mem40_Centroid,Mean_Mem40_Kurtosis,Mean_Mem40_Rolloff,Mean_Mem40_Flux,Mean_Mem40_MFCC_0,Mean_Mem40_MFCC_1,Mean_Mem40_MFCC_2,Mean_Mem40_MFCC_3,Mean_Mem40_MFCC_4,Mean_Mem40_MFCC_5,Mean_Mem40_MFCC_6,Mean_Mem40_MFCC_7,Mean_Mem40_MFCC_8,Mean_Mem40_MFCC_9,Mean_Mem40_MFCC_10,Mean_Mem40_MFCC_11,Mean_Mem40_MFCC_12,Std_Mem40_Centroid,Std_Mem40_Kurtosis,Std_Mem40_Rolloff,Std_Mem40_Flux,Std_Mem40_MFCC_0,Std_Mem40_MFCC_1,Std_Mem40_MFCC_2,Std_Mem40_MFCC_3,Std_Mem40_MFCC_4,Std_Mem40_MFCC_5,Std_Mem40_MFCC_6,Std_Mem40_MFCC_7,Std_Mem40_MFCC_8,Std_Mem40_MFCC_9,Std_Mem40_MFCC_10,Std_Mem40_MFCC_11,Std_Mem40_MFCC_12,

# Number of links = 0

# nComponents = 2

# MarSystem
# Type = Mean
# Name = mn

# MarControls = 10
# /Mean/mn/bool/debug = 0
# /Mean/mn/bool/mute = 0
# /Mean/mn/natural/inObservations = 17
# /Mean/mn/natural/inSamples = 40
# /Mean/mn/natural/onObservations = 17
# /Mean/mn/natural/onSamples = 1
# /Mean/mn/real/israte = 43.0664
# /Mean/mn/real/osrate = 43.0664
# /Mean/mn/string/inObsNames = Mem40_Centroid,Mem40_Kurtosis,Mem40_Rolloff,Mem40_Flux,Mem40_MFCC_0,Mem40_MFCC_1,Mem40_MFCC_2,Mem40_MFCC_3,Mem40_MFCC_4,Mem40_MFCC_5,Mem40_MFCC_6,Mem40_MFCC_7,Mem40_MFCC_8,Mem40_MFCC_9,Mem40_MFCC_10,Mem40_MFCC_11,Mem40_MFCC_12,
# /Mean/mn/string/onObsNames = Mean_Mem40_Centroid,Mean_Mem40_Kurtosis,Mean_Mem40_Rolloff,Mean_Mem40_Flux,Mean_Mem40_MFCC_0,Mean_Mem40_MFCC_1,Mean_Mem40_MFCC_2,Mean_Mem40_MFCC_3,Mean_Mem40_MFCC_4,Mean_Mem40_MFCC_5,Mean_Mem40_MFCC_6,Mean_Mem40_MFCC_7,Mean_Mem40_MFCC_8,Mean_Mem40_MFCC_9,Mean_Mem40_MFCC_10,Mean_Mem40_MFCC_11,Mean_Mem40_MFCC_12,

# Number of links = 0

# MarSystem
# Type = StandardDeviation
# Name = std

# MarControls = 10
# /StandardDeviation/std/bool/debug = 0
# /StandardDeviation/std/bool/mute = 0
# /StandardDeviation/std/natural/inObservations = 17
# /StandardDeviation/std/natural/inSamples = 40
# /StandardDeviation/std/natural/onObservations = 17
# /StandardDeviation/std/natural/onSamples = 1
# /StandardDeviation/std/real/israte = 43.0664
# /StandardDeviation/std/real/osrate = 43.0664
# /StandardDeviation/std/string/inObsNames = Mem40_Centroid,Mem40_Kurtosis,Mem40_Rolloff,Mem40_Flux,Mem40_MFCC_0,Mem40_MFCC_1,Mem40_MFCC_2,Mem40_MFCC_3,Mem40_MFCC_4,Mem40_MFCC_5,Mem40_MFCC_6,Mem40_MFCC_7,Mem40_MFCC_8,Mem40_MFCC_9,Mem40_MFCC_10,Mem40_MFCC_11,Mem40_MFCC_12,
# /StandardDeviation/std/string/onObsNames = Std_Mem40_Centroid,Std_Mem40_Kurtosis,Std_Mem40_Rolloff,Std_Mem40_Flux,Std_Mem40_MFCC_0,Std_Mem40_MFCC_1,Std_Mem40_MFCC_2,Std_Mem40_MFCC_3,Std_Mem40_MFCC_4,Std_Mem40_MFCC_5,Std_Mem40_MFCC_6,Std_Mem40_MFCC_7,Std_Mem40_MFCC_8,Std_Mem40_MFCC_9,Std_Mem40_MFCC_10,Std_Mem40_MFCC_11,Std_Mem40_MFCC_12,

# Number of links = 0


# MarSystem
# Type = Annotator
# Name = annotator

# MarControls = 11
# /Annotator/annotator/bool/debug = 0
# /Annotator/annotator/bool/mute = 0
# /Annotator/annotator/natural/inObservations = 34
# /Annotator/annotator/natural/inSamples = 1
# /Annotator/annotator/natural/label = 1
# /Annotator/annotator/natural/onObservations = 35
# /Annotator/annotator/natural/onSamples = 1
# /Annotator/annotator/real/israte = 43.0664
# /Annotator/annotator/real/osrate = 43.0664
# /Annotator/annotator/string/inObsNames = Mean_Mem40_Centroid,Mean_Mem40_Kurtosis,Mean_Mem40_Rolloff,Mean_Mem40_Flux,Mean_Mem40_MFCC_0,Mean_Mem40_MFCC_1,Mean_Mem40_MFCC_2,Mean_Mem40_MFCC_3,Mean_Mem40_MFCC_4,Mean_Mem40_MFCC_5,Mean_Mem40_MFCC_6,Mean_Mem40_MFCC_7,Mean_Mem40_MFCC_8,Mean_Mem40_MFCC_9,Mean_Mem40_MFCC_10,Mean_Mem40_MFCC_11,Mean_Mem40_MFCC_12,Std_Mem40_Centroid,Std_Mem40_Kurtosis,Std_Mem40_Rolloff,Std_Mem40_Flux,Std_Mem40_MFCC_0,Std_Mem40_MFCC_1,Std_Mem40_MFCC_2,Std_Mem40_MFCC_3,Std_Mem40_MFCC_4,Std_Mem40_MFCC_5,Std_Mem40_MFCC_6,Std_Mem40_MFCC_7,Std_Mem40_MFCC_8,Std_Mem40_MFCC_9,Std_Mem40_MFCC_10,Std_Mem40_MFCC_11,Std_Mem40_MFCC_12,
# /Annotator/annotator/string/onObsNames = Mean_Mem40_Centroid,Mean_Mem40_Kurtosis,Mean_Mem40_Rolloff,Mean_Mem40_Flux,Mean_Mem40_MFCC_0,Mean_Mem40_MFCC_1,Mean_Mem40_MFCC_2,Mean_Mem40_MFCC_3,Mean_Mem40_MFCC_4,Mean_Mem40_MFCC_5,Mean_Mem40_MFCC_6,Mean_Mem40_MFCC_7,Mean_Mem40_MFCC_8,Mean_Mem40_MFCC_9,Mean_Mem40_MFCC_10,Mean_Mem40_MFCC_11,Mean_Mem40_MFCC_12,Std_Mem40_Centroid,Std_Mem40_Kurtosis,Std_Mem40_Rolloff,Std_Mem40_Flux,Std_Mem40_MFCC_0,Std_Mem40_MFCC_1,Std_Mem40_MFCC_2,Std_Mem40_MFCC_3,Std_Mem40_MFCC_4,Std_Mem40_MFCC_5,Std_Mem40_MFCC_6,Std_Mem40_MFCC_7,Std_Mem40_MFCC_8,Std_Mem40_MFCC_9,Std_Mem40_MFCC_10,Std_Mem40_MFCC_11,Std_Mem40_MFCC_12,

# Number of links = 0

# MarSystem
# Type = WekaSink
# Name = wsink

# MarControls = 15
# /WekaSink/wsink/bool/debug = 0
# /WekaSink/wsink/bool/mute = 1
# /WekaSink/wsink/natural/downsample = 40
# /WekaSink/wsink/natural/inObservations = 35
# /WekaSink/wsink/natural/inSamples = 1
# /WekaSink/wsink/natural/nLabels = 2
# /WekaSink/wsink/natural/onObservations = 35
# /WekaSink/wsink/natural/onSamples = 1
# /WekaSink/wsink/natural/precision = 6
# /WekaSink/wsink/real/israte = 43.0664
# /WekaSink/wsink/real/osrate = 43.0664
# /WekaSink/wsink/string/filename = weka.arff
# /WekaSink/wsink/string/inObsNames = Mean_Mem40_Centroid,Mean_Mem40_Kurtosis,Mean_Mem40_Rolloff,Mean_Mem40_Flux,Mean_Mem40_MFCC_0,Mean_Mem40_MFCC_1,Mean_Mem40_MFCC_2,Mean_Mem40_MFCC_3,Mean_Mem40_MFCC_4,Mean_Mem40_MFCC_5,Mean_Mem40_MFCC_6,Mean_Mem40_MFCC_7,Mean_Mem40_MFCC_8,Mean_Mem40_MFCC_9,Mean_Mem40_MFCC_10,Mean_Mem40_MFCC_11,Mean_Mem40_MFCC_12,Std_Mem40_Centroid,Std_Mem40_Kurtosis,Std_Mem40_Rolloff,Std_Mem40_Flux,Std_Mem40_MFCC_0,Std_Mem40_MFCC_1,Std_Mem40_MFCC_2,Std_Mem40_MFCC_3,Std_Mem40_MFCC_4,Std_Mem40_MFCC_5,Std_Mem40_MFCC_6,Std_Mem40_MFCC_7,Std_Mem40_MFCC_8,Std_Mem40_MFCC_9,Std_Mem40_MFCC_10,Std_Mem40_MFCC_11,Std_Mem40_MFCC_12,
# /WekaSink/wsink/string/labelNames = music,speech,
# /WekaSink/wsink/string/onObsNames = Mean_Mem40_Centroid,Mean_Mem40_Kurtosis,Mean_Mem40_Rolloff,Mean_Mem40_Flux,Mean_Mem40_MFCC_0,Mean_Mem40_MFCC_1,Mean_Mem40_MFCC_2,Mean_Mem40_MFCC_3,Mean_Mem40_MFCC_4,Mean_Mem40_MFCC_5,Mean_Mem40_MFCC_6,Mean_Mem40_MFCC_7,Mean_Mem40_MFCC_8,Mean_Mem40_MFCC_9,Mean_Mem40_MFCC_10,Mean_Mem40_MFCC_11,Mean_Mem40_MFCC_12,Std_Mem40_Centroid,Std_Mem40_Kurtosis,Std_Mem40_Rolloff,Std_Mem40_Flux,Std_Mem40_MFCC_0,Std_Mem40_MFCC_1,Std_Mem40_MFCC_2,Std_Mem40_MFCC_3,Std_Mem40_MFCC_4,Std_Mem40_MFCC_5,Std_Mem40_MFCC_6,Std_Mem40_MFCC_7,Std_Mem40_MFCC_8,Std_Mem40_MFCC_9,Std_Mem40_MFCC_10,Std_Mem40_MFCC_11,Std_Mem40_MFCC_12,

# Number of links = 0

# MarSystem
# Type = GaussianClassifier
# Name = gaussian

# MarControls = 15
# /GaussianClassifier/gaussian/bool/debug = 0
# /GaussianClassifier/gaussian/bool/done = 0
# /GaussianClassifier/gaussian/bool/mute = 0
# /GaussianClassifier/gaussian/natural/inObservations = 35
# /GaussianClassifier/gaussian/natural/inSamples = 1
# /GaussianClassifier/gaussian/natural/nLabels = 2
# /GaussianClassifier/gaussian/natural/onObservations = 2
# /GaussianClassifier/gaussian/natural/onSamples = 1
# /GaussianClassifier/gaussian/real/israte = 43.0664
# /GaussianClassifier/gaussian/real/osrate = 43.0664
# /GaussianClassifier/gaussian/realvec/covars = # MARSYAS realvec
# Size = 70


# type: matrix
# rows: 2
# columns: 35
672.412 2.25095e-13 89.5734 281401 0.0234071 0.232361 0.513397 0.931746 1.87386 3.14524 3.59299 3.2908 4.21811 4.22253 4.68603 4.00005 5.20308 2632.27 2.07525e-12 105.06 767515 0.188581 4.92236 11.9496 18.9602 34.6502 50.5243 57.9311 50.1834 43.1419 41.9469 37.4821 38.1671 35.1481 0 
547.156 2.29604e-13 56.7547 247029 0.0283737 0.465241 0.663504 1.21583 3.11321 3.43585 3.91215 5.90331 7.67155 8.40766 11.4518 7.94568 15.9784 471.991 1.95551e-12 79.3581 310892 0.116154 1.40777 5.23507 5.28905 9.50819 23.1527 22.2979 29.6301 45.2712 50.2724 57.3398 61.4123 59.81 0 

# Size = 70
# MARSYAS realvec

# /GaussianClassifier/gaussian/realvec/means = # MARSYAS realvec
# Size = 70


# type: matrix
# rows: 2
# columns: 35
0.0637275 6.95084e+06 0.152105 0.00823376 -70.6699 5.63441 0.0308119 1.93388 0.358039 0.65807 0.31291 0.395904 0.313465 0.338928 0.306103 0.316947 0.314715 0.0256223 2.66177e+06 0.0936317 0.00299289 3.16363 0.986479 0.751975 0.631946 0.554694 0.51454 0.498393 0.499871 0.500675 0.498131 0.495665 0.481479 0.481751 0 
0.0865025 7.80047e+06 0.278593 0.00814738 -76.5049 4.75225 0.152344 2.2623 0.601581 0.437161 0.507566 0.365622 0.475341 0.454826 0.38035 0.400381 0.447519 0.072411 3.55253e+06 0.240569 0.00401133 6.64523 2.13962 1.2967 1.33244 0.996558 0.77092 0.775156 0.711792 0.620098 0.587891 0.547877 0.519988 0.493575 0 

# Size = 70
# MARSYAS realvec

# /GaussianClassifier/gaussian/string/inObsNames = Mean_Mem40_Centroid,Mean_Mem40_Kurtosis,Mean_Mem40_Rolloff,Mean_Mem40_Flux,Mean_Mem40_MFCC_0,Mean_Mem40_MFCC_1,Mean_Mem40_MFCC_2,Mean_Mem40_MFCC_3,Mean_Mem40_MFCC_4,Mean_Mem40_MFCC_5,Mean_Mem40_MFCC_6,Mean_Mem40_MFCC_7,Mean_Mem40_MFCC_8,Mean_Mem40_MFCC_9,Mean_Mem40_MFCC_10,Mean_Mem40_MFCC_11,Mean_Mem40_MFCC_12,Std_Mem40_Centroid,Std_Mem40_Kurtosis,Std_Mem40_Rolloff,Std_Mem40_Flux,Std_Mem40_MFCC_0,Std_Mem40_MFCC_1,Std_Mem40_MFCC_2,Std_Mem40_MFCC_3,Std_Mem40_MFCC_4,Std_Mem40_MFCC_5,Std_Mem40_MFCC_6,Std_Mem40_MFCC_7,Std_Mem40_MFCC_8,Std_Mem40_MFCC_9,Std_Mem40_MFCC_10,Std_Mem40_MFCC_11,Std_Mem40_MFCC_12,
# /GaussianClassifier/gaussian/string/mode = predict
# /GaussianClassifier/gaussian/string/onObsNames = ,

# Number of links = 0

# MarSystem
# Type = Confidence
# Name = confidence

# MarControls = 14
# /Confidence/confidence/bool/debug = 0
# /Confidence/confidence/bool/mute = 0
# /Confidence/confidence/bool/print = 1
# /Confidence/confidence/natural/inObservations = 2
# /Confidence/confidence/natural/inSamples = 1
# /Confidence/confidence/natural/memSize = 40
# /Confidence/confidence/natural/nLabels = 2
# /Confidence/confidence/natural/onObservations = 2
# /Confidence/confidence/natural/onSamples = 1
# /Confidence/confidence/real/israte = 43.0664
# /Confidence/confidence/real/osrate = 43.0664
# /Confidence/confidence/string/inObsNames = ,
# /Confidence/confidence/string/labelNames = music,speech,
# /Confidence/confidence/string/onObsNames = ,

# Number of links = 0


