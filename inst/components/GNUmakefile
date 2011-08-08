
ifndef GECKO_SDK
    GECKO_SDK=$(HOME)/local/xulrunner2.0-sdk
endif
DLIBEXT=so

EXTEN_DIR=../extension/components

ZIP=zip

CXX=g++

# Works with regular g++.  Have to leave off the -bundle in the linker flags.


CPPFLAGS += $(shell R CMD config --cppflags ) \
          -fno-rtti  \
          -fno-exceptions  \
          -fPIC  \
	  -fshort-wchar \
          -gstabs+      \

$(warning "CPPFLAGS is " $(CPPFLAGS))

ifndef R_HOME
 R_HOME=$(shell R RHOME)
endif
XPIDL=$(GECKO_SDK)/bin/xpidl
GECKO_DEFINES=-DXPCOM_GLUE  -DXPCOM_GLUE_USE_NSPR -DMOZILLA_INTERNAL_API 


GECKO_CONFIG_INCLUDE = -include mozilla-config.h 
GECKO_INCLUDES = -I$(GECKO_SDK)/include -I$(GECKO_SDK)/include/xpcom -I$(GECKO_SDK)/include/content -I$(GECKO_SDK)/include/widget -I$(GECKO_SDK)/include/dom -I$(GECKO_SDK)/include/layout -I$(GECKO_SDK)/sdk/include  -I$(GECKO_SDK)/include/string
GECKO_LDFLAGS =  -L$(GECKO_SDK)/lib -L$(GECKO_SDK)/bin -Wl,-rpath-link,$(GECKO_SDK)/bin -lxpcomglue_s  -lxpcom \
                 -lnspr4      \
                 -lplds4  \
                 -lplc4 #\
		 -lxul

#                 -bundle
# Can't use bundle if we use /usr/local/bin/g++ and -dynamiclib

# plc4 is just for GetServiceManager at this point.

R_INCLUDES=-I$(R_HOME)/include
RFF_INCLUDES=-I../../src/
#NON-Portable!!!! came from .../<MozillaSourceBuild>/js/src/js-config --libs
#JS_LDFLAGS=-L/home/gmbecker/gabe/checkedout/Firefox192/objxulrunner-dbg/dist/lib -lmozjs -L/home/gmbecker/gabe/checkedout/Firefox192/obj-xulrunner-dbg/dist/lib -lplds4 -lplc4 -lnspr4 -lpthread -ldl -ldl -lm  -lm -ldl 
#JS_LDFLAGS=-L$(GECKO_SDK)/lib -lmozjs -lplds4 -lplc4 -lnspr4 \
	-lpthread -ldl -ldl -lm -lm -ldl
JS_LDFLAGS=-L$(GECKO_SDK)/lib -lxul -lplds4 -lplc4 -lnspr4 \
	-lpthread -ldl -ldl -lm -lm -ldl
R_LD_FLAGS=-L$(R_HOME)/lib -lR

LD_FLAGS=-shared -lmozalloc

NEW_LDFLAGS= -L$(GECKO_SDK)/lib -lnspr4 -lplc4 -lxpcom -lxul

NEW_LINKFLAGS=-Wl,-z,defs

NEW_LD_LOCS= -L$(GECKO_SDK)/bin -L$(GECKO_SDK)/lib -Wl,-rpath-link,$(GECKO_SDK)/bin

NEW_LD_LIBS= -lxpcom -lplc4 -lnspr4 -lxul

OBJ_FILES=Revaluator.o RevaluatorModule.o RVariants.o



default: extension
Revaluator.h: Revaluator.idl
Revaluator.o: Revaluator.h Revaluator.cpp
Revaluator.$(DLIBEXT): $(OBJ_FILES)

all:extension
testjsapiall : TEST_DEFINES = -DTEST_JSCONTEXT_EVAL -DTEST_JSCONTEXT_EVAL2 -DTEST_JSCONTEXT_FUNCALL
testjsapiall : extension
testjsapieval :TEST_DEFINES = -DTEST_JSCONTEXT_EVAL
testjsapieval : extension
testjsapieval2 : TEST_DEFINES = -DTEST_JSCONTEXT_EVAL -DTEST_JSCONTEXT_EVAL2 
testjsapieval2 : extension
testjsapifun :TEST_DEFINES=-DTEST_JSCONTEXT_FUNCALL
testjsapifun : extension

%.h: %.idl
	$(XPIDL) -m header -I$(GECKO_SDK)/idl $<


%.o: %.cpp Revaluator.h
	echo $(TEST_DEFINES)
	$(CXX)  -c -o $@ $(TEST_DEFINES) $(GECKO_CONFIG_INCLUDE) $(NEW_DEFINES) $(GECKO_INCLUDES) $(R_INCLUDES) $(RFF_INCLUDES) $(CPPFLAGS) $(CXXFLAGS) $<

Revaluator.$(DLIBEXT): $(OBJ_FILES) 
	$(CXX) -Wall -Os $^  -Bstatic $(R_LD_FLAGS) -Bdynamic -L$(GECKO_SDK)/lib -L$(GECKO_SDK)/bin -Wl,-rpath-link,$(GECKO_SDK)ma/bin  $(LD_FLAGS) $(NEW_LINKFLAGS) $(JS_LDFLAGS) -lxpcomglue_s -lxpcom -lnspr4 -ldl -o $@


# How to build the type table information from the idl.
%.xpt: %.idl
	$(XPIDL) -m typelib -I$(GECKO_SDK)/idl $<

extension: Revaluator.$(DLIBEXT) Revaluator.xpt
	cp Revaluator.$(DLIBEXT) $(EXTEN_DIR)/
	cp Revaluator.xpt $(EXTEN_DIR)/
	cd ../extension ; zip RFirefox.xpi * -r ; mv RFirefox.xpi ../
