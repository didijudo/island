CC = g++
#CFLAGS = -Wall
PROG = island

SRCS = main.cpp imageloader.cpp md2model.cpp text3d.cpp vec3f.cpp Terrain.cpp animal.cpp zebra.cpp lion.cpp quadtree.cpp carregadorObj.cpp

ifeq ($(shell uname),Darwin)
	LIBS = -framework OpenGL -framework GLUT
else
	LIBS = -lglut
endif

all: $(PROG)

$(PROG):	$(SRCS)
	$(CC) $(CFLAGS) -o $(PROG) $(SRCS) $(LIBS)

clean:
	rm -f $(PROG)
