# Pointer to AMPS C++ client. Override by setting an AMPS_CLIENT env variable before building.
AMPS_CLIENT ?= $(HOME)/clients/cpp
CXXFLAGS= -std=c++0x -O3 -I$(AMPS_CLIENT)/include
LINK.o = $(LINK.cc)
LDLIBS=-L$(AMPS_CLIENT)/lib/x86_64/release -lamps -lpthread -lrt

TARGETS=q_publisher q_subscriber q_clean

all: $(TARGETS)

.PHONY: clean

clean:
	rm -f $(TARGETS)
