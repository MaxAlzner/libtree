
CC = g++ -g

bin/test: clean src/main.cpp
	@mkdir -p bin
	$(CC) -Wall -Werror -o $@ src/main.cpp

.PHONY: install
install:
	@ln -f include/* /usr/local/include

.PHONY: clean
clean:
	@rm -rf bin