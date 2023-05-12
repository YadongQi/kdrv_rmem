
export BUILD_DIR ?= $(CURDIR)/build

all: driver app dump_pt
.PHONY: driver app dump_pt

driver:
	make -C driver

app:
	make -C app

dump_pt:
	make -C dump_pt
