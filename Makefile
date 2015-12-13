
CC = g++

bin/test: obj/main.o
	@mkdir -p bin
	$(CC) -Wall -Werror -o $@ obj/main.o

.PHONY: obj/main.o
obj/main.o: src/main.cpp
	@mkdir -p obj
	$(CC) -c -o $@ $< -Wall -Werror -fpic

.PHONY: install
install:
	@ln -f include/tree.h /usr/local/include

.PHONY: clean
clean:
	@rm -rf bin
	@rm -rf obj