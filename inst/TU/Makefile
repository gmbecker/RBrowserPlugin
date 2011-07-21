XUL_SDK=$(HOME)/Projects/Mozilla/xulrunner-sdk
MOZ_DIST=$(HOME)/Projects/Mozilla/mozilla-1.9.1/dist

# Do we need nspr explicitly
GCC_FLAGS=-fshort-wchar
CXXFLAGS= -DXPCOM_GLUE $(GCC_FLAGS) -I$(XUL_SDK)/sdk/include -I$(XUL_SDK)/include 
CXXFLAGS= -DXPCOM_GLUE $(GCC_FLAGS) -I$(MOZ_DIST)/include/xpcom -I$(MOZ_DIST)/include -I$(XUL_SDK)/sdk/include -I$(XUL_SDK)/include

%.tu: %.cc
	$(CXX) -o /dev/null -c $(CXXFLAGS) -fdump-translation-unit $<


