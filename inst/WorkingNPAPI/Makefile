# Generated automatically from Makefile.in by configure.
NPAPI_SDK=/home/gmbecker/gabe/checkedout/npapi-sdk-read-only
R_HOME=/usr/lib64/R
IS_MAC=0
PKGCONFIG=pkg-config

CXX=g++
CPPFLAGS += $(shell R CMD config --cppflags ) \
	-I$(NPAPI_SDK)/headers \
	-I$(R_HOME)/include \
	-I./ \
	-fPIC 

ifeq ($(IS_MAC), 1)
  DLIBEXT=dylib
  R_LD_FLAGS=-L$(R_HOME)/lib/$(ARCH) -lR
  TYPE=-dynamiclib
  CPPFLAGS += -DXP_MACOSX -DXP_MAC
else
  DLIBEXT=so
  R_LD_FLAGS=-L$(R_HOME)/lib -lR
  TYPE=-shared
  CPPFLAGS += -DXP_UNIX=1
endif

OBJ_FILES =  WorkingWebR.o WebREngine.o plugin.o
%o: %.cpp
	$(CXX) -c -o $@ $(CPPFLAGS) $<

WebR.$(DLIBEXT): $(OBJ_FILES)
	$(CXX) -Wall -Os $^ $(R_LD_FLAGS) $(TYPE) -o $@

tiny.$(DLIBEXT): tinyplugin.o
	$(CXX) -Wall -Os $^ $(TYPE) -o $@

default: WebR.$(DLIBEXT) tiny.$(DLIBEXT)

clean: 
	rm *.o
	rm *.so