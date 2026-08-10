SRCS := $(shell find src -name '*.c')
OBJS := $(patsubst src/%.c, build/%.o, $(SRCS))

lc3vm: $(OBJS)
	gcc -g -o lc3vm $(OBJS)

build/%.o: src/%.c
	mkdir -p $(@D)
	gcc -c $< -o $@

clean:
	rm -f lc3vm
	rm -rf build
