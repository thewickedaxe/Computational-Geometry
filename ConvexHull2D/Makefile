TRIANGULATION_TARGET=Triangulate
CONVEXHULL2D_TARGET=ConvexHull2D

TRIANGULATION_SOURCE=Triangulation/Triangulation.cpp
CONVEXHULL2D_SOURCE=ConvexHull2D/ConvexHull2D.cpp

CFLAGS += -fpermissive -fopenmp -Wno-deprecated -Wno-write-strings -msse2 -std=c++11
LFLAGS += -lgomp

CFLAGS_DEBUG = -DDEBUG -g3
LFLAGS_DEBUG =

CFLAGS_RELEASE = -O3 -DRELEASE -funroll-loops -ffast-math
LFLAGS_RELEASE = -O3 

SRC = ./
BIN = Bin/Linux/
BIN_O = ./
INCLUDE = /usr/include/ -I. -IInclude -ISource

CC=gcc
CXX=g++
MD=mkdir

TRIANGULATION_OBJECTS=$(addprefix $(BIN_O), $(addsuffix .o, $(basename $(TRIANGULATION_SOURCE))))
CONVEXHULL2D_OBJECTS=$(addprefix $(BIN_O), $(addsuffix .o, $(basename $(CONVEXHULL2D_SOURCE))))


all: CFLAGS += $(CFLAGS_RELEASE)
all: LFLAGS += $(LFLAGS_RELEASE)
all: $(BIN)$(TRIANGULATION_TARGET)
all: $(BIN)$(CONVEXHULL2D_TARGET)

debug: CFLAGS += $(CFLAGS_DEBUG)
debug: LFLAGS += $(LFLAGS_DEBUG)
debug: $(BIN)$(TRIANGULATION_TARGET)
debug: $(BIN)$(CONVEXHULL2D_TARGET)

clean:
	rm -f $(BIN)$(TRIANGULATION_TARGET)
	rm -f $(BIN)$(CONVEXHULL2D_TARGET)

$(BIN):
	mkdir -p $(BIN)

$(BIN)$(TRIANGULATION_TARGET): $(TRIANGULATION_OBJECTS)
	mkdir -p $(BIN)
	$(CXX) -o $@ $(TRIANGULATION_OBJECTS) $(LFLAGS)

$(BIN)$(CONVEXHULL2D_TARGET): $(CONVEXHULL2D_OBJECTS)
	mkdir -p $(BIN)
	$(CXX) -o $@ $(CONVEXHULL2D_OBJECTS) $(LFLAGS)

$(BIN_O)%.o: $(SRC)%.c
	$(CC) -c -o $@ $(CFLAGS) -I$(INCLUDE) $<

$(BIN_O)%.o: $(SRC)%.cpp
	$(CXX) -c -o $@ $(CFLAGS) -I$(INCLUDE) $<

