#  List of all programs you want to build
EX=client server

#  Main target
all: $(EX)

#  Generic compile rules
.c.o:
	gcc $< -o $@
