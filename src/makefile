SUBDIRS=$(shell ls -l | grep ^d | awk '{if($$9 != "debug") print $$9}')
all:$(SUBDIRS) 
$(SUBDIRS):ECHO
	make -C $@
DEBUG:ECHO
	make -C debug
ECHO:
	@echo $(SUBDIRS)
