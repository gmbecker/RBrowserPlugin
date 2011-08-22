PACKAGE=RFirefox

#include ../../AdminTools/GNUmakefile

TARGET_DIR=www.omegahat.org:/home3/WebSites/Omega/$(PACKAGE)

FILES=AFAQ macInstall.txt linuxInstall.txt

EXAMPLE_HTML=exLinkedPlot exLogisticDynDoc exKSmoothDynDoc

EXAMPLE_R=exKSmoothDynDoc exLogisticDynDoc

TEST_HTML=raphaeltest callJSMethod tworaphtest invokeRCall

JAVASCRIPT=raphael Rhelpers


ship: 
	scp $(EXAMPLE_HTML:%=../inst/examples/%.html) $(TEST_HTML:%=../inst/tests/%.html) ../inst/examples/gradAdmisdat.csv $(TARGET_DIR)/examples/
	scp $(EXAMPLE_R:%=../inst/examples/%.R) $(TARGET_DIR)/examples/R/
	scp $(JAVASCRIPT:%=../inst/javascript/%.js) $(TARGET_DIR)/javascript/
	scp $(FILES:%=../%) index.html $(TARGET_DIR)/

dirs: 
	mkdir $(TARGET_DIR)/examples
	mkdir $(TARGET_DIR)/javascript
