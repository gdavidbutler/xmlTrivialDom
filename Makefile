SQLITE_INC=
SQLITE_LIB=-lsqlite3

XML_INC=-I../xmlTrivialCallbackParser
XML_OBJ=../xmlTrivialCallbackParser/xml.o

CFLAGS = $(SQLITE_INC) $(XML_INC) -I. -Os -g

all: xql.o main

clean:
	rm -f xql.str xql.o main

xql.str: str.sed xql.sql
	sed -f str.sed <xql.sql >xql.str

xql.o: xql.c xql.h xql.str
	cc $(CFLAGS) -c xql.c

main: test/main.c xql.o
	cc $(CFLAGS) -o main test/main.c xql.o $(XML_OBJ) $(SQLITE_LIB)

check: main
	echo '<a "b" c="d">1<e>2<f>3</f>4<f>5</f>6<g></g>7</e>8</a>' | ./main
