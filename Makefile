MAKE := make

DIRS := main tlu vme ni eudrb mvd depfet mimoroma taki root gui fortis doc

default: main

all: $(DIRS)

$(DIRS:main=): main

eudrb mvd: vme

$(DIRS):
	$(MAKE) -C $@

clean:
	@for d in $(DIRS) bin; do $(MAKE) -C "$$d" clean; done

.PHONY: default all clean $(DIRS)
