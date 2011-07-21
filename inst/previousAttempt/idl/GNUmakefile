XPIDL=xpidl
XULRUNNER_SDK=/Users/duncan/Projects/Mozilla/xulrunner-sdk
XPT_DIR=../components

HEADERS=$(patsubst %.idl, ../include/%.h, $(wildcard *.idl))

all: $(HEADERS)

../include/%.h:	%.idl
	$(XPIDL) -m header -w -v -I$(XULRUNNER_SDK)/idl -e $@ $<

$(XPT_DIR)/%.xpt:	%.idl
	$(XPIDL) -m typelib -w -v -I$(XULRUNNER_SDK)/idl -e $@ $<
