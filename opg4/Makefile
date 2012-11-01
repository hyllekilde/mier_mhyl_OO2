all: banker

OBJS = banker.o 
LIBS= -lpthread

banker: banker.o ${OBJS}
	gcc -o $@  ${LIBS} ${OBJS}

clean:
	rm -rf *o banker
