# MarSystemComposite
# Type = Series
# Name = featureNetwork

# MarControls = 23
# mrs_bool/active = 1
# Links = 0
# mrs_bool/debug = 0
# Links = 0
# mrs_bool/mute = 0
# Links = 0
# mrs_bool/hasData = 0
# Links = 1
# /Series/featureNetwork/SoundFileSource/src/mrs_bool/hasData
# mrs_bool/probe = 0
# Links = 0
# mrs_natural/inObservations = 1
# Links = 0
# mrs_natural/inSamples = 512
# Links = 0
# mrs_natural/nChannels = 1
# Links = 2
# /Series/featureNetwork/SoundFileSource/src/mrs_natural/nChannels
# /Series/featureNetwork/AudioSink/dest/mrs_natural/nChannels
# mrs_natural/onObservations = 2
# Links = 0
# mrs_natural/onSamples = 1
# Links = 0
# mrs_natural/pos = 661500
# Links = 1
# /Series/featureNetwork/SoundFileSource/src/mrs_natural/pos
# mrs_real/israte = 22050
# Links = 1
# /Series/featureNetwork/SoundFileSource/src/mrs_real/israte
# mrs_real/osrate = 43.0664
# Links = 0
# mrs_realvec/input0 = # MARSYAS mrs_realvec
# Size = 0


# type: matrix
# rows: 0
# columns: 0

# Size = 0
# MARSYAS mrs_realvec

# Links = 0
# mrs_realvec/input1 = # MARSYAS mrs_realvec
# Size = 0


# type: matrix
# rows: 0
# columns: 0

# Size = 0
# MARSYAS mrs_realvec

# Links = 0
# mrs_realvec/input2 = # MARSYAS mrs_realvec
# Size = 0


# type: matrix
# rows: 0
# columns: 0

# Size = 0
# MARSYAS mrs_realvec

# Links = 0
# mrs_realvec/input3 = # MARSYAS mrs_realvec
# Size = 0


# type: matrix
# rows: 0
# columns: 0

# Size = 0
# MARSYAS mrs_realvec

# Links = 0
# mrs_realvec/input4 = # MARSYAS mrs_realvec
# Size = 0


# type: matrix
# rows: 0
# columns: 0

# Size = 0
# MARSYAS mrs_realvec

# Links = 0
# mrs_realvec/input5 = # MARSYAS mrs_realvec
# Size = 0


# type: matrix
# rows: 0
# columns: 0

# Size = 0
# MARSYAS mrs_realvec

# Links = 0
# mrs_realvec/input6 = # MARSYAS mrs_realvec
# Size = 0


# type: matrix
# rows: 0
# columns: 0

# Size = 0
# MARSYAS mrs_realvec

# Links = 0
# mrs_string/filename = /home/gtzan/data/sound/music_speech/speech/voices.au
# Links = 1
# /Series/featureNetwork/SoundFileSource/src/mrs_string/filename
# mrs_string/inObsNames = audio,
# Links = 0
# mrs_string/onObsNames = ,
# Links = 0

# nComponents = 8

# MarSystem
# Type = SoundFileSource
# Name = src

# MarControls = 27
# mrs_bool/active = 1
# Links = 0
# mrs_bool/advance = 0
# Links = 0
# mrs_bool/debug = 0
# Links = 0
# mrs_bool/mute = 0
# Links = 0
# mrs_bool/hasData = 0
# Links = 1
# /Series/featureNetwork/mrs_bool/hasData
# mrs_bool/noteon = 0
# Links = 0
# mrs_bool/shuffle = 0
# Links = 0
# mrs_natural/cindex = 0
# Links = 0
# mrs_natural/inObservations = 1
# Links = 0
# mrs_natural/inSamples = 512
# Links = 0
# mrs_natural/loopPos = 0
# Links = 0
# mrs_natural/nChannels = 1
# Links = 1
# /Series/featureNetwork/mrs_natural/nChannels
# mrs_natural/numFiles = 1
# Links = 0
# mrs_natural/onObservations = 1
# Links = 0
# mrs_natural/onSamples = 512
# Links = 0
# mrs_natural/pos = 661500
# Links = 1
# /Series/featureNetwork/mrs_natural/pos
# mrs_natural/size = 661500
# Links = 0
# mrs_real/duration = -1
# Links = 0
# mrs_real/frequency = 0
# Links = 0
# mrs_real/israte = 22050
# Links = 1
# /Series/featureNetwork/mrs_real/israte
# mrs_real/osrate = 22050
# Links = 0
# mrs_real/repetitions = 1
# Links = 0
# mrs_string/allfilenames = ,
# Links = 0
# mrs_string/currentlyPlaying = daufile
# Links = 0
# mrs_string/filename = /home/gtzan/data/sound/music_speech/speech/voices.au
# Links = 1
# /Series/featureNetwork/mrs_string/filename
# mrs_string/inObsNames = audio,
# Links = 0
# mrs_string/onObsNames = audio,
# Links = 0

# MarSystem
# Type = AudioSink
# Name = dest

# MarControls = 13
# mrs_bool/active = 1
# Links = 0
# mrs_bool/debug = 0
# Links = 0
# mrs_bool/mute = 0
# Links = 0
# mrs_natural/bufferSize = 512
# Links = 0
# mrs_natural/inObservations = 1
# Links = 0
# mrs_natural/inSamples = 512
# Links = 0
# mrs_natural/nChannels = 1
# Links = 1
# /Series/featureNetwork/mrs_natural/nChannels
# mrs_natural/onObservations = 1
# Links = 0
# mrs_natural/onSamples = 512
# Links = 0
# mrs_real/israte = 22050
# Links = 0
# mrs_real/osrate = 22050
# Links = 0
# mrs_string/inObsNames = audio,
# Links = 0
# mrs_string/onObsNames = audio,
# Links = 0

# MarSystemComposite
# Type = Series
# Name = STFTMFCCextractor

# MarControls = 13
# mrs_bool/active = 1
# Links = 0
# mrs_bool/debug = 0
# Links = 0
# mrs_bool/mute = 0
# Links = 0
# mrs_bool/probe = 0
# Links = 0
# mrs_natural/inObservations = 1
# Links = 0
# mrs_natural/inSamples = 512
# Links = 0
# mrs_natural/onObservations = 17
# Links = 0
# mrs_natural/onSamples = 1
# Links = 0
# mrs_real/israte = 22050
# Links = 0
# mrs_real/osrate = 43.0664
# Links = 0
# mrs_realvec/input0 = # MARSYAS mrs_realvec
# Size = 0


# type: matrix
# rows: 0
# columns: 0

# Size = 0
# MARSYAS mrs_realvec

# Links = 0
# mrs_string/inObsNames = audio,
# Links = 0
# mrs_string/onObsNames = Centroid,Kurtosis,Rolloff,Flux,MFCC_0,MFCC_1,MFCC_2,MFCC_3,MFCC_4,MFCC_5,MFCC_6,MFCC_7,MFCC_8,MFCC_9,MFCC_10,MFCC_11,MFCC_12,
# Links = 0

# nComponents = 2

# MarSystemComposite
# Type = PowerSpectrumNet
# Name = powerSpect

# MarControls = 17
# mrs_bool/active = 1
# Links = 0
# mrs_bool/debug = 0
# Links = 0
# mrs_bool/mute = 0
# Links = 0
# mrs_bool/probe = 0
# Links = 0
# mrs_natural/WindowSize = 512
# Links = 1
# /Series/featureNetwork/Series/STFTMFCCextractor/PowerSpectrumNet/powerSpect/ShiftInput/si/mrs_natural/WindowSize
# mrs_natural/inObservations = 1
# Links = 0
# mrs_natural/inSamples = 512
# Links = 0
# mrs_natural/onObservations = 256
# Links = 0
# mrs_natural/onSamples = 1
# Links = 0
# mrs_real/cutoff = 1
# Links = 1
# /Series/featureNetwork/Series/STFTMFCCextractor/PowerSpectrumNet/powerSpect/Spectrum/spk/mrs_real/cutoff
# mrs_real/israte = 22050
# Links = 0
# mrs_real/osrate = 43.0664
# Links = 0
# mrs_realvec/input0 = # MARSYAS mrs_realvec
# Size = 0


# type: matrix
# rows: 0
# columns: 0

# Size = 0
# MARSYAS mrs_realvec

# Links = 0
# mrs_realvec/input1 = # MARSYAS mrs_realvec
# Size = 0


# type: matrix
# rows: 0
# columns: 0

# Size = 0
# MARSYAS mrs_realvec

# Links = 0
# mrs_realvec/input2 = # MARSYAS mrs_realvec
# Size = 0


# type: matrix
# rows: 0
# columns: 0

# Size = 0
# MARSYAS mrs_realvec

# Links = 0
# mrs_string/inObsNames = audio,
# Links = 0
# mrs_string/onObsNames = mbin_0,mbin_1,mbin_2,mbin_3,mbin_4,mbin_5,mbin_6,mbin_7,mbin_8,mbin_9,mbin_10,mbin_11,mbin_12,mbin_13,mbin_14,mbin_15,mbin_16,mbin_17,mbin_18,mbin_19,mbin_20,mbin_21,mbin_22,mbin_23,mbin_24,mbin_25,mbin_26,mbin_27,mbin_28,mbin_29,mbin_30,mbin_31,mbin_32,mbin_33,mbin_34,mbin_35,mbin_36,mbin_37,mbin_38,mbin_39,mbin_40,mbin_41,mbin_42,mbin_43,mbin_44,mbin_45,mbin_46,mbin_47,mbin_48,mbin_49,mbin_50,mbin_51,mbin_52,mbin_53,mbin_54,mbin_55,mbin_56,mbin_57,mbin_58,mbin_59,mbin_60,mbin_61,mbin_62,mbin_63,mbin_64,mbin_65,mbin_66,mbin_67,mbin_68,mbin_69,mbin_70,mbin_71,mbin_72,mbin_73,mbin_74,mbin_75,mbin_76,mbin_77,mbin_78,mbin_79,mbin_80,mbin_81,mbin_82,mbin_83,mbin_84,mbin_85,mbin_86,mbin_87,mbin_88,mbin_89,mbin_90,mbin_91,mbin_92,mbin_93,mbin_94,mbin_95,mbin_96,mbin_97,mbin_98,mbin_99,mbin_100,mbin_101,mbin_102,mbin_103,mbin_104,mbin_105,mbin_106,mbin_107,mbin_108,mbin_109,mbin_110,mbin_111,mbin_112,mbin_113,mbin_114,mbin_115,mbin_116,mbin_117,mbin_118,mbin_119,mbin_120,mbin_121,mbin_122,mbin_123,mbin_124,mbin_125,mbin_126,mbin_127,mbin_128,mbin_129,mbin_130,mbin_131,mbin_132,mbin_133,mbin_134,mbin_135,mbin_136,mbin_137,mbin_138,mbin_139,mbin_140,mbin_141,mbin_142,mbin_143,mbin_144,mbin_145,mbin_146,mbin_147,mbin_148,mbin_149,mbin_150,mbin_151,mbin_152,mbin_153,mbin_154,mbin_155,mbin_156,mbin_157,mbin_158,mbin_159,mbin_160,mbin_161,mbin_162,mbin_163,mbin_164,mbin_165,mbin_166,mbin_167,mbin_168,mbin_169,mbin_170,mbin_171,mbin_172,mbin_173,mbin_174,mbin_175,mbin_176,mbin_177,mbin_178,mbin_179,mbin_180,mbin_181,mbin_182,mbin_183,mbin_184,mbin_185,mbin_186,mbin_187,mbin_188,mbin_189,mbin_190,mbin_191,mbin_192,mbin_193,mbin_194,mbin_195,mbin_196,mbin_197,mbin_198,mbin_199,mbin_200,mbin_201,mbin_202,mbin_203,mbin_204,mbin_205,mbin_206,mbin_207,mbin_208,mbin_209,mbin_210,mbin_211,mbin_212,mbin_213,mbin_214,mbin_215,mbin_216,mbin_217,mbin_218,mbin_219,mbin_220,mbin_221,mbin_222,mbin_223,mbin_224,mbin_225,mbin_226,mbin_227,mbin_228,mbin_229,mbin_230,mbin_231,mbin_232,mbin_233,mbin_234,mbin_235,mbin_236,mbin_237,mbin_238,mbin_239,mbin_240,mbin_241,mbin_242,mbin_243,mbin_244,mbin_245,mbin_246,mbin_247,mbin_248,mbin_249,mbin_250,mbin_251,mbin_252,mbin_253,mbin_254,mbin_255,
# Links = 0

# nComponents = 4

# MarSystem
# Type = ShiftInput
# Name = si

# MarControls = 14
# mrs_bool/active = 1
# Links = 0
# mrs_bool/debug = 0
# Links = 0
# mrs_bool/mute = 0
# Links = 0
# mrs_bool/reset = 0
# Links = 0
# mrs_natural/Decimation = 512
# Links = 0
# mrs_natural/WindowSize = 512
# Links = 1
# /Series/featureNetwork/Series/STFTMFCCextractor/PowerSpectrumNet/powerSpect/mrs_natural/WindowSize
# mrs_natural/inObservations = 1
# Links = 0
# mrs_natural/inSamples = 512
# Links = 0
# mrs_natural/onObservations = 1
# Links = 0
# mrs_natural/onSamples = 512
# Links = 0
# mrs_real/israte = 22050
# Links = 0
# mrs_real/osrate = 22050
# Links = 0
# mrs_string/inObsNames = audio,
# Links = 0
# mrs_string/onObsNames = ,
# Links = 0

# MarSystem
# Type = Hamming
# Name = hamming

# MarControls = 11
# mrs_bool/active = 1
# Links = 0
# mrs_bool/debug = 0
# Links = 0
# mrs_bool/mute = 0
# Links = 0
# mrs_natural/inObservations = 1
# Links = 0
# mrs_natural/inSamples = 512
# Links = 0
# mrs_natural/onObservations = 1
# Links = 0
# mrs_natural/onSamples = 512
# Links = 0
# mrs_real/israte = 22050
# Links = 0
# mrs_real/osrate = 22050
# Links = 0
# mrs_string/inObsNames = ,
# Links = 0
# mrs_string/onObsNames = ,
# Links = 0

# MarSystem
# Type = Spectrum
# Name = spk

# MarControls = 13
# mrs_bool/active = 1
# Links = 0
# mrs_bool/debug = 0
# Links = 0
# mrs_bool/mute = 0
# Links = 0
# mrs_natural/inObservations = 1
# Links = 0
# mrs_natural/inSamples = 512
# Links = 0
# mrs_natural/onObservations = 512
# Links = 0
# mrs_natural/onSamples = 1
# Links = 0
# mrs_real/cutoff = 1
# Links = 1
# /Series/featureNetwork/Series/STFTMFCCextractor/PowerSpectrumNet/powerSpect/mrs_real/cutoff
# mrs_real/israte = 22050
# Links = 0
# mrs_real/lowcutoff = 0
# Links = 0
# mrs_real/osrate = 43.0664
# Links = 0
# mrs_string/inObsNames = ,
# Links = 0
# mrs_string/onObsNames = rbin_0,ibin_0,rbin_1,ibin_1,rbin_2,ibin_2,rbin_3,ibin_3,rbin_4,ibin_4,rbin_5,ibin_5,rbin_6,ibin_6,rbin_7,ibin_7,rbin_8,ibin_8,rbin_9,ibin_9,rbin_10,ibin_10,rbin_11,ibin_11,rbin_12,ibin_12,rbin_13,ibin_13,rbin_14,ibin_14,rbin_15,ibin_15,rbin_16,ibin_16,rbin_17,ibin_17,rbin_18,ibin_18,rbin_19,ibin_19,rbin_20,ibin_20,rbin_21,ibin_21,rbin_22,ibin_22,rbin_23,ibin_23,rbin_24,ibin_24,rbin_25,ibin_25,rbin_26,ibin_26,rbin_27,ibin_27,rbin_28,ibin_28,rbin_29,ibin_29,rbin_30,ibin_30,rbin_31,ibin_31,rbin_32,ibin_32,rbin_33,ibin_33,rbin_34,ibin_34,rbin_35,ibin_35,rbin_36,ibin_36,rbin_37,ibin_37,rbin_38,ibin_38,rbin_39,ibin_39,rbin_40,ibin_40,rbin_41,ibin_41,rbin_42,ibin_42,rbin_43,ibin_43,rbin_44,ibin_44,rbin_45,ibin_45,rbin_46,ibin_46,rbin_47,ibin_47,rbin_48,ibin_48,rbin_49,ibin_49,rbin_50,ibin_50,rbin_51,ibin_51,rbin_52,ibin_52,rbin_53,ibin_53,rbin_54,ibin_54,rbin_55,ibin_55,rbin_56,ibin_56,rbin_57,ibin_57,rbin_58,ibin_58,rbin_59,ibin_59,rbin_60,ibin_60,rbin_61,ibin_61,rbin_62,ibin_62,rbin_63,ibin_63,rbin_64,ibin_64,rbin_65,ibin_65,rbin_66,ibin_66,rbin_67,ibin_67,rbin_68,ibin_68,rbin_69,ibin_69,rbin_70,ibin_70,rbin_71,ibin_71,rbin_72,ibin_72,rbin_73,ibin_73,rbin_74,ibin_74,rbin_75,ibin_75,rbin_76,ibin_76,rbin_77,ibin_77,rbin_78,ibin_78,rbin_79,ibin_79,rbin_80,ibin_80,rbin_81,ibin_81,rbin_82,ibin_82,rbin_83,ibin_83,rbin_84,ibin_84,rbin_85,ibin_85,rbin_86,ibin_86,rbin_87,ibin_87,rbin_88,ibin_88,rbin_89,ibin_89,rbin_90,ibin_90,rbin_91,ibin_91,rbin_92,ibin_92,rbin_93,ibin_93,rbin_94,ibin_94,rbin_95,ibin_95,rbin_96,ibin_96,rbin_97,ibin_97,rbin_98,ibin_98,rbin_99,ibin_99,rbin_100,ibin_100,rbin_101,ibin_101,rbin_102,ibin_102,rbin_103,ibin_103,rbin_104,ibin_104,rbin_105,ibin_105,rbin_106,ibin_106,rbin_107,ibin_107,rbin_108,ibin_108,rbin_109,ibin_109,rbin_110,ibin_110,rbin_111,ibin_111,rbin_112,ibin_112,rbin_113,ibin_113,rbin_114,ibin_114,rbin_115,ibin_115,rbin_116,ibin_116,rbin_117,ibin_117,rbin_118,ibin_118,rbin_119,ibin_119,rbin_120,ibin_120,rbin_121,ibin_121,rbin_122,ibin_122,rbin_123,ibin_123,rbin_124,ibin_124,rbin_125,ibin_125,rbin_126,ibin_126,rbin_127,ibin_127,rbin_128,ibin_128,rbin_129,ibin_129,rbin_130,ibin_130,rbin_131,ibin_131,rbin_132,ibin_132,rbin_133,ibin_133,rbin_134,ibin_134,rbin_135,ibin_135,rbin_136,ibin_136,rbin_137,ibin_137,rbin_138,ibin_138,rbin_139,ibin_139,rbin_140,ibin_140,rbin_141,ibin_141,rbin_142,ibin_142,rbin_143,ibin_143,rbin_144,ibin_144,rbin_145,ibin_145,rbin_146,ibin_146,rbin_147,ibin_147,rbin_148,ibin_148,rbin_149,ibin_149,rbin_150,ibin_150,rbin_151,ibin_151,rbin_152,ibin_152,rbin_153,ibin_153,rbin_154,ibin_154,rbin_155,ibin_155,rbin_156,ibin_156,rbin_157,ibin_157,rbin_158,ibin_158,rbin_159,ibin_159,rbin_160,ibin_160,rbin_161,ibin_161,rbin_162,ibin_162,rbin_163,ibin_163,rbin_164,ibin_164,rbin_165,ibin_165,rbin_166,ibin_166,rbin_167,ibin_167,rbin_168,ibin_168,rbin_169,ibin_169,rbin_170,ibin_170,rbin_171,ibin_171,rbin_172,ibin_172,rbin_173,ibin_173,rbin_174,ibin_174,rbin_175,ibin_175,rbin_176,ibin_176,rbin_177,ibin_177,rbin_178,ibin_178,rbin_179,ibin_179,rbin_180,ibin_180,rbin_181,ibin_181,rbin_182,ibin_182,rbin_183,ibin_183,rbin_184,ibin_184,rbin_185,ibin_185,rbin_186,ibin_186,rbin_187,ibin_187,rbin_188,ibin_188,rbin_189,ibin_189,rbin_190,ibin_190,rbin_191,ibin_191,rbin_192,ibin_192,rbin_193,ibin_193,rbin_194,ibin_194,rbin_195,ibin_195,rbin_196,ibin_196,rbin_197,ibin_197,rbin_198,ibin_198,rbin_199,ibin_199,rbin_200,ibin_200,rbin_201,ibin_201,rbin_202,ibin_202,rbin_203,ibin_203,rbin_204,ibin_204,rbin_205,ibin_205,rbin_206,ibin_206,rbin_207,ibin_207,rbin_208,ibin_208,rbin_209,ibin_209,rbin_210,ibin_210,rbin_211,ibin_211,rbin_212,ibin_212,rbin_213,ibin_213,rbin_214,ibin_214,rbin_215,ibin_215,rbin_216,ibin_216,rbin_217,ibin_217,rbin_218,ibin_218,rbin_219,ibin_219,rbin_220,ibin_220,rbin_221,ibin_221,rbin_222,ibin_222,rbin_223,ibin_223,rbin_224,ibin_224,rbin_225,ibin_225,rbin_226,ibin_226,rbin_227,ibin_227,rbin_228,ibin_228,rbin_229,ibin_229,rbin_230,ibin_230,rbin_231,ibin_231,rbin_232,ibin_232,rbin_233,ibin_233,rbin_234,ibin_234,rbin_235,ibin_235,rbin_236,ibin_236,rbin_237,ibin_237,rbin_238,ibin_238,rbin_239,ibin_239,rbin_240,ibin_240,rbin_241,ibin_241,rbin_242,ibin_242,rbin_243,ibin_243,rbin_244,ibin_244,rbin_245,ibin_245,rbin_246,ibin_246,rbin_247,ibin_247,rbin_248,ibin_248,rbin_249,ibin_249,rbin_250,ibin_250,rbin_251,ibin_251,rbin_252,ibin_252,rbin_253,ibin_253,rbin_254,ibin_254,rbin_255,ibin_255,
# Links = 0

# MarSystem
# Type = PowerSpectrum
# Name = pspk

# MarControls = 12
# mrs_bool/active = 1
# Links = 0
# mrs_bool/debug = 0
# Links = 0
# mrs_bool/mute = 0
# Links = 0
# mrs_natural/inObservations = 512
# Links = 0
# mrs_natural/inSamples = 1
# Links = 0
# mrs_natural/onObservations = 256
# Links = 0
# mrs_natural/onSamples = 1
# Links = 0
# mrs_real/israte = 43.0664
# Links = 0
# mrs_real/osrate = 43.0664
# Links = 0
# mrs_string/inObsNames = rbin_0,ibin_0,rbin_1,ibin_1,rbin_2,ibin_2,rbin_3,ibin_3,rbin_4,ibin_4,rbin_5,ibin_5,rbin_6,ibin_6,rbin_7,ibin_7,rbin_8,ibin_8,rbin_9,ibin_9,rbin_10,ibin_10,rbin_11,ibin_11,rbin_12,ibin_12,rbin_13,ibin_13,rbin_14,ibin_14,rbin_15,ibin_15,rbin_16,ibin_16,rbin_17,ibin_17,rbin_18,ibin_18,rbin_19,ibin_19,rbin_20,ibin_20,rbin_21,ibin_21,rbin_22,ibin_22,rbin_23,ibin_23,rbin_24,ibin_24,rbin_25,ibin_25,rbin_26,ibin_26,rbin_27,ibin_27,rbin_28,ibin_28,rbin_29,ibin_29,rbin_30,ibin_30,rbin_31,ibin_31,rbin_32,ibin_32,rbin_33,ibin_33,rbin_34,ibin_34,rbin_35,ibin_35,rbin_36,ibin_36,rbin_37,ibin_37,rbin_38,ibin_38,rbin_39,ibin_39,rbin_40,ibin_40,rbin_41,ibin_41,rbin_42,ibin_42,rbin_43,ibin_43,rbin_44,ibin_44,rbin_45,ibin_45,rbin_46,ibin_46,rbin_47,ibin_47,rbin_48,ibin_48,rbin_49,ibin_49,rbin_50,ibin_50,rbin_51,ibin_51,rbin_52,ibin_52,rbin_53,ibin_53,rbin_54,ibin_54,rbin_55,ibin_55,rbin_56,ibin_56,rbin_57,ibin_57,rbin_58,ibin_58,rbin_59,ibin_59,rbin_60,ibin_60,rbin_61,ibin_61,rbin_62,ibin_62,rbin_63,ibin_63,rbin_64,ibin_64,rbin_65,ibin_65,rbin_66,ibin_66,rbin_67,ibin_67,rbin_68,ibin_68,rbin_69,ibin_69,rbin_70,ibin_70,rbin_71,ibin_71,rbin_72,ibin_72,rbin_73,ibin_73,rbin_74,ibin_74,rbin_75,ibin_75,rbin_76,ibin_76,rbin_77,ibin_77,rbin_78,ibin_78,rbin_79,ibin_79,rbin_80,ibin_80,rbin_81,ibin_81,rbin_82,ibin_82,rbin_83,ibin_83,rbin_84,ibin_84,rbin_85,ibin_85,rbin_86,ibin_86,rbin_87,ibin_87,rbin_88,ibin_88,rbin_89,ibin_89,rbin_90,ibin_90,rbin_91,ibin_91,rbin_92,ibin_92,rbin_93,ibin_93,rbin_94,ibin_94,rbin_95,ibin_95,rbin_96,ibin_96,rbin_97,ibin_97,rbin_98,ibin_98,rbin_99,ibin_99,rbin_100,ibin_100,rbin_101,ibin_101,rbin_102,ibin_102,rbin_103,ibin_103,rbin_104,ibin_104,rbin_105,ibin_105,rbin_106,ibin_106,rbin_107,ibin_107,rbin_108,ibin_108,rbin_109,ibin_109,rbin_110,ibin_110,rbin_111,ibin_111,rbin_112,ibin_112,rbin_113,ibin_113,rbin_114,ibin_114,rbin_115,ibin_115,rbin_116,ibin_116,rbin_117,ibin_117,rbin_118,ibin_118,rbin_119,ibin_119,rbin_120,ibin_120,rbin_121,ibin_121,rbin_122,ibin_122,rbin_123,ibin_123,rbin_124,ibin_124,rbin_125,ibin_125,rbin_126,ibin_126,rbin_127,ibin_127,rbin_128,ibin_128,rbin_129,ibin_129,rbin_130,ibin_130,rbin_131,ibin_131,rbin_132,ibin_132,rbin_133,ibin_133,rbin_134,ibin_134,rbin_135,ibin_135,rbin_136,ibin_136,rbin_137,ibin_137,rbin_138,ibin_138,rbin_139,ibin_139,rbin_140,ibin_140,rbin_141,ibin_141,rbin_142,ibin_142,rbin_143,ibin_143,rbin_144,ibin_144,rbin_145,ibin_145,rbin_146,ibin_146,rbin_147,ibin_147,rbin_148,ibin_148,rbin_149,ibin_149,rbin_150,ibin_150,rbin_151,ibin_151,rbin_152,ibin_152,rbin_153,ibin_153,rbin_154,ibin_154,rbin_155,ibin_155,rbin_156,ibin_156,rbin_157,ibin_157,rbin_158,ibin_158,rbin_159,ibin_159,rbin_160,ibin_160,rbin_161,ibin_161,rbin_162,ibin_162,rbin_163,ibin_163,rbin_164,ibin_164,rbin_165,ibin_165,rbin_166,ibin_166,rbin_167,ibin_167,rbin_168,ibin_168,rbin_169,ibin_169,rbin_170,ibin_170,rbin_171,ibin_171,rbin_172,ibin_172,rbin_173,ibin_173,rbin_174,ibin_174,rbin_175,ibin_175,rbin_176,ibin_176,rbin_177,ibin_177,rbin_178,ibin_178,rbin_179,ibin_179,rbin_180,ibin_180,rbin_181,ibin_181,rbin_182,ibin_182,rbin_183,ibin_183,rbin_184,ibin_184,rbin_185,ibin_185,rbin_186,ibin_186,rbin_187,ibin_187,rbin_188,ibin_188,rbin_189,ibin_189,rbin_190,ibin_190,rbin_191,ibin_191,rbin_192,ibin_192,rbin_193,ibin_193,rbin_194,ibin_194,rbin_195,ibin_195,rbin_196,ibin_196,rbin_197,ibin_197,rbin_198,ibin_198,rbin_199,ibin_199,rbin_200,ibin_200,rbin_201,ibin_201,rbin_202,ibin_202,rbin_203,ibin_203,rbin_204,ibin_204,rbin_205,ibin_205,rbin_206,ibin_206,rbin_207,ibin_207,rbin_208,ibin_208,rbin_209,ibin_209,rbin_210,ibin_210,rbin_211,ibin_211,rbin_212,ibin_212,rbin_213,ibin_213,rbin_214,ibin_214,rbin_215,ibin_215,rbin_216,ibin_216,rbin_217,ibin_217,rbin_218,ibin_218,rbin_219,ibin_219,rbin_220,ibin_220,rbin_221,ibin_221,rbin_222,ibin_222,rbin_223,ibin_223,rbin_224,ibin_224,rbin_225,ibin_225,rbin_226,ibin_226,rbin_227,ibin_227,rbin_228,ibin_228,rbin_229,ibin_229,rbin_230,ibin_230,rbin_231,ibin_231,rbin_232,ibin_232,rbin_233,ibin_233,rbin_234,ibin_234,rbin_235,ibin_235,rbin_236,ibin_236,rbin_237,ibin_237,rbin_238,ibin_238,rbin_239,ibin_239,rbin_240,ibin_240,rbin_241,ibin_241,rbin_242,ibin_242,rbin_243,ibin_243,rbin_244,ibin_244,rbin_245,ibin_245,rbin_246,ibin_246,rbin_247,ibin_247,rbin_248,ibin_248,rbin_249,ibin_249,rbin_250,ibin_250,rbin_251,ibin_251,rbin_252,ibin_252,rbin_253,ibin_253,rbin_254,ibin_254,rbin_255,ibin_255,
# Links = 0
# mrs_string/onObsNames = mbin_0,mbin_1,mbin_2,mbin_3,mbin_4,mbin_5,mbin_6,mbin_7,mbin_8,mbin_9,mbin_10,mbin_11,mbin_12,mbin_13,mbin_14,mbin_15,mbin_16,mbin_17,mbin_18,mbin_19,mbin_20,mbin_21,mbin_22,mbin_23,mbin_24,mbin_25,mbin_26,mbin_27,mbin_28,mbin_29,mbin_30,mbin_31,mbin_32,mbin_33,mbin_34,mbin_35,mbin_36,mbin_37,mbin_38,mbin_39,mbin_40,mbin_41,mbin_42,mbin_43,mbin_44,mbin_45,mbin_46,mbin_47,mbin_48,mbin_49,mbin_50,mbin_51,mbin_52,mbin_53,mbin_54,mbin_55,mbin_56,mbin_57,mbin_58,mbin_59,mbin_60,mbin_61,mbin_62,mbin_63,mbin_64,mbin_65,mbin_66,mbin_67,mbin_68,mbin_69,mbin_70,mbin_71,mbin_72,mbin_73,mbin_74,mbin_75,mbin_76,mbin_77,mbin_78,mbin_79,mbin_80,mbin_81,mbin_82,mbin_83,mbin_84,mbin_85,mbin_86,mbin_87,mbin_88,mbin_89,mbin_90,mbin_91,mbin_92,mbin_93,mbin_94,mbin_95,mbin_96,mbin_97,mbin_98,mbin_99,mbin_100,mbin_101,mbin_102,mbin_103,mbin_104,mbin_105,mbin_106,mbin_107,mbin_108,mbin_109,mbin_110,mbin_111,mbin_112,mbin_113,mbin_114,mbin_115,mbin_116,mbin_117,mbin_118,mbin_119,mbin_120,mbin_121,mbin_122,mbin_123,mbin_124,mbin_125,mbin_126,mbin_127,mbin_128,mbin_129,mbin_130,mbin_131,mbin_132,mbin_133,mbin_134,mbin_135,mbin_136,mbin_137,mbin_138,mbin_139,mbin_140,mbin_141,mbin_142,mbin_143,mbin_144,mbin_145,mbin_146,mbin_147,mbin_148,mbin_149,mbin_150,mbin_151,mbin_152,mbin_153,mbin_154,mbin_155,mbin_156,mbin_157,mbin_158,mbin_159,mbin_160,mbin_161,mbin_162,mbin_163,mbin_164,mbin_165,mbin_166,mbin_167,mbin_168,mbin_169,mbin_170,mbin_171,mbin_172,mbin_173,mbin_174,mbin_175,mbin_176,mbin_177,mbin_178,mbin_179,mbin_180,mbin_181,mbin_182,mbin_183,mbin_184,mbin_185,mbin_186,mbin_187,mbin_188,mbin_189,mbin_190,mbin_191,mbin_192,mbin_193,mbin_194,mbin_195,mbin_196,mbin_197,mbin_198,mbin_199,mbin_200,mbin_201,mbin_202,mbin_203,mbin_204,mbin_205,mbin_206,mbin_207,mbin_208,mbin_209,mbin_210,mbin_211,mbin_212,mbin_213,mbin_214,mbin_215,mbin_216,mbin_217,mbin_218,mbin_219,mbin_220,mbin_221,mbin_222,mbin_223,mbin_224,mbin_225,mbin_226,mbin_227,mbin_228,mbin_229,mbin_230,mbin_231,mbin_232,mbin_233,mbin_234,mbin_235,mbin_236,mbin_237,mbin_238,mbin_239,mbin_240,mbin_241,mbin_242,mbin_243,mbin_244,mbin_245,mbin_246,mbin_247,mbin_248,mbin_249,mbin_250,mbin_251,mbin_252,mbin_253,mbin_254,mbin_255,
# Links = 0
# mrs_string/spectrumType = power
# Links = 0


# MarSystemComposite
# Type = Fanout
# Name = spectrumFeatures

# MarControls = 13
# mrs_bool/active = 1
# Links = 0
# mrs_bool/debug = 0
# Links = 0
# mrs_bool/mute = 0
# Links = 0
# mrs_bool/probe = 0
# Links = 0
# mrs_natural/disable = -1
# Links = 0
# mrs_natural/inObservations = 256
# Links = 0
# mrs_natural/inSamples = 1
# Links = 0
# mrs_natural/onObservations = 17
# Links = 0
# mrs_natural/onSamples = 1
# Links = 0
# mrs_real/israte = 43.0664
# Links = 0
# mrs_real/osrate = 43.0664
# Links = 0
# mrs_string/inObsNames = mbin_0,mbin_1,mbin_2,mbin_3,mbin_4,mbin_5,mbin_6,mbin_7,mbin_8,mbin_9,mbin_10,mbin_11,mbin_12,mbin_13,mbin_14,mbin_15,mbin_16,mbin_17,mbin_18,mbin_19,mbin_20,mbin_21,mbin_22,mbin_23,mbin_24,mbin_25,mbin_26,mbin_27,mbin_28,mbin_29,mbin_30,mbin_31,mbin_32,mbin_33,mbin_34,mbin_35,mbin_36,mbin_37,mbin_38,mbin_39,mbin_40,mbin_41,mbin_42,mbin_43,mbin_44,mbin_45,mbin_46,mbin_47,mbin_48,mbin_49,mbin_50,mbin_51,mbin_52,mbin_53,mbin_54,mbin_55,mbin_56,mbin_57,mbin_58,mbin_59,mbin_60,mbin_61,mbin_62,mbin_63,mbin_64,mbin_65,mbin_66,mbin_67,mbin_68,mbin_69,mbin_70,mbin_71,mbin_72,mbin_73,mbin_74,mbin_75,mbin_76,mbin_77,mbin_78,mbin_79,mbin_80,mbin_81,mbin_82,mbin_83,mbin_84,mbin_85,mbin_86,mbin_87,mbin_88,mbin_89,mbin_90,mbin_91,mbin_92,mbin_93,mbin_94,mbin_95,mbin_96,mbin_97,mbin_98,mbin_99,mbin_100,mbin_101,mbin_102,mbin_103,mbin_104,mbin_105,mbin_106,mbin_107,mbin_108,mbin_109,mbin_110,mbin_111,mbin_112,mbin_113,mbin_114,mbin_115,mbin_116,mbin_117,mbin_118,mbin_119,mbin_120,mbin_121,mbin_122,mbin_123,mbin_124,mbin_125,mbin_126,mbin_127,mbin_128,mbin_129,mbin_130,mbin_131,mbin_132,mbin_133,mbin_134,mbin_135,mbin_136,mbin_137,mbin_138,mbin_139,mbin_140,mbin_141,mbin_142,mbin_143,mbin_144,mbin_145,mbin_146,mbin_147,mbin_148,mbin_149,mbin_150,mbin_151,mbin_152,mbin_153,mbin_154,mbin_155,mbin_156,mbin_157,mbin_158,mbin_159,mbin_160,mbin_161,mbin_162,mbin_163,mbin_164,mbin_165,mbin_166,mbin_167,mbin_168,mbin_169,mbin_170,mbin_171,mbin_172,mbin_173,mbin_174,mbin_175,mbin_176,mbin_177,mbin_178,mbin_179,mbin_180,mbin_181,mbin_182,mbin_183,mbin_184,mbin_185,mbin_186,mbin_187,mbin_188,mbin_189,mbin_190,mbin_191,mbin_192,mbin_193,mbin_194,mbin_195,mbin_196,mbin_197,mbin_198,mbin_199,mbin_200,mbin_201,mbin_202,mbin_203,mbin_204,mbin_205,mbin_206,mbin_207,mbin_208,mbin_209,mbin_210,mbin_211,mbin_212,mbin_213,mbin_214,mbin_215,mbin_216,mbin_217,mbin_218,mbin_219,mbin_220,mbin_221,mbin_222,mbin_223,mbin_224,mbin_225,mbin_226,mbin_227,mbin_228,mbin_229,mbin_230,mbin_231,mbin_232,mbin_233,mbin_234,mbin_235,mbin_236,mbin_237,mbin_238,mbin_239,mbin_240,mbin_241,mbin_242,mbin_243,mbin_244,mbin_245,mbin_246,mbin_247,mbin_248,mbin_249,mbin_250,mbin_251,mbin_252,mbin_253,mbin_254,mbin_255,
# Links = 0
# mrs_string/onObsNames = Centroid,Kurtosis,Rolloff,Flux,MFCC_0,MFCC_1,MFCC_2,MFCC_3,MFCC_4,MFCC_5,MFCC_6,MFCC_7,MFCC_8,MFCC_9,MFCC_10,MFCC_11,MFCC_12,
# Links = 0

# nComponents = 5

# MarSystem
# Type = Centroid
# Name = cntrd

# MarControls = 11
# mrs_bool/active = 1
# Links = 0
# mrs_bool/debug = 0
# Links = 0
# mrs_bool/mute = 0
# Links = 0
# mrs_natural/inObservations = 256
# Links = 0
# mrs_natural/inSamples = 1
# Links = 0
# mrs_natural/onObservations = 1
# Links = 0
# mrs_natural/onSamples = 1
# Links = 0
# mrs_real/israte = 43.0664
# Links = 0
# mrs_real/osrate = 43.0664
# Links = 0
# mrs_string/inObsNames = mbin_0,mbin_1,mbin_2,mbin_3,mbin_4,mbin_5,mbin_6,mbin_7,mbin_8,mbin_9,mbin_10,mbin_11,mbin_12,mbin_13,mbin_14,mbin_15,mbin_16,mbin_17,mbin_18,mbin_19,mbin_20,mbin_21,mbin_22,mbin_23,mbin_24,mbin_25,mbin_26,mbin_27,mbin_28,mbin_29,mbin_30,mbin_31,mbin_32,mbin_33,mbin_34,mbin_35,mbin_36,mbin_37,mbin_38,mbin_39,mbin_40,mbin_41,mbin_42,mbin_43,mbin_44,mbin_45,mbin_46,mbin_47,mbin_48,mbin_49,mbin_50,mbin_51,mbin_52,mbin_53,mbin_54,mbin_55,mbin_56,mbin_57,mbin_58,mbin_59,mbin_60,mbin_61,mbin_62,mbin_63,mbin_64,mbin_65,mbin_66,mbin_67,mbin_68,mbin_69,mbin_70,mbin_71,mbin_72,mbin_73,mbin_74,mbin_75,mbin_76,mbin_77,mbin_78,mbin_79,mbin_80,mbin_81,mbin_82,mbin_83,mbin_84,mbin_85,mbin_86,mbin_87,mbin_88,mbin_89,mbin_90,mbin_91,mbin_92,mbin_93,mbin_94,mbin_95,mbin_96,mbin_97,mbin_98,mbin_99,mbin_100,mbin_101,mbin_102,mbin_103,mbin_104,mbin_105,mbin_106,mbin_107,mbin_108,mbin_109,mbin_110,mbin_111,mbin_112,mbin_113,mbin_114,mbin_115,mbin_116,mbin_117,mbin_118,mbin_119,mbin_120,mbin_121,mbin_122,mbin_123,mbin_124,mbin_125,mbin_126,mbin_127,mbin_128,mbin_129,mbin_130,mbin_131,mbin_132,mbin_133,mbin_134,mbin_135,mbin_136,mbin_137,mbin_138,mbin_139,mbin_140,mbin_141,mbin_142,mbin_143,mbin_144,mbin_145,mbin_146,mbin_147,mbin_148,mbin_149,mbin_150,mbin_151,mbin_152,mbin_153,mbin_154,mbin_155,mbin_156,mbin_157,mbin_158,mbin_159,mbin_160,mbin_161,mbin_162,mbin_163,mbin_164,mbin_165,mbin_166,mbin_167,mbin_168,mbin_169,mbin_170,mbin_171,mbin_172,mbin_173,mbin_174,mbin_175,mbin_176,mbin_177,mbin_178,mbin_179,mbin_180,mbin_181,mbin_182,mbin_183,mbin_184,mbin_185,mbin_186,mbin_187,mbin_188,mbin_189,mbin_190,mbin_191,mbin_192,mbin_193,mbin_194,mbin_195,mbin_196,mbin_197,mbin_198,mbin_199,mbin_200,mbin_201,mbin_202,mbin_203,mbin_204,mbin_205,mbin_206,mbin_207,mbin_208,mbin_209,mbin_210,mbin_211,mbin_212,mbin_213,mbin_214,mbin_215,mbin_216,mbin_217,mbin_218,mbin_219,mbin_220,mbin_221,mbin_222,mbin_223,mbin_224,mbin_225,mbin_226,mbin_227,mbin_228,mbin_229,mbin_230,mbin_231,mbin_232,mbin_233,mbin_234,mbin_235,mbin_236,mbin_237,mbin_238,mbin_239,mbin_240,mbin_241,mbin_242,mbin_243,mbin_244,mbin_245,mbin_246,mbin_247,mbin_248,mbin_249,mbin_250,mbin_251,mbin_252,mbin_253,mbin_254,mbin_255,
# Links = 0
# mrs_string/onObsNames = Centroid,
# Links = 0

# MarSystem
# Type = Kurtosis
# Name = krt

# MarControls = 11
# mrs_bool/active = 1
# Links = 0
# mrs_bool/debug = 0
# Links = 0
# mrs_bool/mute = 0
# Links = 0
# mrs_natural/inObservations = 256
# Links = 0
# mrs_natural/inSamples = 1
# Links = 0
# mrs_natural/onObservations = 1
# Links = 0
# mrs_natural/onSamples = 1
# Links = 0
# mrs_real/israte = 43.0664
# Links = 0
# mrs_real/osrate = 43.0664
# Links = 0
# mrs_string/inObsNames = mbin_0,mbin_1,mbin_2,mbin_3,mbin_4,mbin_5,mbin_6,mbin_7,mbin_8,mbin_9,mbin_10,mbin_11,mbin_12,mbin_13,mbin_14,mbin_15,mbin_16,mbin_17,mbin_18,mbin_19,mbin_20,mbin_21,mbin_22,mbin_23,mbin_24,mbin_25,mbin_26,mbin_27,mbin_28,mbin_29,mbin_30,mbin_31,mbin_32,mbin_33,mbin_34,mbin_35,mbin_36,mbin_37,mbin_38,mbin_39,mbin_40,mbin_41,mbin_42,mbin_43,mbin_44,mbin_45,mbin_46,mbin_47,mbin_48,mbin_49,mbin_50,mbin_51,mbin_52,mbin_53,mbin_54,mbin_55,mbin_56,mbin_57,mbin_58,mbin_59,mbin_60,mbin_61,mbin_62,mbin_63,mbin_64,mbin_65,mbin_66,mbin_67,mbin_68,mbin_69,mbin_70,mbin_71,mbin_72,mbin_73,mbin_74,mbin_75,mbin_76,mbin_77,mbin_78,mbin_79,mbin_80,mbin_81,mbin_82,mbin_83,mbin_84,mbin_85,mbin_86,mbin_87,mbin_88,mbin_89,mbin_90,mbin_91,mbin_92,mbin_93,mbin_94,mbin_95,mbin_96,mbin_97,mbin_98,mbin_99,mbin_100,mbin_101,mbin_102,mbin_103,mbin_104,mbin_105,mbin_106,mbin_107,mbin_108,mbin_109,mbin_110,mbin_111,mbin_112,mbin_113,mbin_114,mbin_115,mbin_116,mbin_117,mbin_118,mbin_119,mbin_120,mbin_121,mbin_122,mbin_123,mbin_124,mbin_125,mbin_126,mbin_127,mbin_128,mbin_129,mbin_130,mbin_131,mbin_132,mbin_133,mbin_134,mbin_135,mbin_136,mbin_137,mbin_138,mbin_139,mbin_140,mbin_141,mbin_142,mbin_143,mbin_144,mbin_145,mbin_146,mbin_147,mbin_148,mbin_149,mbin_150,mbin_151,mbin_152,mbin_153,mbin_154,mbin_155,mbin_156,mbin_157,mbin_158,mbin_159,mbin_160,mbin_161,mbin_162,mbin_163,mbin_164,mbin_165,mbin_166,mbin_167,mbin_168,mbin_169,mbin_170,mbin_171,mbin_172,mbin_173,mbin_174,mbin_175,mbin_176,mbin_177,mbin_178,mbin_179,mbin_180,mbin_181,mbin_182,mbin_183,mbin_184,mbin_185,mbin_186,mbin_187,mbin_188,mbin_189,mbin_190,mbin_191,mbin_192,mbin_193,mbin_194,mbin_195,mbin_196,mbin_197,mbin_198,mbin_199,mbin_200,mbin_201,mbin_202,mbin_203,mbin_204,mbin_205,mbin_206,mbin_207,mbin_208,mbin_209,mbin_210,mbin_211,mbin_212,mbin_213,mbin_214,mbin_215,mbin_216,mbin_217,mbin_218,mbin_219,mbin_220,mbin_221,mbin_222,mbin_223,mbin_224,mbin_225,mbin_226,mbin_227,mbin_228,mbin_229,mbin_230,mbin_231,mbin_232,mbin_233,mbin_234,mbin_235,mbin_236,mbin_237,mbin_238,mbin_239,mbin_240,mbin_241,mbin_242,mbin_243,mbin_244,mbin_245,mbin_246,mbin_247,mbin_248,mbin_249,mbin_250,mbin_251,mbin_252,mbin_253,mbin_254,mbin_255,
# Links = 0
# mrs_string/onObsNames = Kurtosis,
# Links = 0

# MarSystem
# Type = Rolloff
# Name = rlf

# MarControls = 12
# mrs_bool/active = 1
# Links = 0
# mrs_bool/debug = 0
# Links = 0
# mrs_bool/mute = 0
# Links = 0
# mrs_natural/inObservations = 256
# Links = 0
# mrs_natural/inSamples = 1
# Links = 0
# mrs_natural/onObservations = 1
# Links = 0
# mrs_natural/onSamples = 1
# Links = 0
# mrs_real/israte = 43.0664
# Links = 0
# mrs_real/osrate = 43.0664
# Links = 0
# mrs_real/percentage = 0.9
# Links = 0
# mrs_string/inObsNames = mbin_0,mbin_1,mbin_2,mbin_3,mbin_4,mbin_5,mbin_6,mbin_7,mbin_8,mbin_9,mbin_10,mbin_11,mbin_12,mbin_13,mbin_14,mbin_15,mbin_16,mbin_17,mbin_18,mbin_19,mbin_20,mbin_21,mbin_22,mbin_23,mbin_24,mbin_25,mbin_26,mbin_27,mbin_28,mbin_29,mbin_30,mbin_31,mbin_32,mbin_33,mbin_34,mbin_35,mbin_36,mbin_37,mbin_38,mbin_39,mbin_40,mbin_41,mbin_42,mbin_43,mbin_44,mbin_45,mbin_46,mbin_47,mbin_48,mbin_49,mbin_50,mbin_51,mbin_52,mbin_53,mbin_54,mbin_55,mbin_56,mbin_57,mbin_58,mbin_59,mbin_60,mbin_61,mbin_62,mbin_63,mbin_64,mbin_65,mbin_66,mbin_67,mbin_68,mbin_69,mbin_70,mbin_71,mbin_72,mbin_73,mbin_74,mbin_75,mbin_76,mbin_77,mbin_78,mbin_79,mbin_80,mbin_81,mbin_82,mbin_83,mbin_84,mbin_85,mbin_86,mbin_87,mbin_88,mbin_89,mbin_90,mbin_91,mbin_92,mbin_93,mbin_94,mbin_95,mbin_96,mbin_97,mbin_98,mbin_99,mbin_100,mbin_101,mbin_102,mbin_103,mbin_104,mbin_105,mbin_106,mbin_107,mbin_108,mbin_109,mbin_110,mbin_111,mbin_112,mbin_113,mbin_114,mbin_115,mbin_116,mbin_117,mbin_118,mbin_119,mbin_120,mbin_121,mbin_122,mbin_123,mbin_124,mbin_125,mbin_126,mbin_127,mbin_128,mbin_129,mbin_130,mbin_131,mbin_132,mbin_133,mbin_134,mbin_135,mbin_136,mbin_137,mbin_138,mbin_139,mbin_140,mbin_141,mbin_142,mbin_143,mbin_144,mbin_145,mbin_146,mbin_147,mbin_148,mbin_149,mbin_150,mbin_151,mbin_152,mbin_153,mbin_154,mbin_155,mbin_156,mbin_157,mbin_158,mbin_159,mbin_160,mbin_161,mbin_162,mbin_163,mbin_164,mbin_165,mbin_166,mbin_167,mbin_168,mbin_169,mbin_170,mbin_171,mbin_172,mbin_173,mbin_174,mbin_175,mbin_176,mbin_177,mbin_178,mbin_179,mbin_180,mbin_181,mbin_182,mbin_183,mbin_184,mbin_185,mbin_186,mbin_187,mbin_188,mbin_189,mbin_190,mbin_191,mbin_192,mbin_193,mbin_194,mbin_195,mbin_196,mbin_197,mbin_198,mbin_199,mbin_200,mbin_201,mbin_202,mbin_203,mbin_204,mbin_205,mbin_206,mbin_207,mbin_208,mbin_209,mbin_210,mbin_211,mbin_212,mbin_213,mbin_214,mbin_215,mbin_216,mbin_217,mbin_218,mbin_219,mbin_220,mbin_221,mbin_222,mbin_223,mbin_224,mbin_225,mbin_226,mbin_227,mbin_228,mbin_229,mbin_230,mbin_231,mbin_232,mbin_233,mbin_234,mbin_235,mbin_236,mbin_237,mbin_238,mbin_239,mbin_240,mbin_241,mbin_242,mbin_243,mbin_244,mbin_245,mbin_246,mbin_247,mbin_248,mbin_249,mbin_250,mbin_251,mbin_252,mbin_253,mbin_254,mbin_255,
# Links = 0
# mrs_string/onObsNames = Rolloff,
# Links = 0

# MarSystem
# Type = Flux
# Name = flux

# MarControls = 11
# mrs_bool/active = 1
# Links = 0
# mrs_bool/debug = 0
# Links = 0
# mrs_bool/mute = 0
# Links = 0
# mrs_natural/inObservations = 256
# Links = 0
# mrs_natural/inSamples = 1
# Links = 0
# mrs_natural/onObservations = 1
# Links = 0
# mrs_natural/onSamples = 1
# Links = 0
# mrs_real/israte = 43.0664
# Links = 0
# mrs_real/osrate = 43.0664
# Links = 0
# mrs_string/inObsNames = mbin_0,mbin_1,mbin_2,mbin_3,mbin_4,mbin_5,mbin_6,mbin_7,mbin_8,mbin_9,mbin_10,mbin_11,mbin_12,mbin_13,mbin_14,mbin_15,mbin_16,mbin_17,mbin_18,mbin_19,mbin_20,mbin_21,mbin_22,mbin_23,mbin_24,mbin_25,mbin_26,mbin_27,mbin_28,mbin_29,mbin_30,mbin_31,mbin_32,mbin_33,mbin_34,mbin_35,mbin_36,mbin_37,mbin_38,mbin_39,mbin_40,mbin_41,mbin_42,mbin_43,mbin_44,mbin_45,mbin_46,mbin_47,mbin_48,mbin_49,mbin_50,mbin_51,mbin_52,mbin_53,mbin_54,mbin_55,mbin_56,mbin_57,mbin_58,mbin_59,mbin_60,mbin_61,mbin_62,mbin_63,mbin_64,mbin_65,mbin_66,mbin_67,mbin_68,mbin_69,mbin_70,mbin_71,mbin_72,mbin_73,mbin_74,mbin_75,mbin_76,mbin_77,mbin_78,mbin_79,mbin_80,mbin_81,mbin_82,mbin_83,mbin_84,mbin_85,mbin_86,mbin_87,mbin_88,mbin_89,mbin_90,mbin_91,mbin_92,mbin_93,mbin_94,mbin_95,mbin_96,mbin_97,mbin_98,mbin_99,mbin_100,mbin_101,mbin_102,mbin_103,mbin_104,mbin_105,mbin_106,mbin_107,mbin_108,mbin_109,mbin_110,mbin_111,mbin_112,mbin_113,mbin_114,mbin_115,mbin_116,mbin_117,mbin_118,mbin_119,mbin_120,mbin_121,mbin_122,mbin_123,mbin_124,mbin_125,mbin_126,mbin_127,mbin_128,mbin_129,mbin_130,mbin_131,mbin_132,mbin_133,mbin_134,mbin_135,mbin_136,mbin_137,mbin_138,mbin_139,mbin_140,mbin_141,mbin_142,mbin_143,mbin_144,mbin_145,mbin_146,mbin_147,mbin_148,mbin_149,mbin_150,mbin_151,mbin_152,mbin_153,mbin_154,mbin_155,mbin_156,mbin_157,mbin_158,mbin_159,mbin_160,mbin_161,mbin_162,mbin_163,mbin_164,mbin_165,mbin_166,mbin_167,mbin_168,mbin_169,mbin_170,mbin_171,mbin_172,mbin_173,mbin_174,mbin_175,mbin_176,mbin_177,mbin_178,mbin_179,mbin_180,mbin_181,mbin_182,mbin_183,mbin_184,mbin_185,mbin_186,mbin_187,mbin_188,mbin_189,mbin_190,mbin_191,mbin_192,mbin_193,mbin_194,mbin_195,mbin_196,mbin_197,mbin_198,mbin_199,mbin_200,mbin_201,mbin_202,mbin_203,mbin_204,mbin_205,mbin_206,mbin_207,mbin_208,mbin_209,mbin_210,mbin_211,mbin_212,mbin_213,mbin_214,mbin_215,mbin_216,mbin_217,mbin_218,mbin_219,mbin_220,mbin_221,mbin_222,mbin_223,mbin_224,mbin_225,mbin_226,mbin_227,mbin_228,mbin_229,mbin_230,mbin_231,mbin_232,mbin_233,mbin_234,mbin_235,mbin_236,mbin_237,mbin_238,mbin_239,mbin_240,mbin_241,mbin_242,mbin_243,mbin_244,mbin_245,mbin_246,mbin_247,mbin_248,mbin_249,mbin_250,mbin_251,mbin_252,mbin_253,mbin_254,mbin_255,
# Links = 0
# mrs_string/onObsNames = Flux,
# Links = 0

# MarSystem
# Type = MFCC
# Name = mfcc

# MarControls = 11
# mrs_bool/active = 1
# Links = 0
# mrs_bool/debug = 0
# Links = 0
# mrs_bool/mute = 0
# Links = 0
# mrs_natural/inObservations = 256
# Links = 0
# mrs_natural/inSamples = 1
# Links = 0
# mrs_natural/onObservations = 13
# Links = 0
# mrs_natural/onSamples = 1
# Links = 0
# mrs_real/israte = 43.0664
# Links = 0
# mrs_real/osrate = 43.0664
# Links = 0
# mrs_string/inObsNames = mbin_0,mbin_1,mbin_2,mbin_3,mbin_4,mbin_5,mbin_6,mbin_7,mbin_8,mbin_9,mbin_10,mbin_11,mbin_12,mbin_13,mbin_14,mbin_15,mbin_16,mbin_17,mbin_18,mbin_19,mbin_20,mbin_21,mbin_22,mbin_23,mbin_24,mbin_25,mbin_26,mbin_27,mbin_28,mbin_29,mbin_30,mbin_31,mbin_32,mbin_33,mbin_34,mbin_35,mbin_36,mbin_37,mbin_38,mbin_39,mbin_40,mbin_41,mbin_42,mbin_43,mbin_44,mbin_45,mbin_46,mbin_47,mbin_48,mbin_49,mbin_50,mbin_51,mbin_52,mbin_53,mbin_54,mbin_55,mbin_56,mbin_57,mbin_58,mbin_59,mbin_60,mbin_61,mbin_62,mbin_63,mbin_64,mbin_65,mbin_66,mbin_67,mbin_68,mbin_69,mbin_70,mbin_71,mbin_72,mbin_73,mbin_74,mbin_75,mbin_76,mbin_77,mbin_78,mbin_79,mbin_80,mbin_81,mbin_82,mbin_83,mbin_84,mbin_85,mbin_86,mbin_87,mbin_88,mbin_89,mbin_90,mbin_91,mbin_92,mbin_93,mbin_94,mbin_95,mbin_96,mbin_97,mbin_98,mbin_99,mbin_100,mbin_101,mbin_102,mbin_103,mbin_104,mbin_105,mbin_106,mbin_107,mbin_108,mbin_109,mbin_110,mbin_111,mbin_112,mbin_113,mbin_114,mbin_115,mbin_116,mbin_117,mbin_118,mbin_119,mbin_120,mbin_121,mbin_122,mbin_123,mbin_124,mbin_125,mbin_126,mbin_127,mbin_128,mbin_129,mbin_130,mbin_131,mbin_132,mbin_133,mbin_134,mbin_135,mbin_136,mbin_137,mbin_138,mbin_139,mbin_140,mbin_141,mbin_142,mbin_143,mbin_144,mbin_145,mbin_146,mbin_147,mbin_148,mbin_149,mbin_150,mbin_151,mbin_152,mbin_153,mbin_154,mbin_155,mbin_156,mbin_157,mbin_158,mbin_159,mbin_160,mbin_161,mbin_162,mbin_163,mbin_164,mbin_165,mbin_166,mbin_167,mbin_168,mbin_169,mbin_170,mbin_171,mbin_172,mbin_173,mbin_174,mbin_175,mbin_176,mbin_177,mbin_178,mbin_179,mbin_180,mbin_181,mbin_182,mbin_183,mbin_184,mbin_185,mbin_186,mbin_187,mbin_188,mbin_189,mbin_190,mbin_191,mbin_192,mbin_193,mbin_194,mbin_195,mbin_196,mbin_197,mbin_198,mbin_199,mbin_200,mbin_201,mbin_202,mbin_203,mbin_204,mbin_205,mbin_206,mbin_207,mbin_208,mbin_209,mbin_210,mbin_211,mbin_212,mbin_213,mbin_214,mbin_215,mbin_216,mbin_217,mbin_218,mbin_219,mbin_220,mbin_221,mbin_222,mbin_223,mbin_224,mbin_225,mbin_226,mbin_227,mbin_228,mbin_229,mbin_230,mbin_231,mbin_232,mbin_233,mbin_234,mbin_235,mbin_236,mbin_237,mbin_238,mbin_239,mbin_240,mbin_241,mbin_242,mbin_243,mbin_244,mbin_245,mbin_246,mbin_247,mbin_248,mbin_249,mbin_250,mbin_251,mbin_252,mbin_253,mbin_254,mbin_255,
# Links = 0
# mrs_string/onObsNames = MFCC_0,MFCC_1,MFCC_2,MFCC_3,MFCC_4,MFCC_5,MFCC_6,MFCC_7,MFCC_8,MFCC_9,MFCC_10,MFCC_11,MFCC_12,
# Links = 0



# MarSystemComposite
# Type = TextureStats
# Name = tStats

# MarControls = 15
# mrs_bool/active = 1
# Links = 0
# mrs_bool/debug = 0
# Links = 0
# mrs_bool/mute = 0
# Links = 0
# mrs_bool/probe = 0
# Links = 0
# mrs_bool/reset = 0
# Links = 1
# /Series/featureNetwork/TextureStats/tStats/Memory/mempr/mrs_bool/reset
# mrs_natural/inObservations = 17
# Links = 0
# mrs_natural/inSamples = 1
# Links = 0
# mrs_natural/memSize = 40
# Links = 1
# /Series/featureNetwork/TextureStats/tStats/Memory/mempr/mrs_natural/memSize
# mrs_natural/onObservations = 34
# Links = 0
# mrs_natural/onSamples = 1
# Links = 0
# mrs_real/israte = 43.0664
# Links = 0
# mrs_real/osrate = 43.0664
# Links = 0
# mrs_realvec/input0 = # MARSYAS mrs_realvec
# Size = 0


# type: matrix
# rows: 0
# columns: 0

# Size = 0
# MARSYAS mrs_realvec

# Links = 0
# mrs_string/inObsNames = Centroid,Kurtosis,Rolloff,Flux,MFCC_0,MFCC_1,MFCC_2,MFCC_3,MFCC_4,MFCC_5,MFCC_6,MFCC_7,MFCC_8,MFCC_9,MFCC_10,MFCC_11,MFCC_12,
# Links = 0
# mrs_string/onObsNames = Mean_Mem40_Centroid,Mean_Mem40_Kurtosis,Mean_Mem40_Rolloff,Mean_Mem40_Flux,Mean_Mem40_MFCC_0,Mean_Mem40_MFCC_1,Mean_Mem40_MFCC_2,Mean_Mem40_MFCC_3,Mean_Mem40_MFCC_4,Mean_Mem40_MFCC_5,Mean_Mem40_MFCC_6,Mean_Mem40_MFCC_7,Mean_Mem40_MFCC_8,Mean_Mem40_MFCC_9,Mean_Mem40_MFCC_10,Mean_Mem40_MFCC_11,Mean_Mem40_MFCC_12,Std_Mem40_Centroid,Std_Mem40_Kurtosis,Std_Mem40_Rolloff,Std_Mem40_Flux,Std_Mem40_MFCC_0,Std_Mem40_MFCC_1,Std_Mem40_MFCC_2,Std_Mem40_MFCC_3,Std_Mem40_MFCC_4,Std_Mem40_MFCC_5,Std_Mem40_MFCC_6,Std_Mem40_MFCC_7,Std_Mem40_MFCC_8,Std_Mem40_MFCC_9,Std_Mem40_MFCC_10,Std_Mem40_MFCC_11,Std_Mem40_MFCC_12,
# Links = 0

# nComponents = 2

# MarSystem
# Type = Memory
# Name = mempr

# MarControls = 13
# mrs_bool/active = 1
# Links = 0
# mrs_bool/debug = 0
# Links = 0
# mrs_bool/mute = 0
# Links = 0
# mrs_bool/reset = 0
# Links = 1
# /Series/featureNetwork/TextureStats/tStats/mrs_bool/reset
# mrs_natural/inObservations = 17
# Links = 0
# mrs_natural/inSamples = 1
# Links = 0
# mrs_natural/memSize = 40
# Links = 1
# /Series/featureNetwork/TextureStats/tStats/mrs_natural/memSize
# mrs_natural/onObservations = 17
# Links = 0
# mrs_natural/onSamples = 40
# Links = 0
# mrs_real/israte = 43.0664
# Links = 0
# mrs_real/osrate = 43.0664
# Links = 0
# mrs_string/inObsNames = Centroid,Kurtosis,Rolloff,Flux,MFCC_0,MFCC_1,MFCC_2,MFCC_3,MFCC_4,MFCC_5,MFCC_6,MFCC_7,MFCC_8,MFCC_9,MFCC_10,MFCC_11,MFCC_12,
# Links = 0
# mrs_string/onObsNames = Mem40_Centroid,Mem40_Kurtosis,Mem40_Rolloff,Mem40_Flux,Mem40_MFCC_0,Mem40_MFCC_1,Mem40_MFCC_2,Mem40_MFCC_3,Mem40_MFCC_4,Mem40_MFCC_5,Mem40_MFCC_6,Mem40_MFCC_7,Mem40_MFCC_8,Mem40_MFCC_9,Mem40_MFCC_10,Mem40_MFCC_11,Mem40_MFCC_12,
# Links = 0

# MarSystemComposite
# Type = Fanout
# Name = meanstdpr

# MarControls = 13
# mrs_bool/active = 1
# Links = 0
# mrs_bool/debug = 0
# Links = 0
# mrs_bool/mute = 0
# Links = 0
# mrs_bool/probe = 0
# Links = 0
# mrs_natural/disable = -1
# Links = 0
# mrs_natural/inObservations = 17
# Links = 0
# mrs_natural/inSamples = 40
# Links = 0
# mrs_natural/onObservations = 34
# Links = 0
# mrs_natural/onSamples = 1
# Links = 0
# mrs_real/israte = 43.0664
# Links = 0
# mrs_real/osrate = 43.0664
# Links = 0
# mrs_string/inObsNames = Mem40_Centroid,Mem40_Kurtosis,Mem40_Rolloff,Mem40_Flux,Mem40_MFCC_0,Mem40_MFCC_1,Mem40_MFCC_2,Mem40_MFCC_3,Mem40_MFCC_4,Mem40_MFCC_5,Mem40_MFCC_6,Mem40_MFCC_7,Mem40_MFCC_8,Mem40_MFCC_9,Mem40_MFCC_10,Mem40_MFCC_11,Mem40_MFCC_12,
# Links = 0
# mrs_string/onObsNames = Mean_Mem40_Centroid,Mean_Mem40_Kurtosis,Mean_Mem40_Rolloff,Mean_Mem40_Flux,Mean_Mem40_MFCC_0,Mean_Mem40_MFCC_1,Mean_Mem40_MFCC_2,Mean_Mem40_MFCC_3,Mean_Mem40_MFCC_4,Mean_Mem40_MFCC_5,Mean_Mem40_MFCC_6,Mean_Mem40_MFCC_7,Mean_Mem40_MFCC_8,Mean_Mem40_MFCC_9,Mean_Mem40_MFCC_10,Mean_Mem40_MFCC_11,Mean_Mem40_MFCC_12,Std_Mem40_Centroid,Std_Mem40_Kurtosis,Std_Mem40_Rolloff,Std_Mem40_Flux,Std_Mem40_MFCC_0,Std_Mem40_MFCC_1,Std_Mem40_MFCC_2,Std_Mem40_MFCC_3,Std_Mem40_MFCC_4,Std_Mem40_MFCC_5,Std_Mem40_MFCC_6,Std_Mem40_MFCC_7,Std_Mem40_MFCC_8,Std_Mem40_MFCC_9,Std_Mem40_MFCC_10,Std_Mem40_MFCC_11,Std_Mem40_MFCC_12,
# Links = 0

# nComponents = 2

# MarSystem
# Type = Mean
# Name = meanpr

# MarControls = 11
# mrs_bool/active = 1
# Links = 0
# mrs_bool/debug = 0
# Links = 0
# mrs_bool/mute = 0
# Links = 0
# mrs_natural/inObservations = 17
# Links = 0
# mrs_natural/inSamples = 40
# Links = 0
# mrs_natural/onObservations = 17
# Links = 0
# mrs_natural/onSamples = 1
# Links = 0
# mrs_real/israte = 43.0664
# Links = 0
# mrs_real/osrate = 43.0664
# Links = 0
# mrs_string/inObsNames = Mem40_Centroid,Mem40_Kurtosis,Mem40_Rolloff,Mem40_Flux,Mem40_MFCC_0,Mem40_MFCC_1,Mem40_MFCC_2,Mem40_MFCC_3,Mem40_MFCC_4,Mem40_MFCC_5,Mem40_MFCC_6,Mem40_MFCC_7,Mem40_MFCC_8,Mem40_MFCC_9,Mem40_MFCC_10,Mem40_MFCC_11,Mem40_MFCC_12,
# Links = 0
# mrs_string/onObsNames = Mean_Mem40_Centroid,Mean_Mem40_Kurtosis,Mean_Mem40_Rolloff,Mean_Mem40_Flux,Mean_Mem40_MFCC_0,Mean_Mem40_MFCC_1,Mean_Mem40_MFCC_2,Mean_Mem40_MFCC_3,Mean_Mem40_MFCC_4,Mean_Mem40_MFCC_5,Mean_Mem40_MFCC_6,Mean_Mem40_MFCC_7,Mean_Mem40_MFCC_8,Mean_Mem40_MFCC_9,Mean_Mem40_MFCC_10,Mean_Mem40_MFCC_11,Mean_Mem40_MFCC_12,
# Links = 0

# MarSystem
# Type = StandardDeviation
# Name = stdpr

# MarControls = 11
# mrs_bool/active = 1
# Links = 0
# mrs_bool/debug = 0
# Links = 0
# mrs_bool/mute = 0
# Links = 0
# mrs_natural/inObservations = 17
# Links = 0
# mrs_natural/inSamples = 40
# Links = 0
# mrs_natural/onObservations = 17
# Links = 0
# mrs_natural/onSamples = 1
# Links = 0
# mrs_real/israte = 43.0664
# Links = 0
# mrs_real/osrate = 43.0664
# Links = 0
# mrs_string/inObsNames = Mem40_Centroid,Mem40_Kurtosis,Mem40_Rolloff,Mem40_Flux,Mem40_MFCC_0,Mem40_MFCC_1,Mem40_MFCC_2,Mem40_MFCC_3,Mem40_MFCC_4,Mem40_MFCC_5,Mem40_MFCC_6,Mem40_MFCC_7,Mem40_MFCC_8,Mem40_MFCC_9,Mem40_MFCC_10,Mem40_MFCC_11,Mem40_MFCC_12,
# Links = 0
# mrs_string/onObsNames = Std_Mem40_Centroid,Std_Mem40_Kurtosis,Std_Mem40_Rolloff,Std_Mem40_Flux,Std_Mem40_MFCC_0,Std_Mem40_MFCC_1,Std_Mem40_MFCC_2,Std_Mem40_MFCC_3,Std_Mem40_MFCC_4,Std_Mem40_MFCC_5,Std_Mem40_MFCC_6,Std_Mem40_MFCC_7,Std_Mem40_MFCC_8,Std_Mem40_MFCC_9,Std_Mem40_MFCC_10,Std_Mem40_MFCC_11,Std_Mem40_MFCC_12,
# Links = 0



# MarSystem
# Type = Annotator
# Name = annotator

# MarControls = 12
# mrs_bool/active = 1
# Links = 0
# mrs_bool/debug = 0
# Links = 0
# mrs_bool/mute = 0
# Links = 0
# mrs_natural/inObservations = 34
# Links = 0
# mrs_natural/inSamples = 1
# Links = 0
# mrs_natural/label = 1
# Links = 0
# mrs_natural/onObservations = 35
# Links = 0
# mrs_natural/onSamples = 1
# Links = 0
# mrs_real/israte = 43.0664
# Links = 0
# mrs_real/osrate = 43.0664
# Links = 0
# mrs_string/inObsNames = Mean_Mem40_Centroid,Mean_Mem40_Kurtosis,Mean_Mem40_Rolloff,Mean_Mem40_Flux,Mean_Mem40_MFCC_0,Mean_Mem40_MFCC_1,Mean_Mem40_MFCC_2,Mean_Mem40_MFCC_3,Mean_Mem40_MFCC_4,Mean_Mem40_MFCC_5,Mean_Mem40_MFCC_6,Mean_Mem40_MFCC_7,Mean_Mem40_MFCC_8,Mean_Mem40_MFCC_9,Mean_Mem40_MFCC_10,Mean_Mem40_MFCC_11,Mean_Mem40_MFCC_12,Std_Mem40_Centroid,Std_Mem40_Kurtosis,Std_Mem40_Rolloff,Std_Mem40_Flux,Std_Mem40_MFCC_0,Std_Mem40_MFCC_1,Std_Mem40_MFCC_2,Std_Mem40_MFCC_3,Std_Mem40_MFCC_4,Std_Mem40_MFCC_5,Std_Mem40_MFCC_6,Std_Mem40_MFCC_7,Std_Mem40_MFCC_8,Std_Mem40_MFCC_9,Std_Mem40_MFCC_10,Std_Mem40_MFCC_11,Std_Mem40_MFCC_12,
# Links = 0
# mrs_string/onObsNames = Mean_Mem40_Centroid,Mean_Mem40_Kurtosis,Mean_Mem40_Rolloff,Mean_Mem40_Flux,Mean_Mem40_MFCC_0,Mean_Mem40_MFCC_1,Mean_Mem40_MFCC_2,Mean_Mem40_MFCC_3,Mean_Mem40_MFCC_4,Mean_Mem40_MFCC_5,Mean_Mem40_MFCC_6,Mean_Mem40_MFCC_7,Mean_Mem40_MFCC_8,Mean_Mem40_MFCC_9,Mean_Mem40_MFCC_10,Mean_Mem40_MFCC_11,Mean_Mem40_MFCC_12,Std_Mem40_Centroid,Std_Mem40_Kurtosis,Std_Mem40_Rolloff,Std_Mem40_Flux,Std_Mem40_MFCC_0,Std_Mem40_MFCC_1,Std_Mem40_MFCC_2,Std_Mem40_MFCC_3,Std_Mem40_MFCC_4,Std_Mem40_MFCC_5,Std_Mem40_MFCC_6,Std_Mem40_MFCC_7,Std_Mem40_MFCC_8,Std_Mem40_MFCC_9,Std_Mem40_MFCC_10,Std_Mem40_MFCC_11,Std_Mem40_MFCC_12,
# Links = 0

# MarSystem
# Type = WekaSink
# Name = wsink

# MarControls = 16
# mrs_bool/active = 1
# Links = 0
# mrs_bool/debug = 0
# Links = 0
# mrs_bool/mute = 1
# Links = 0
# mrs_natural/downsample = 40
# Links = 0
# mrs_natural/inObservations = 35
# Links = 0
# mrs_natural/inSamples = 1
# Links = 0
# mrs_natural/nLabels = 2
# Links = 0
# mrs_natural/onObservations = 35
# Links = 0
# mrs_natural/onSamples = 1
# Links = 0
# mrs_natural/precision = 6
# Links = 0
# mrs_real/israte = 43.0664
# Links = 0
# mrs_real/osrate = 43.0664
# Links = 0
# mrs_string/filename = ms.arff
# Links = 0
# mrs_string/inObsNames = Mean_Mem40_Centroid,Mean_Mem40_Kurtosis,Mean_Mem40_Rolloff,Mean_Mem40_Flux,Mean_Mem40_MFCC_0,Mean_Mem40_MFCC_1,Mean_Mem40_MFCC_2,Mean_Mem40_MFCC_3,Mean_Mem40_MFCC_4,Mean_Mem40_MFCC_5,Mean_Mem40_MFCC_6,Mean_Mem40_MFCC_7,Mean_Mem40_MFCC_8,Mean_Mem40_MFCC_9,Mean_Mem40_MFCC_10,Mean_Mem40_MFCC_11,Mean_Mem40_MFCC_12,Std_Mem40_Centroid,Std_Mem40_Kurtosis,Std_Mem40_Rolloff,Std_Mem40_Flux,Std_Mem40_MFCC_0,Std_Mem40_MFCC_1,Std_Mem40_MFCC_2,Std_Mem40_MFCC_3,Std_Mem40_MFCC_4,Std_Mem40_MFCC_5,Std_Mem40_MFCC_6,Std_Mem40_MFCC_7,Std_Mem40_MFCC_8,Std_Mem40_MFCC_9,Std_Mem40_MFCC_10,Std_Mem40_MFCC_11,Std_Mem40_MFCC_12,
# Links = 0
# mrs_string/labelNames = music,speech,
# Links = 0
# mrs_string/onObsNames = Mean_Mem40_Centroid,Mean_Mem40_Kurtosis,Mean_Mem40_Rolloff,Mean_Mem40_Flux,Mean_Mem40_MFCC_0,Mean_Mem40_MFCC_1,Mean_Mem40_MFCC_2,Mean_Mem40_MFCC_3,Mean_Mem40_MFCC_4,Mean_Mem40_MFCC_5,Mean_Mem40_MFCC_6,Mean_Mem40_MFCC_7,Mean_Mem40_MFCC_8,Mean_Mem40_MFCC_9,Mean_Mem40_MFCC_10,Mean_Mem40_MFCC_11,Mean_Mem40_MFCC_12,Std_Mem40_Centroid,Std_Mem40_Kurtosis,Std_Mem40_Rolloff,Std_Mem40_Flux,Std_Mem40_MFCC_0,Std_Mem40_MFCC_1,Std_Mem40_MFCC_2,Std_Mem40_MFCC_3,Std_Mem40_MFCC_4,Std_Mem40_MFCC_5,Std_Mem40_MFCC_6,Std_Mem40_MFCC_7,Std_Mem40_MFCC_8,Std_Mem40_MFCC_9,Std_Mem40_MFCC_10,Std_Mem40_MFCC_11,Std_Mem40_MFCC_12,
# Links = 0

# MarSystem
# Type = GaussianClassifier
# Name = gaussian

# MarControls = 16
# mrs_bool/active = 1
# Links = 0
# mrs_bool/debug = 0
# Links = 0
# mrs_bool/done = 0
# Links = 0
# mrs_bool/mute = 0
# Links = 0
# mrs_natural/inObservations = 35
# Links = 0
# mrs_natural/inSamples = 1
# Links = 0
# mrs_natural/nLabels = 2
# Links = 0
# mrs_natural/onObservations = 2
# Links = 0
# mrs_natural/onSamples = 1
# Links = 0
# mrs_real/israte = 43.0664
# Links = 0
# mrs_real/osrate = 43.0664
# Links = 0
# mrs_realvec/covars = # MARSYAS mrs_realvec
# Size = 70


# type: matrix
# rows: 2
# columns: 35
672.379 1.47497e-08 89.5768 8517.5 0.0233969 0.23238 0.513391 0.931705 1.87384 3.14514 3.59291 3.2907 4.21805 4.22248 4.68597 4 5.20302 2632.23 1.3601e-07 105.06 1108.59 0.188578 4.92252 11.9482 18.9613 34.6472 50.5192 57.9484 50.1774 43.1415 41.9485 37.4851 38.1676 35.1479 0 
547.166 1.50454e-08 56.7602 6419.93 0.0283574 0.465259 0.6635 1.21599 3.11309 3.43575 3.91207 5.90317 7.67139 8.40763 11.452 7.94557 15.9763 471.969 1.28097e-07 79.3564 1137.1 0.116155 1.4079 5.23462 5.28944 9.50641 23.1513 22.2943 29.6261 45.2732 50.2671 57.3334 61.4233 59.8031 0 

# Size = 70
# MARSYAS mrs_realvec

# Links = 0
# mrs_realvec/means = # MARSYAS mrs_realvec
# Size = 70


# type: matrix
# rows: 2
# columns: 35
0.0637271 27148.7 0.152104 0.0806132 -70.6699 5.63442 0.0308151 1.93386 0.358042 0.658067 0.312909 0.395901 0.313466 0.338927 0.306107 0.31695 0.314723 0.0256222 10397.5 0.0936324 0.0310235 3.16361 0.986486 0.75197 0.63195 0.554695 0.514538 0.498398 0.499868 0.500676 0.498132 0.495666 0.481479 0.48175 0 
0.0865026 30467.5 0.278593 0.089635 -76.5046 4.75224 0.152343 2.26231 0.601581 0.437158 0.507565 0.36562 0.475342 0.454828 0.380355 0.400384 0.447518 0.0724108 13876.9 0.240568 0.0350258 6.64523 2.13963 1.2967 1.33244 0.996553 0.770918 0.775152 0.711787 0.620098 0.587889 0.547875 0.519989 0.493572 0 

# Size = 70
# MARSYAS mrs_realvec

# Links = 0
# mrs_string/inObsNames = Mean_Mem40_Centroid,Mean_Mem40_Kurtosis,Mean_Mem40_Rolloff,Mean_Mem40_Flux,Mean_Mem40_MFCC_0,Mean_Mem40_MFCC_1,Mean_Mem40_MFCC_2,Mean_Mem40_MFCC_3,Mean_Mem40_MFCC_4,Mean_Mem40_MFCC_5,Mean_Mem40_MFCC_6,Mean_Mem40_MFCC_7,Mean_Mem40_MFCC_8,Mean_Mem40_MFCC_9,Mean_Mem40_MFCC_10,Mean_Mem40_MFCC_11,Mean_Mem40_MFCC_12,Std_Mem40_Centroid,Std_Mem40_Kurtosis,Std_Mem40_Rolloff,Std_Mem40_Flux,Std_Mem40_MFCC_0,Std_Mem40_MFCC_1,Std_Mem40_MFCC_2,Std_Mem40_MFCC_3,Std_Mem40_MFCC_4,Std_Mem40_MFCC_5,Std_Mem40_MFCC_6,Std_Mem40_MFCC_7,Std_Mem40_MFCC_8,Std_Mem40_MFCC_9,Std_Mem40_MFCC_10,Std_Mem40_MFCC_11,Std_Mem40_MFCC_12,
# Links = 0
# mrs_string/mode = predict
# Links = 0
# mrs_string/onObsNames = ,
# Links = 0

# MarSystem
# Type = Confidence
# Name = confidence

# MarControls = 16
# mrs_bool/active = 1
# Links = 0
# mrs_bool/debug = 0
# Links = 0
# mrs_bool/forcePrint = 0
# Links = 0
# mrs_bool/mute = 0
# Links = 0
# mrs_bool/print = 1
# Links = 0
# mrs_natural/inObservations = 2
# Links = 0
# mrs_natural/inSamples = 1
# Links = 0
# mrs_natural/memSize = 40
# Links = 0
# mrs_natural/nLabels = 2
# Links = 0
# mrs_natural/onObservations = 2
# Links = 0
# mrs_natural/onSamples = 1
# Links = 0
# mrs_real/israte = 43.0664
# Links = 0
# mrs_real/osrate = 43.0664
# Links = 0
# mrs_string/inObsNames = ,
# Links = 0
# mrs_string/labelNames = music,speech,
# Links = 0
# mrs_string/onObsNames = ,
# Links = 0


