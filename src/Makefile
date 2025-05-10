.PHONY: all clean

PROJECT_NAME = proj
BUILD_DIR = ../build

all:
	mkdir -p $(BUILD_DIR)
	cd $(BUILD_DIR) && qmake ../src/$(PROJECT_NAME).pro && $(MAKE)
	cp $(BUILD_DIR)/$(PROJECT_NAME) ../$(PROJECT_NAME)

clean:
	rm -rf $(BUILD_DIR)
	rm -f ../$(PROJECT_NAME)