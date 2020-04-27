CC= gcc
GDB= -g
LIBFLAGS= -pthread
DEPS= List.h Helpers.h SocketMetaData.h
OBJ= s-talk.o List.o Helpers.o SocketMetaData.o

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(GDB)

s-talk: $(OBJ)
	$(CC) -o $@ $^ $(LIBFLAGS)
	
clean:
	rm *.o s-talk