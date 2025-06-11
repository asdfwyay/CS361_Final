STD=--std=c++17 -pthread
GCC=g++
OBJ=obj
BIN=bin

bin/fourofakind: src/main.cpp src/t_stack.h
	[ -d $(BIN) ] || mkdir -p $(BIN)
	${GCC} ${STD} -o bin/fourofakind src/main.cpp
	
run: bin/fourofakind
	./bin/fourofakind

.PHONY: doc
doc:
	doxygen config
	cd latex && make

clean:
	rm -f bin/fourofakind
	rm -r -f bin
	rm -r -f obj
	rm -r -f html
	rm -r -f latex