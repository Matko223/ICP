.PHONY: all clean run doxygen pack

PROJECT_NAME = proj
BUILD_DIR = build
SRC_DIR = src
DOC_DIR = doc
ARCHIVE_NAME = xpalesr00-xsedot00-xvalapm00.zip

UNAME := $(shell uname -s)
ifeq ($(UNAME),Linux)
    QMAKE := /usr/local/share/Qt-5.5.1/5.5/gcc_64/bin/qmake
else
    QMAKE := qmake
endif

all:
	mkdir -p $(BUILD_DIR)
	cd $(BUILD_DIR) && $(QMAKE) ../$(SRC_DIR)/$(PROJECT_NAME).pro && $(MAKE)
	cp $(BUILD_DIR)/$(PROJECT_NAME) $(PROJECT_NAME)

run: all
	./$(PROJECT_NAME)

doxygen:
	mkdir -p $(DOC_DIR)
	doxygen doc/Doxyfile

clean:
	rm -rf $(BUILD_DIR)
	rm -rf $(DOC_DIR)/html
	rm -f $(PROJECT_NAME)
	$(MAKE) -C $(SRC_DIR) clean

pack:
	zip -r $(ARCHIVE_NAME) $(SRC_DIR)/ examples/ README.txt Makefile doc/