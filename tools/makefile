SRC := $(wildcard src/*.c src/*/*.c src/*/*/*.c)
OBJ := $(patsubst %.c, %.o, $(SRC))

LD_FLAGS :=  -lSDL2 -lSDL2_image -lm
CC_FLAGS :=  -Iinclude -Wall #-O3

png2bin:
	if [ ! -d "bin" ]; then \
		mkdir bin; \
	fi
	gcc -Wall src/png2bin.c -o ./bin/$@ $(LD_FLAGS)
