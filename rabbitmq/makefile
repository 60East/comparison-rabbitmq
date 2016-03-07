RABBIT_DIR ?= $(HOME)/rabbitmq-c-client
CXXFLAGS=-O3 -std=c++0x -I$(RABBIT_DIR)/include
LDLIBS=-L$(RABBIT_DIR)/lib64 -lrabbitmq -Wl,-rpath=$(RABBIT_DIR)/lib64

TARGETS=amqp_publisher amqp_subscriber

all: $(TARGETS)

clean:
	rm -rf $(TARGETS)

