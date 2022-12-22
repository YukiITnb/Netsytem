// #include <stdio.h>      /* for printf() and fprintf() */
// #include <sys/socket.h> /* for socket(), connect(), sendto(), and recvfrom() */
// #include <arpa/inet.h>  /* for sockaddr_in and inet_addr() */
// #include <stdlib.h>     /* for atoi() and exit() */
// #include <string.h>     /* for memset() */
// #include <unistd.h>     /* for close() */

// #define MAXRECVSTRING 255  /* Longest string to receive */

// void DieWithError(char *errorMessage);  /* External error handling function */

// int main(int argc, char *argv[])
// {
//     int sock;                         /* Socket */
//     struct sockaddr_in broadcastAddr; /* Broadcast Address */
//     unsigned short broadcastPort;     /* Port */
//     char recvString[MAXRECVSTRING+1]; /* Buffer for received string */
//     int recvStringLen;                /* Length of received string */

//     if (argc != 2)    /* Test for correct number of arguments */
//     {
//         fprintf(stderr,"Usage: %s <Broadcast Port>\n", argv[0]);
//         exit(1);
//     }

//     broadcastPort = atoi(argv[1]);   /* First arg: broadcast port */

//     /* Create a best-effort datagram socket using UDP */
//     sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);

//     /* Construct bind structure */
//     memset(&broadcastAddr, 0, sizeof(broadcastAddr));   /* Zero out structure */
//     broadcastAddr.sin_family = AF_INET;                 /* Internet address family */
//     broadcastAddr.sin_addr.s_addr = htonl(INADDR_ANY);  /* Any incoming interface */
//     broadcastAddr.sin_port = htons(broadcastPort);      /* Broadcast port */

//     /* Bind to the broadcast port */
//     bind(sock, (struct sockaddr *) &broadcastAddr, sizeof(broadcastAddr));
//     /* Receive a single datagram from the server */
//     recvStringLen = recvfrom(sock, recvString, MAXRECVSTRING, 0, NULL, 0);

//     recvString[recvStringLen] = '\0';
//     printf("Received: %s\n", recvString);    /* Print the received string */
    
//     close(sock);
//     exit(0);
// }

 /* udp-broadcast-client.c
  * udp datagram client
  * Get datagram stock market quotes from UDP broadcast:
  * see below the step by step explanation
  */
  #include <stdio.h>
  #include <unistd.h>
  #include <stdlib.h>
  #include <errno.h>
  #include <string.h>
  #include <time.h>
  #include <signal.h>
  #include <sys/types.h>
  #include <sys/socket.h>
  #include <netinet/in.h>
  #include <arpa/inet.h>

  #ifndef TRUE
  #define TRUE 1
  #define FALSE 0
  #endif

  extern int mkaddr(
                    void *addr,
                    int *addrlen,
                    char *str_addr,
                    char *protocol);

/*
 * This function reports the error and
 * exits back to the shell:
 */
 static void
 displayError(const char *on_what) {
     fputs(strerror(errno),stderr);
     fputs(": ",stderr);
     fputs(on_what,stderr);
     fputc('\n',stderr);
     exit(1);
}

 int
 main(int argc,char **argv) {
     int z;
     int x;
     struct sockaddr_in adr;  /* AF_INET */
     int len_inet;            /* length */
     int s;                   /* Socket */
     char dgram[512];         /* Recv buffer */
     static int so_reuseaddr = TRUE;
     static char
     *bc_addr = "127.255.255.2:9097";

    /*
     * Use a server address from the command
     * line, if one has been provided.
     * Otherwise, this program will default
     * to using the arbitrary address
     * 127.0.0.:
     */
     if ( argc > 1 )
     /* Broadcast address: */
        bc_addr = argv[1];

    /*
     * Create a UDP socket to use:
     */
     s = socket(AF_INET,SOCK_DGRAM,0);
     if ( s == -1 )
        displayError("socket()");

    /*
     * Form the broadcast address:
     */
     len_inet = sizeof adr;

     z = mkaddr(&adr,
                &len_inet,
                bc_addr,
                "udp");

     if ( z == -1 )
        displayError("Bad broadcast address");

    /*
     * Allow multiple listeners on the
     * broadcast address:
     */
     z = setsockopt(s,
                    SOL_SOCKET,
                    SO_REUSEADDR,
                    &so_reuseaddr,
                    sizeof so_reuseaddr);

     if ( z == -1 )
        displayError("setsockopt(SO_REUSEADDR)");

    /*
     * Bind our socket to the broadcast address:
     */
     z = bind(s,
             (struct sockaddr *)&adr,
             len_inet);

     if ( z == -1 )
        displayError("bind(2)");

     for (;;) {
        /*
         * Wait for a broadcast message:
         */
         z = recvfrom(s,      /* Socket */
                      dgram,  /* Receiving buffer */
                      sizeof dgram,/* Max rcv buf size */
                      0,      /* Flags: no options */
                      (struct sockaddr *)&adr, /* Addr */
                      &x);    /* Addr len, in & out */

         if ( z < 0 )
            displayError("recvfrom(2)"); /* else err */

         fwrite(dgram,z,1,stdout);
         putchar('\n');

         fflush(stdout);
     }

     return 0;
 }