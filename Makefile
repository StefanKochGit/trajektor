SUBDIRS      = src testcases
     
.PHONY: subdirs $(SUBDIRS) lib

all: mkdirs subdirs

subdirs: $(SUBDIRS)
     
$(SUBDIRS):
	$(MAKE) -C $@

mkdirs:
	mkdir -p lib

clean:
	rm -rf lib
	for dir in $(SUBDIRS); do \
		$(MAKE) -C $$dir clean; \
	done

