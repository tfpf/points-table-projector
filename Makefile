CXXFLAGS = -std=c++17 -O2 -Wall -Wextra -flto -fstrict-aliasing

Sources = $(wildcard lib/*.cc)
Objects = $(Sources:.cc=.o)

project: $(Objects)
	$(LINK.cc) -o $@ $^
