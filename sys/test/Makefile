# Makefile for sys test

CC := g++

EXTRA_FLAGS := -Wall -O0 -g -std=c++11

DEP_LIBS := -pthread -lglog -lgtest_main -lgtest

SRCS := ../easynet_thread.cc \
	easynet_thread_test.cc

EXTRA_INCLUDES := -I../ \
	-I../../deps/gtest/include \
	-I../../deps/gmock/include

EXTRA_DEP_LIB_PATH := -L../../deps/gtest/libs \
	-L../../deps/gmock/libs \
	-L/usr/local/lib/

OBJS := $(SRCS:.cc=.o)

all : easynet_thread_test
.PHONY : all

easynet_thread_test: $(OBJS)
	@echo "Building "$(PROJECT_NAME)
	@$(CC) $(DEP_LIBS) $(EXTRA_FLAGS) $(EXTRA_DEP_LIB_PATH) $^ -o $@

%.o : %.cc
	@echo "Building "$<
	@$(CC) -c $(EXTRA_FLAGS) $(EXTRA_INCLUDES) $< -o $@

clean:
	find ../ -name "*.o" | xargs rm
	rm -f $(PROJECT_NAME)