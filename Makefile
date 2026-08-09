lc3vm: build/main.o
	gcc -g -o lc3vm build/main.o

build/%.o: src/%.c
	mkdir -p $(@D)
	gcc -c $^ -o $@

clean:
	rm lc3vm
	rm build/*
