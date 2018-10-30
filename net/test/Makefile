#Makefile for net test

CC := g++

EXTRA_FLAGS := -g -Wall -O0 -std=c++11

DEP_LIBS := -lglog -lgtest_main -lgtest -lpthread

CHANNEL_SRCS := ../easynet_channel.cc \
	../easynet_eventloop.cc \
	../easynet_epollpoller.cc \
	../easynet_timer.cc \
	../easynet_timerid.cc \
	easynet_channel_test.cc

BUFFER_SRCS := ../easynet_buffer.cc \
	easynet_buffer_test.cc

TIMER_SRCS := ../easynet_eventloop.cc \
	../easynet_channel.cc \
	../easynet_epollpoller.cc \
    ../easynet_timer.cc \
	../easynet_timerid.cc \
    easynet_timer_test.cc

TCPSERVER_SRCS := ../easynet_eventloop.cc \
    ../easynet_channel.cc \
    ../easynet_epollpoller.cc \
    ../easynet_acceptor.cc \
    ../easynet_timer.cc \
    ../easynet_timerid.cc \
    ../easynet_tcpconnection.cc \
    ../easynet_tcpserver.cc \
    ../easynet_socket.cc \
    ../easynet_buffer.cc \
    ./easynet_tcpserver_test.cc

EXTRA_INCLUDES := -I../ \
	-I../../deps/gtest/include/ \
	-I../../deps/gmock/include/ \
	-I../../sys/

DEP_LIB_PATHS := -L../../deps/gtest/libs \
	-L../../deps/gmock/libs/ \
	-L/usr/local/lib/

CHANNEL_OBJS := $(CHANNEL_SRCS:.cc=.o)

BUFFER_OBJS := $(BUFFER_SRCS:.cc=.o)

TIMER_OBJS := $(TIMER_SRCS:.cc=.o)

TCPSERVER_OBJS := $(TCPSERVER_SRCS:.cc=.o)

all : easynet_channel_test easynet_buffer_test easynet_timer_test easynet_tcpserver_test
.PHONY : all

easynet_channel_test : $(CHANNEL_OBJS)
	@echo "Building "$@
	$(CC) $(DEP_LIB_PATHS) $(EXTRA_FLAGS) $^ -o $@ $(DEP_LIBS)

easynet_buffer_test : $(BUFFER_OBJS)
	@echo "Building "$@
	$(CC) $(DEP_LIB_PATHS) $(EXTRA_FLAGS) $^ -o $@ $(DEP_LIBS)

easynet_timer_test : $(TIMER_OBJS)
	@echo "Building "$@
	$(CC) $(DEP_LIB_PATHS) $(EXTRA_FLAGS) $^ -o $@ $(DEP_LIBS)

easynet_tcpserver_test : $(TCPSERVER_OBJS)
	@echo "Building "$@
	$(CC) $(DEP_LIB_PATHS) $(EXTRA_FLAGS) $^ -o $@ $(DEP_LIBS)

%.o : %.cc
	@echo "Building "$<
	@$(CC) -c $(EXTRA_FLAGS) $(EXTRA_INCLUDES) $< -o $@

clean:
	find ../ -name "*.o" | xargs rm
	-rm -f easynet_channel_test
	-rm -f easynet_buffer_test
	-rm -f easynet_timer_test
	-rm -f easynet_tcpserver_test
