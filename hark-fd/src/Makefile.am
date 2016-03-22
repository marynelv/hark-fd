#override datadir=$(FLOWDESIGNER_DATA)
override datadir=${prefix}/lib/flowdesigner/toolbox/hark
#override libdir=$(FLOWDESIGNER_DATA)
override libdir=${prefix}/lib/flowdesigner/toolbox/hark

lib_LTLIBRARIES = libhark.la 

boost_thread = @BOOST_THREAD@

install-data-local:
	mkdir -p ${DESTDIR}$(datadir)
	(if [ -f ${DESTDIR}$(libdir)/lib$(PACKAGE) ]; then cd ${DESTDIR}$(libdir); rm -f $(PACKAGE).tlb; ln -s lib$(PACKAGE) $(PACKAGE).tlb; else cd ${DESTDIR}$(libdir); rm -f $(PACKAGE).tlb; ln -s lib$(PACKAGE).so $(PACKAGE).tlb; fi)
	(info2def.pl $(libhark_la_SOURCES) > ${DESTDIR}$(datadir)/hark.def)

libhark_la_SOURCES = \
	VERSION.cc \
	MatrixToMap.cc \
	AudioStreamFromMic.cc \
	AudioStreamFromWave.cc \
	ChannelSelector.cc \
	MultiFFT.cc \
	HarkParamsDynReconf.cc \
	ConstantLocalization.cc \
	SaveSourceLocation.cc LoadSourceLocation.cc \
	LocalizationFunctionMUSIC.cpp \
	LocalizationFunction.cpp \
	CMChannelSelector.cc \
	CMIdentityMatrix.cc \
	CMLoad.cc \
	CMMakerFromFFTwithFlag.cc \
	CMMultiplyMatrix.cc \
	CMDivideEachElement.cc \
	CMConjEachElement.cc \
	CMInverseMatrix.cc \
	CMMakerFromFFT.cc \
	CMMultiplyEachElement.cc \
	CMSave.cc \
	CombineSource.cc \
	DisplayLocalization.cc \
	MelFilterBank.cc \
	MultiDownSampler.cc \
	MSLSExtraction.cc \
	MFCCExtraction.cc \
	Delta.cc \
	SpectralMeanNormalization.cc \
	MFMGeneration.cc \
	DeltaMask.cc \
	SaveFeatures.cc \
	SaveHTKFeatures.cc \
	SaveRawPCM.cc \
	SaveWavePCM.cc \
	Synthesize.cc \
	WhiteNoiseAdder.cc \
	SpeechRecognitionClient.cc 	SpeechRecognitionSMNClient.cc \
	HarkDataStreamSender.cc \
	SourceSelectorByDirection.cc \
	MapSelectorBySource.cc \
	FeatureRemover.cc \
	SourceIntervalExtender.cc \
	PF_est_leak.c PF_est_noise.c PF_est_rev.c PF_est_sn.c \
	PF_est_state_noise.c PF_est_voice_p.c PF_est_zeta.c PF_execV2.c \
	PF_hypergeom_gain.c PF_Init.c \
	PF_init_state_noise.c \
	CPF_exec.cpp \
	CPF_init_state_noise.cpp \
	PowerCalcForMap.cc \
	PowerCalcForMatrix.cc \
	BGNEstimator.cc \
	PostFilter.cc \
	SourceSelectorByID.cc \
	DeltaPowerMask.cc \
	PreEmphasis.cc \
	MultiGain.cc \
	GHDSS_SeparateMat.cpp \
	CGHDSS_exec.cpp \
	DataLogger.cc \
	HRLE.cc \
	SpectralGainFilter.cc \
	CalcSpecAddPower.cc \
	CalcSpecSubGain.cc \
	EstimateLeak.cc \
	SegmentAudioStreamByID.cc \
	SemiBlindICA.cc \
	NormalizeMUSIC.cc \
	normalize_MUSIC_src/train_VBHMM.cpp \
	normalize_MUSIC_src/track_PF.cpp \
	normalize_MUSIC_src/mt.cc 


pkginclude_HEADERS= \
	../include/Cexec.h ../include/Cexec_NH.h \
	../include/CGHDSS_exec.h \
	../include/CPF_exec.h \
	../include/CPF_init_state_noise.h \
	../include/endian.hpp \
	../include/evd.hpp \
	../include/fft.hpp \
	../include/GHDSS_MathMat2.h \
	../include/GHDSS_MathMatDouble.h \
	../include/GHDSS_SeparateMat.h \
	../include/HarkData.h \
	../include/HarkFileIO.h ../include/HarkFileIO_NH.h \
	../include/HarkParamsObjects.h \
	../include/htkheader.hpp \
	../include/LocalizationFunction.hpp \
	../include/LocalizationFunctionMUSIC.hpp \
	../include/LocalizationMUSIC.hpp \
	../include/Map.h \
	../include/melfilterbank.hpp \
	../include/mfcnet.h \
	../include/PF_est_leak.h \
	../include/PF_est_noise.h \
	../include/PF_est_rev.h \
	../include/PF_est_sn.h \
	../include/PF_est_state_noise.h \
	../include/PF_est_voice_p.h \
	../include/PF_est_zeta.h \
	../include/PF_exec.h \
	../include/PF_hypergeom_gain.h \
	../include/PF_Init.h \
	../include/PF_init_state_noise.h \
	../include/PF_values.h \
	../include/Source.h \
	../include/SpectralMean.hpp \
	../include/spline.hpp \
	../include/svd.hpp \
	../include/Tensor.h \
	../include/VecStrVecFlt.h \
	../include/Time.h \
	../include/TimeStamp.h \
	../include/wave.h \
	./normalize_MUSIC_src/Array2D.hpp \
	./normalize_MUSIC_src/Configuration.hpp \
	./normalize_MUSIC_src/CsvIO.hpp \
	./normalize_MUSIC_src/PreciseNumber.hpp \
	./normalize_MUSIC_src/mt.h \
	./normalize_MUSIC_src/track_PF.h \
	./normalize_MUSIC_src/train_VBHMM.h \
	./normalize_MUSIC_src/utilities.hpp \
	../libseparation/ArrayInit.h \
	../libseparation/CMICARY_Matrix.h \
	../libseparation/MICARYHA_struct.h \
	../libseparation/MICARY_GHDSS_struct.h \
	../libseparation/MICARY_PF_Struct.h \
	../libseparation/MICARY_TimeWin.h \
	../libseparation/MICARY_def.h \
	../libseparation/MICARY_struct.h





libhark_la_CXXFLAGS = @GTK_CFLAGS@

libhark_la_LDFLAGS = -release $(LT_RELEASE) \
	`flowdesigner-config --libs config libflow audio_blocks` \
	`pkg-config libharkio --libs` \
	-L../librecorder -lrecorder @GTK_LIBS@ \
	-L../libseparation -lseparation @GTK_LIBS@ \
	$(boost_thread) -lasound


AM_CPPFLAGS = `flowdesigner-config --cflags config libflow audio_blocks` \
	-I ../include \
	`pkg-config libharkio --cflags` \
	-I ../librecorder/boost-sandbox -I ../librecorder \
	-I ../libseparation

if ENABLE_HARKIO3
libhark_la_SOURCES += LocalizeMUSIC_NH.cc LocalizationMUSIC_NH.cpp \
	SourceTracker_NH.cc \
	GHDSS_NH.cc Cexec_NH.cpp HarkFileIO_NH.cpp 
else
libhark_la_SOURCES += LocalizeMUSIC.cc LocalizationMUSIC.cpp \
	SourceTracker.cc \
	GHDSS.cc Cexec.cpp HarkFileIO.cpp
endif

if ENABLE_HARKIO1
libhark_la_LDFLAGS += -lharkio
endif
if ENABLE_HARKIO2
libhark_la_LDFLAGS += -lharkio2 -lxml++-2.6 -lxml2 -lglibmm-2.4 -lgobject-2.0 -lsigc-2.0 -lgthread-2.0 -lrt -lglib-2.0 -pthread
endif
if ENABLE_HARKIO3
libhark_la_LDFLAGS += -lharkio3
endif
if ENABLE_ASRASP
libhark_la_LDFLAGS += -lrasp-netapi
endif
if ENABLE_WS
libhark_la_LDFLAGS += -lrasp-netapi
endif
if ENABLE_RASP24
libhark_la_LDFLAGS += -lrasp-netapi
endif
if ENABLE_HARKNETAPI
libhark_la_LDFLAGS += -lhark-netapi
endif
#if ENABLE_TDBD8CSUSB
#libhark_la_LDFLAGS += -lasio
#endif
#if ENABLE_TDBD16ADUSB
#libhark_la_LDFLAGS += -lsinich
#endif
if ENABLE_RASPLC
libhark_la_LDFLAGS += -lrasp-netapi
endif
if ENABLE_KANEKO
libhark_la_LDFLAGS += -lrasp-netapi -lkm-netapi
endif
if ENABLE_PLAYAUDIO
libhark_la_SOURCES += PlayAudio.cc SDLsound.cpp
pkginclude_HEADERS += ../include/SDLsound.h
libhark_la_LDFLAGS += `sdl2-config --cflags --libs`
endif




