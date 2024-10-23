CFLAGS=-Wall -Werror -g  -fdiagnostics-color=always 

# Include this to see preprocessor output
# CFLAGS+=-save-temps

TARGETS = helloWorld

all: helloWorld

helloWorld: helloWorld.o functionFile.o
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.c
	$(CC) -c $(CFLAGS) $<

clean:
	rm -f $(TARGETS) *.o *.i

