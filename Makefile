SOURCE 	=	buffer.c index.c graph.c main.c queue.c stack.c ConnectedComponents.c SCC.c hashTable.c JobScheduler.c JobQueue.c
OBJS 	= 	buffer.o index.o graph.o queue.o stack.o ConnectedComponents.o SCC.o hashTable.o JobScheduler.o JobQueue.o main.o
OUT		= 	project 
HEADER	=	header.h buffer.h index.h graph.h queue.h stack.h ConnectedComponents.h SCC.h hashTable.h JobScheduler.h JobQueue.h
FLAGS	=	-c -g -O3


all:	$(OBJS)	$(HEADER)
	gcc $(OBJS) -pthread -lm -O3 -o project

main.o:		main.c 		$(HEADER)
	gcc	$(FLAGS)	main.c	

JobScheduler.o:		JobScheduler.c 	$(HEADER)
	gcc	$(FLAGS) 	JobScheduler.c

JobQueue.o:		JobQueue.c $(HEADER)
	gcc	$(FLAGS) 	JobQueue.c

hashTable.o:	hashTable.c $(HEADER)
	gcc	$(FLAGS)	hashTable.c

queue.o:	queue.c 	$(HEADER)
	gcc	$(FLAGS)	queue.c

stack.o:	stack.c 	$(HEADER)
	gcc	$(FLAGS) 	stack.c

buffer.o:	buffer.c 	$(HEADER)
	gcc	$(FLAGS)	buffer.c

index.o:	index.c		$(HEADER)
	gcc	$(FLAGS)	index.c
 
graph.o:	graph.c		$(HEADER)
	gcc	$(FLAGS)	graph.c

ConnectedComponents.o:	ConnectedComponents.c 		$(HEADER)
	gcc	$(FLAGS)	ConnectedComponents.c

SCC.o:	SCC.c 	$(HEADER)
	gcc	$(FLAGS) 	SCC.c




clean:
	rm $(OBJS)	$(OUT)

