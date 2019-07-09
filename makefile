CC=g++
SUBDIRS=src/
ROOT_DIR=$(shell pwd)
BIN=myapp
OBJS_DIR=debug/obj
BIN_DIR=debug/bin
CUR_SOURCE=${wildcard *.c}
CUR_OBJS=${patsubst %.c, %.o, $(CUR_SOURCE)}
CXXFLAGS=-g -std=c++11
export CC BIN OBJS_DIR BIN_DIR ROOT_DIR
all:$(SUBDIRS) $(CUR_OBJS) DEBUG
$(SUBDIRS):ECHO
	make -C $@
DEBUG:ECHO
	make -C debug/obj
ECHO:
	@echo $(SUBDIRS)
$(CUR_OBJS):%.o:%.c
	$(CC) -c $^ -o $(ROOT_DIR)/$(OBJS_DIR)/$@ $(CXXFLAGS)
CLEAN:
	@rm $(OBJS_DIR)/*.o
	@rm -rf $(BIN_DIR)/*