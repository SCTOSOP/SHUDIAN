PATHBUUILD = ./build
PATHHAEAD = ./head
PATHSRC = ./src
SOURCE = $(PATHSRC)/main.cpp \
$(PATHSRC)/file.cpp \
$(PATHSRC)/interpreter.cpp

all: main

rebuild: clean all

clean:
	rm -r (PATHBUUILD)

main: $(SOURCE)
	mkdir -p ./build
	g++ -g -I $(PATHHAEAD) -o $(PATHBUUILD)/main $(SOURCE) --std=c++20
	$(PATHBUUILD)/main

test: all
	lldb $(PATHBUUILD)/main