<<<<<<< HEAD
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <stdio.h>


int
Accept(int fd, struct sockaddr *sa, socklen_t *salenptr)
{
    int  n;

    if ( (n = accept(fd, sa, salenptr)) < 0) {
        perror("accept error");
        exit(1);
    }
    return(n);
}

void
Bind(int fd, const struct sockaddr *sa, socklen_t salen)
{
    if (bind(fd, sa, salen) < 0){
        perror("bind error");
        exit(1);
    }
}

int
Connect(int fd, const struct sockaddr *sa, socklen_t salen)
{
    int result;
    if ((result = connect(fd, sa, salen)) < 0) {
        perror("connect error");
        exit (1);
    }
    return(result);
}

void
Listen(int fd, int backlog)
{
    if (listen(fd, backlog) < 0) {
        perror("listen error");
        exit(1);
    }
}

int
Select(int nfds, fd_set *readfds, fd_set *writefds, fd_set *exceptfds,
       struct timeval *timeout)
{
    int n;

    if ( (n = select(nfds, readfds, writefds, exceptfds, timeout)) < 0) {
        perror("select error");
        exit(1);
    }
    return(n);              /* can return 0 on timeout */
}

int
Socket(int family, int type, int protocol)
{
    int n;

    if ( (n = socket(family, type, protocol)) < 0) {
        perror("socket error");
        exit(1);
    }
    return(n);
}

void
Close(int fd)
{
    if (close(fd) == -1) {
        perror("close error");
        exit(1);
    }
}

int Write(int filedes, char *buf, size_t nbytes) {
    int amount_written;
    if ((amount_written = write(filedes, buf, nbytes)) != nbytes) {
        perror("write");
    }
   return amount_written;
}


int Read(int filedes, char *buf, size_t nbytes) {
    int amount_read; 
    if ((amount_read = read(filedes, buf, nbytes)) != nbytes) {
        perror("read");
    }
    return amount_read;
}

=======
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <stdio.h>


int
Accept(int fd, struct sockaddr *sa, socklen_t *salenptr)
{
    int  n;

    if ( (n = accept(fd, sa, salenptr)) < 0) {
        perror("accept error");
        exit(1);
    }
    return(n);
}

void
Bind(int fd, const struct sockaddr *sa, socklen_t salen)
{
    if (bind(fd, sa, salen) < 0){
        perror("bind error");
        exit(1);
    }
}

int
Connect(int fd, const struct sockaddr *sa, socklen_t salen)
{
    int result;
    if ((result = connect(fd, sa, salen)) < 0) {
        perror("connect error");
        exit (1);
    }
    return(result);
}

void
Listen(int fd, int backlog)
{
    if (listen(fd, backlog) < 0) {
        perror("listen error");
        exit(1);
    }
}

int
Select(int nfds, fd_set *readfds, fd_set *writefds, fd_set *exceptfds,
       struct timeval *timeout)
{
    int n;

    if ( (n = select(nfds, readfds, writefds, exceptfds, timeout)) < 0) {
        perror("select error");
        exit(1);
    }
    return(n);              /* can return 0 on timeout */
}

int
Socket(int family, int type, int protocol)
{
    int n;

    if ( (n = socket(family, type, protocol)) < 0) {
        perror("socket error");
        exit(1);
    }
    return(n);
}

void
Close(int fd)
{
    if (close(fd) == -1) {
        perror("close error");
        exit(1);
    }
}

int Write(int filedes, char *buf, size_t nbytes) {
    int amount_written;
    if ((amount_written = write(filedes, buf, nbytes)) != nbytes) {
        perror("write");
    }
   return amount_written;
}


int Read(int filedes, char *buf, size_t nbytes) {
    int amount_read; 
    if ((amount_read = read(filedes, buf, nbytes)) != nbytes) {
        perror("read");
    }
    return amount_read;
}

>>>>>>> 065814e8b6dfd54a9fe3d6ee12331e28e75be793
