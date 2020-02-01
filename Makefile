# Makefile, versao 1
# Sistemas Operativos, DEI/IST/ULisboa 2018-19
SOURCES= CircuitRouter-SeqSolver/router.c CircuitRouter-SeqSolver/maze.c CircuitRouter-SeqSolver/grid.c CircuitRouter-SeqSolver/coordinate.c CircuitRouter-SeqSolver/CircuitRouter-SeqSolver.c
SOURCES+= lib/vector.c lib/queue.c lib/list.c lib/pair.c lib/commandlinereader.c
SOURCES2= CircuitRouter-AdvShell/CircuitRouter-AdvShell.c
SOURCES2+= lib/vector.c lib/commandlinereader.c
SOURCES3= CircuitRouter-Client/CircuitRouter-Client.c
SOURCES3+= lib/commandlinereader.c
OBJS = $(SOURCES:%.c=%.o)
OBJS2 = $(SOURCES2:%.c=%.o)
OBJS3 = $(SOURCES3:%.c=%.o)
CC   = gcc
CFLAGS =-Wall -std=gnu99
LDFLAGS=-lm
LDFLAGS2=-lm
LDFLAGS3=-lm
TARGET = CircuitRouter-SeqSolver/CircuitRouter-SeqSolver
TARGET2 = CircuitRouter-AdvShell/CircuitRouter-AdvShell
TARGET3 = CircuitRouter-Client/CircuitRouter-Client

all: $(TARGET) $(TARGET2) $(TARGET3)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $^ -o $(TARGET) $(LDFLAGS)

$(TARGET2): $(OBJS2)
	$(CC) $(CFLAGS) $^ -o $(TARGET2) $(LDFLAGS2)

$(TARGET3): $(OBJS3)
	$(CC) $(CFLAGS) $^ -o $(TARGET3) $(LDFLAGS3)

CircuitRouter-Client/CircuitRouter-Client.o: CircuitRouter-Client/CircuitRouter-Client.c lib/commandlinereader.c
CircuitRouter-AdvShell/CircuitRouter-AdvShell.o: CircuitRouter-AdvShell/CircuitRouter-AdvShell.c CircuitRouter-AdvShell/CircuitRouter-AdvShell.h lib/vector.h lib/commandlinereader.h
CircuitRouter-SeqSolver/CircuitRouter-SeqSolver.o: CircuitRouter-SeqSolver/CircuitRouter-SeqSolver.c CircuitRouter-SeqSolver/maze.h CircuitRouter-SeqSolver/router.h lib/list.h lib/timer.h lib/types.h lib/vector.h
CircuitRouter-SeqSolver/router.o: CircuitRouter-SeqSolver/router.c CircuitRouter-SeqSolver/router.h CircuitRouter-SeqSolver/coordinate.h CircuitRouter-SeqSolver/grid.h lib/queue.h lib/vector.h
CircuitRouter-SeqSolver/maze.o: CircuitRouter-SeqSolver/maze.c CircuitRouter-SeqSolver/maze.h CircuitRouter-SeqSolver/coordinate.h CircuitRouter-SeqSolver/grid.h lib/list.h lib/queue.h lib/pair.h lib/types.h lib/vector.h
CircuitRouter-SeqSolver/grid.o: CircuitRouter-SeqSolver/grid.c CircuitRouter-SeqSolver/grid.h CircuitRouter-SeqSolver/coordinate.h lib/types.h lib/vector.h
CircuitRouter-SeqSolver/coordinate.o: CircuitRouter-SeqSolver/coordinate.c CircuitRouter-SeqSolver/coordinate.h lib/pair.h lib/types.h
lib/vector.o: lib/vector.c lib/vector.h lib/types.h lib/utility.h
lib/queue.o: lib/queue.c lib/queue.h lib/types.h
lib/list.o: lib/list.c lib/list.h lib/types.h
lib/pair.o: lib/pair.c lib/pair.h
lib/commandlinereader.o: lib/commandlinereader.c lib/commandlinereader.h
lib/timer.o: lib/timer.h
lib/utillity.o: lib/utillity.h

$(OBJS):
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	@echo Cleaning...
	rm -f $(OBJS) $(TARGET) $(OBJS2) $(TARGET2) $(OBJS3) $(TARGET3)
