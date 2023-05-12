
export BUILD_DIR ?= $(CURDIR)/build

all: driver app
.PHONY: driver app

driver:
	make -C driver

app:
	make -C app
