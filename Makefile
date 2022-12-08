CFLAGS = -Wall -fPIC -O2
OBJECTS = src/fdpass.o

.PHONY: all
all: $(OBJECTS)

.PHONY: clean
clean:
	$(RM) $(OBJECTS)

src/%.o: src/%.c
	$(CC) $(CFLAGS) -c $< -o $@
