CFLAGS = -I. -g

#$(CC) -o mybc main.o lexer.o parser.o

mybc: main.o lexer.o parser.o
	$(CC) -o $@ $^

clean:
	$(RM) *.o
