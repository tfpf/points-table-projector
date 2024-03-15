CPPFLAGS = -Isrc/include
CXXFLAGS = -std=c++17 -O2 -Wall -Wextra -flto -fstrict-aliasing
LDFLAGS = -flto

Headers = $(wildcard src/include/*.hh)
Sources = $(wildcard src/lib/*.cc)
Objects = $(Sources:.cc=.o)

project: $(Headers) $(Objects)
	$(CXX) $(LDFLAGS) -o $@ $(Objects)
