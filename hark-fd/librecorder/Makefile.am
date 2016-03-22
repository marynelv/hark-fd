noinst_LIBRARIES = librecorder.a

boost_thread = @BOOST_THREAD@

#librecorder_a_SOURCES = ALSARecorder.cpp ASIORecorder.cpp RASPRecorder.cpp \
#	WSRecorder.cpp
librecorder_a_CXXFLAGS = -pthread -fPIC -DPIC

librecorder_a_SOURCES = ALSARecorder.cpp
if ENABLE_ASRASP
librecorder_a_SOURCES += ASRASPRecorder.cpp
endif
if ENABLE_WS
librecorder_a_SOURCES += WSRecorder.cpp
endif
if ENABLE_RASP24
librecorder_a_SOURCES += RASP24Recorder.cpp
endif
if ENABLE_HARKNETAPI
librecorder_a_SOURCES += HARKNETAPIRecorder.cpp
endif
#if ENABLE_TDBD8CSUSB
#librecorder_a_SOURCES += ASIORecorder.cpp
#endif
#if ENABLE_TDBD16ADUSB
#librecorder_a_SOURCES += SINICHRecorder.cpp
#librecorder_a_CXXFLAGS += -D_LINUX_DEF_
#endif
if ENABLE_RASP2
librecorder_a_SOURCES += RASPRecorder.cpp
endif
if ENABLE_RASPLC
librecorder_a_SOURCES += RASPLCRecorder.cpp
endif
if ENABLE_KANEKO
librecorder_a_SOURCES += KANEKORecorder.cpp
endif

#INCLUDES = -I ./boost-sandbox
AM_CPPFLAGS = -I ./boost-sandbox




#bin_PROGRAMS = recorder rasprec wsrecord
bin_PROGRAMS =

#if ENABLE_TDBD8CSUSB
#bin_PROGRAMS += recorder
#recorder_SOURCES = main.cpp
#recorder_CXXFLAGS = -pthread
#recorder_LDFLAGS = -static
#recorder_LDADD = -L. librecorder.a -lasound -lasio $(boost_thread)
#endif

#if ENABLE_TDBD16ADUSB
#bin_PROGRAMS += sinrec
#sinrec_SOURCES = sinmain.cpp
#sinrec_CXXFLAGS = -pthread
#sinrec_LDFLAGS = -static
#sinrec_LDADD = -L. librecorder.a -lsinich $(boost_thread)
#endif	

if ENABLE_RASP2
bin_PROGRAMS += rasprec
rasprec_SOURCES = rasprec.cpp
rasprec_CXXFLAGS = -pthread
rasprec_LDFLAGS = -static
rasprec_LDADD = -L. librecorder.a $(boost_thread) -lboost_system
endif

if ENABLE_WS
bin_PROGRAMS += wsrecord
wsrecord_SOURCES = wsrecord.cpp
wsrecord_CXXFLAGS = -pthread
wsrecord_LDFLAGS = -static
wsrecord_LDADD = -L. librecorder.a $(boost_thread) -lrasp-netapi -lboost_system
endif

if ENABLE_RASP24
bin_PROGRAMS += rasp24record
rasp24record_SOURCES  = rasp24record.cpp
rasp24record_CXXFLAGS = -pthread
rasp24record_LDFLAGS  = -static
rasp24record_LDADD    = -L. librecorder.a $(boost_thread) -lrasp-netapi -lboost_system
endif

if ENABLE_HARKNETAPI
bin_PROGRAMS += harknetapirecord
harknetapirecord_SOURCES  = harknetapirecord.cpp
harknetapirecord_CXXFLAGS = -pthread
harknetapirecord_LDFLAGS  = -static
harknetapirecord_LDADD    = -L. librecorder.a $(boost_thread) -lhark-netapi -lboost_system
endif

if ENABLE_RASPLC
bin_PROGRAMS += rasplcrecord
rasplcrecord_SOURCES  = rasplcrecord.cpp
rasplcrecord_CXXFLAGS = -pthread
rasplcrecord_LDFLAGS  = -static
rasplcrecord_LDADD    = -L. librecorder.a $(boost_thread) -lrasp-netapi -lboost_system
endif

if ENABLE_KANEKO
bin_PROGRAMS += kanekorecord
kanekorecord_SOURCES  = kanekorecord.cpp
kanekorecord_CXXFLAGS = -pthread
kanekorecord_LDFLAGS  = -static
kanekorecord_LDADD    = -L. librecorder.a $(boost_thread) -lrasp-netapi -lkm-netapi -lboost_system
endif


