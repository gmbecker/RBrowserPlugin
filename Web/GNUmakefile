PACKAGE=RFirefox

include ../../AdminTools/GNUmakefile

TARGET_DIR=www.omegahat.org:/home2/duncan/Omegaweb/$(PACKAGE)

FILES=AFAQ.txt macInstall.txt Installation.txt

EXAMPLE_HTML=exLinkedPlot exLogisticDynDoc exKSmoothDynDoc

EXAMPLE_R=exKSmoothDynDoc exLogisticDynDoc

TEST_HTML=raphaeltest callJSMethod tworaphtest

JAVASCRIPT=raphael Rhelpers Rscripthandler


ship: 
	scp $(EXAMPLE_HTML:%=../inst/examples/%.html) $(TEST_HTML:%=../inst/tests/%.html) $(TARGET_DIR)/examples/
	scp $(EXAMPLE_R:%=../inst/examples/%.R) $(TARGET_DIR)/examples/R/
	scp $(JAVASCRIPT:%=../inst/javascript/%.js) $(TARGET_DIR)/javascript/
	scp $(FILES:%=../%) index.html $(TARGET_DIR)

