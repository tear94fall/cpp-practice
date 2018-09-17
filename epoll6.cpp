#include <stdio.h>
#include <string.h>
#include <netdb.h>
#include <unistd.h>
#include <sys/epoll.h>
#include <errno.h>
#include <sys/un.h>
#include <sys/socket.h>
#include <fcntl.h>

#include <cassert>
#include <thread>

#include "net_utils.hpp"
#include "ring_buffer.hpp"

#define