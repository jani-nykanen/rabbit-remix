#
# Makefile
# (c) 2019 Jani Nykänen

# ------------------------------------------------------- #

#
# Game
#

SRC := $(wildcard src/*.c src/*/*.c src/*/*/*.c)
OBJ := $(patsubst %.c, %.o, $(SRC))

LD_FLAGS :=  lib/libengine.a -lSDL2 -lm -I ./include
CC_FLAGS :=  -Iinclude -Wall #-O3

all: game clean

clean:
	find ./src -type f -name '*.o' -delete

%.o: %.c
	$(CC) -c -o $@ $< -I./include

game: $(OBJ)
	gcc $(CC_FLAGS) -o $@ $^ $(LD_FLAGS)

run:
	./game

# ------------------------------------------------------- #

#
# Engine
# 

ENGINE_SRC := $(wildcard engine/src/*.c)
ENGINE_OBJ := $(patsubst engine/src/%.c, engine/src/%.o, $(ENGINE_SRC))

engine: libengine.a clean_engine

libengine.a: $(ENGINE_OBJ)
	ar rcs libengine.a $(ENGINE_OBJ)

clean_engine:
	find ./engine -type f -name '*.o' -delete

install:
	make install_debug
	rm libengine.a
	make clean_engine

install_debug: libengine.a
	install -d ./lib/
	install -m 644 libengine.a ./lib/
	install -d ./include/engine/
	install -m 644 $(wildcard engine/src/*.h) ./include/engine/

# ------------------------------------------------------- #

#
# Other
#
test:
	make install_debug
	make game
	make run

clean_all:
	make clean
	rm libengine.a
	make clean_engine
