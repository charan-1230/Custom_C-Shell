
CC = gcc

LDFLAGS = -lm

SRCS = main.c 1.c 3.c 4.c 5.c 6.c 7.c 8.c 10_redirection_tokenise.c 10_redirection.c 13_activities.c 14_signals.c 15_fgandbg.c 16_neonate.c 17_man.c

TARGET = myprogram

$(TARGET):
	$(CC) $(SRCS) -o $(TARGET) $(LDFLAGS)

clean:
	rm -f $(TARGET) 


