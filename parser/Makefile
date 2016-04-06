OBJS = parser.c
CC = gcc 

#INCLUDE_PATHS = -ID:\AppleCakes\MinGW\include\SDL2 
#LIBRARY_PATHS = -LD:\AppleCakes\MinGW\lib 

COMPILER_FLAGS = -Wall -Wextra -Wpedantic -std=c99 -g -Wl,--stack,16777216

#LINKER_FLAGS = -lmingw32 -lSDL2main -lSDL2 

OBJ_NAME = parser.out

all : $(OBJS) 
	$(CC) $(OBJS) $(COMPILER_FLAGS) -o $(OBJ_NAME)
