CPPFLAGS = -Isrc/include
CXXFLAGS = -std=c++17 -O2 -Wall -Wextra -flto -fstrict-aliasing
LDFLAGS = -flto

Sources = $(wildcard src/lib/*.cc)
Objects = $(Sources:.cc=.o)

project: $(Objects)
	$(CXX) $(LDFLAGS) -o $@ $^
