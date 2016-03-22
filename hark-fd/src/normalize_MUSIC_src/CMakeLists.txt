cmake_minimum_required(VERSION 2.6)
project(bayesian_MUSIC)

set(CMAKE_C_COMPILER gcc)
set(CMAKE_CXX_COMPILER g++)

INCLUDE_DIRECTORIES(/usr/include /usr/include/python2.7 )
LINK_DIRECTORIES(/usr/lib )

set(VBHMM_SRC ${VBHMM_SRC} test_VBHMM.cpp)
set(VBHMM_SRC ${VBHMM_SRC} train_VBHMM.cpp)

set(PF_SRC ${PF_SRC} test_PF.cpp)
set(PF_SRC ${PF_SRC} train_VBHMM.cpp)
set(PF_SRC ${PF_SRC} track_PF.cpp)
set(PF_SRC ${PF_SRC} mt.cc)

ADD_EXECUTABLE(test_vbhmm ${VBHMM_SRC})
ADD_EXECUTABLE(pf_test ${PF_SRC})

SET_TARGET_PROPERTIES(test_vbhmm PROPERTIES COMPILE_FLAGS "-Wall -O3 -march=native -pipe -fomit-frame-pointer -c -fmessage-length=0 -fopenmp")
SET_TARGET_PROPERTIES(pf_test PROPERTIES COMPILE_FLAGS "-Wall -O3 -march=native -pipe -fomit-frame-pointer -c -fmessage-length=0 -fopenmp")

TARGET_LINK_LIBRARIES(test_vbhmm boost_system boost_regex)
#TARGET_LINK_LIBRARIES(pf_test boost_system boost_regex)
TARGET_LINK_LIBRARIES(pf_test boost_regex)
