#ifndef ADVSHELLPROTOCOL_H
#define ADVSHELLPROTOCOL_H

#define MAXPIPELEN 128
#define MAXMSGSIZE 256

// struct clientmsg
// used to define a fixed way to communicate between processes.
// msg can be used for writing and reading.
typedef struct clientmsg {
    char pipe[MAXPIPELEN];
    char msg[MAXMSGSIZE];
} clientmsg_t;

#endif
