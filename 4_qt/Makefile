src = $(wildcard *.cpp)
obj = $(patsubst %.cpp,%.o,$(src))

LDFLAGS =

main: $(obj)
	$(CXX) -std=c++14 -o $@ $^ $(LDFLAGS)

.PHONY: clean
clean:
	rm -f $(obj) main
