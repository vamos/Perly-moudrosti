#	VUT FIT IMS Project: 
#
#	Modeling voles population in Cellular automaton
#
#	File: Makefile
#	Authors: David Vodak	(xvodak05)
#		 Libor Dvoracek	(xdvora2t)
#

CCC=g++
CCFLAGS=-std=C++14 -pedantic
SRC=src/main.cc src/cellular_automaton.cc src/debug.cc
HDR=src/cellular_automaton.h src/debug.h

all: $(HDR)
	$(CCC) $(CCLFLAGS) $(SRC) -o ca

debug: $(HDR)
	$(CCC) $(CCLFLAGS) $(SRC) -DDEBUG -o ca

gdb: $(HDR)
	$(CCC) $(CCLFLAGS) $(SRC) -g -ggdb -o ca

pack:
	zip xvodak05_xdvora2t.zip Makefile $(SRC) $(HDR) README.txt ${DOC}
