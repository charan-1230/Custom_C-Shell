
CC = gcc

LDFLAGS = -lm

SRCS = main.c prompt_display.c hop.c reveal.c log.c execute_commands.c proclore.c seek.c redirection_tokenise.c redirection.c activities.c signals.c fg_and_bg.c neonate.c man.c

TARGET = myshell

$(TARGET):
	$(CC) $(SRCS) -o $(TARGET) $(LDFLAGS)

clean:
	rm -f $(TARGET) 


