OBJS = HelloGLSL.cpp obj_loader.cpp Camera2.cpp

CC = g++

COMPILER_FLAGS = -std=c++11 -w -Werror -Wno-deprecated

LINKER_FLAGS = -framework OpenGL -framework Cocoa -framework GLUT

OBJ_NAME = exec

all : $(OBJS)
	$(CC) $(OBJS) $(COMPILER_FLAGS) $(LINKER_FLAGS) -o $(OBJ_NAME)
