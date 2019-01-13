CC := g++
CXX_FLAGS := -Wall -g -O0 -std=c++11
CXX_EXTRA_INCLUDES := -I../../ \
	-I../../../sys/
CXX_EXTRA_LIBS := -lglog -lpthread
CXX_EXTRA_LIBS_PATH := -L./

SERVER_SRC_LST := echo_server.cc
CLIENT_SRC_LST := echo_client.cc

SERVER_OBJ_LST := $(SERVER_SRC_LST:.cc=.o)
CLIENT_OBJ_LST := $(CLIENT_SRC_LST:.cc=.o)

all : echo_server echo_client

echo_server : $(SERVER_OBJ_LST)
	$(CC) $(CXX_FLAGS) $^ -o $@ libeasynet.a $(CXX_EXTRA_LIBS)

echo_client : $(CLIENT_OBJ_LST)
	$(CC) $(CXX_FLAGS) $^ -o $@ libeasynet.a $(CXX_EXTRA_LIBS)

%.o : %.cc
	$(CC) -c $(CXX_FLAGS) $(CXX_EXTRA_INCLUDES) $< -o $@

clean :
	find . -name "*.o" | xargs rm -f
	-rm -f echo_server echo_client
