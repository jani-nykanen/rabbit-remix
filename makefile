#
# Project "Fungus 2" makefile
# (c) 2019 Jani Nykänen

# ------------------------------------------------------- #

#
# Game
#

SRC := $(wildcard src/*.c src/*/*.c src/*/*/*.c)
OBJ := $(patsubst %.c, %.o, $(SRC))

LD_FLAGS :=  lib/libengine.a -lSDL2 -lm -I ./include
CC_FLAGS :=  -Iinclude -Wall #-O3

all: fungus clean

clean:
	find ./src -type f -name '*.o' -delete

%.o: %.c
	$(CC) -c -o $@ $< -I./include

fungus: $(OBJ)
	gcc $(CC_FLAGS) -o $@ $^ $(LD_FLAGS)

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

install: libengine.a
	install -d ./lib/
	install -m 644 libengine.a ./lib/
	install -d ./include/engine/
	install -m 644 $(wildcard engine/src/*.h) ./include/engine/
	rm libengine.a
	make clean_engine

# ------------------------------------------------------- #
