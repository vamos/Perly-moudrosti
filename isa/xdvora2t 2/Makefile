.PHONY: clean

CXX=g++
#CXXFLAGS= -std=c++11 -pg
CXXFLAGS = -std=c++11 -Wl,--no-as-needed -lprofiler -Wl,--as-needed -ltcmalloc
GPRFLAGS = -std=c++11 -pg
OBJECTS = isa-tazatel.o

main: $(OBJECTS)
	$(CXX) $(CXXFLAGS) -o isa-tazatel isa-tazatel.cpp -lresolv

# gprofiler flags
# after make run once ./isa-tazatel
# then
# gprof isa-tazatel > [output_filename]
gprof: $(OBJECTS)
	$(CXX) $(GPRFLAGS) -o isa-tazatel isa-tazatel.cpp -lresolv

$(OBJECTS): isa-tazatel.h 

clean:
	-rm $(OBJECTS)
