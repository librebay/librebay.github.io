PACKAGE = md2html
SOURCES = md2html.c
OBJS    = ${SOURCES:.c=.o}
CFLAGS  = -Wall `pkg-config glib-2.0 --cflags`
LDADD   = `pkg-config glib-2.0 --libs`
CC      = gcc

all : ${OBJS}
	${CC} -o ${PACKAGE} ${OBJS} ${LDADD}

.c.o:
	${CC} ${CFLAGS} -c $<

clean:
	rm ${PACKAGE} ${OBJS}
