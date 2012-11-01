all: banker

OBJS = banker.o 
LIBS= -lpthread -lm

banker: banker.o ${OBJS}
	gcc -o $@  ${LIBS} ${OBJS}

clean:
	rm -rf *o banker
