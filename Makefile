CPPFLAGS = -Isrc/include
CXXFLAGS = -std=c++20 -O2 -Wall -Wextra -flto=4 -fstrict-aliasing
LDFLAGS = -flto=4

Headers = $(wildcard src/include/*.hh)
Sources = $(wildcard src/lib/*.cc)
Objects = $(Sources:.cc=.o)

project: $(Headers:=.gch) $(Objects)
	$(CXX) $(LDFLAGS) -o $@ $(Objects)

%.gch: %
	$(CXX) $<
