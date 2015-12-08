APP=eegpower
CC=gcc
CFLAGS=-lm -g
LIBS=-lbluetooth -ljack
SRC= main.c ThinkGearStreamParser.c
OBJ=main.o ThinkGearStreamParser.o

%.o: %.c
	$(CC) -c $< -o $@

$(APP): $(OBJ)
	$(CC) $(CFLAGS) -o $(APP) $(OBJ) $(LIBS)

clean:
	rm *.o $(APP)
run:
		./eegpower
