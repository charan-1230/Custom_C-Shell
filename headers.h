#ifndef HEADERS_H
#define HEADERS_H

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

#include "1.h"
#include "3.h"
#include "4.h"
#include "5.h"
#include "6.h"
#include "7.h"
#include "8.h"
#include "10_redirection.h"
#include "11_pipe.h"
#include "13_activities.h"
#include "14_signals.h"
#include "15_fgandbg.h"
#include "16_neonate.h"
#include "17_man.h"


#define PATH_MAX 4096
#define GREEN "\x1b[32m"
#define BLUE "\x1b[34m"
#define RED "\x1b[31m"
#define RESET "\x1b[0m"
#define YELLOW "\x1b[33m"

#endif
