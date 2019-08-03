#
# Makefile
# (c) 2019 Jani Nykänen

# ------------------------------------------------------- #

#
# Game
#

SRC := $(wildcard src/*.c src/*/*.c src/*/*/*.c)
OBJ := $(patsubst %.c, %.o, $(SRC))

LD_FLAGS :=  lib/libengine.a lib/libleaderboard.a -lSDL2 -lm -lcurl -lssl -lcrypto -I ./include 
CC_FLAGS :=  -Iinclude -Wall # -O3

all: game clean

clean:
	find ./src -type f -name '*.o' -delete

%.o: %.c
	$(CC) -c -o $@ $< -I./include $(DEFINES)

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

install_engine: libengine.a
	install -d ./lib/
	install -m 644 libengine.a ./lib/
	install -d ./include/engine/
	install -m 644 $(wildcard engine/src/*.h) ./include/engine/
	rm libengine.a
	make clean_engine

# ------------------------------------------------------- #

#
# Leaderboard
#

LB_SRC := $(wildcard leaderboard/src/*.c)
LB_OBJ := $(patsubst leaderboard/src/%.c, leaderboard/src/%.o, $(LB_SRC))

leaderboard: libleaderboard.a clean_lb

libleaderboard.a: $(LB_OBJ)
	ar rcs libleaderboard.a $(LB_OBJ)

clean_lb:
	find ./leaderboard -type f -name '*.o' -delete

install_lb: libleaderboard.a
	install -d ./lib/
	install -m 644 libleaderboard.a ./lib/
	install -d ./include/leaderboard/
	install -m 644 $(wildcard leaderboard/src/*.h) ./include/leaderboard/
	rm libleaderboard.a
	make clean_lb


# ------------------------------------------------------- #

#
# Other
#

install:
	make install_lb
	make install_engine

clean_all:
	make clean
	rm libengine.a
	make clean_engine
