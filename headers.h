#ifndef HEADERS_H
#define HEADERS_H

#define PATH_MAX 4096
#define PORT 80
#define BUFFER_SIZE 4096

#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "pwd.h"
#include <limits.h>
#include <errno.h>
#include "dirent.h"
#include <ctype.h>
#include "unistd.h"
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <signal.h>
#include <sys/select.h>
#include <termios.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <time.h>
#include <math.h>
#include <grp.h>
#include <arpa/inet.h>
#include <netinet/in.h> 
#include <netdb.h>        // for gethostbyname
#include <sys/socket.h>   


#include "prompt_display.h"
#include "hop.h"
#include "reveal.h"
#include "log.h"
#include "execute_commands.h"
#include "proclore.h"
#include "seek.h"
#include "redirection.h"
#include "activities.h"
#include "signals.h"
#include "fg_and_bg.h"
#include "neonate.h"
#include "man.h"
#include "pipes.h"



#define GREEN "\x1b[32m"
#define BLUE "\x1b[34m"
#define RED "\x1b[31m"
#define RESET "\x1b[0m"
#define YELLOW "\x1b[33m"

#endif
