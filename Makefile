.SUFFIXES: .c .o
CC = gcc
EXEC = ray
CCFLAGS = -g -Wall -std=c99
OBJS = vec.o raytracer.o

${EXEC}: ${OBJS}
	${CC} ${CCFLAGS} -o ${EXEC} ${OBJS} -lm

.c.o:
	${CC} ${CCFLAGS} -c $<

run: ${EXEC}
	./${EXEC}
    
clean:
	rm -f ${EXEC} ${OBJS} *.png

raytracer.o: raytracer.c
vec.o: vec.c
