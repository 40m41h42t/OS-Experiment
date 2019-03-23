TARGET=test

C_FILES=$(shell ls tasks/*.c)
BASE = $(basename $(C_FILES))
OBJS = $(addsuffix .o, $(addprefix bin/, $(BASE)))

# TODO: finish Makefile