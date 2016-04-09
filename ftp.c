#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

#include <arpa/inet.h>
#include <errno.h>

#include <sys/stat.h>
#include <fcntl.h>
#include <sys/sendfile.h>
#include <time.h>
#include <sys/timeb.h>
#include <sys/time.h>

#define FILENAME  "receive.txt"
#define BUFFER_SIZE 1024


void die(char *s);
void tcp_server_anytype(int argc, char *argv[]);
void tcp_client_anytype(int argc, char *argv[]);
void udp_server_message(int argc, char *argv[]);
void udp_client_message(int argc, char *argv[]);
void udp_server_anytype(int argc, char* argv[]);
void udp_client_anytype(int argc, char* argv[]);

void error(const char *msg)
{
    perror(msg);
    exit(1);
}

int main(int argc, char *argv[]) {

    int protocol, type, file;

    if (strcmp(argv[1] , "tcp") == 0) {
        protocol = 0;
    }
    else if (strcmp(argv[1] , "udp") == 0) {
        protocol = 1;
    }
    else {
        printf("wrong protocol\n");
    }


    if (strcmp(argv[2] , "send") == 0) {
        type = 0;
    }
    else if (strcmp(argv[2] , "recv") == 0) {
        type = 1;
    }
    else {
        printf("wrong type\n");
    }


    if (strcmp(argv[6] , "txt") == 0) {
        file = 0;
    }
    else if (strcmp(argv[6] , "img") == 0) {
        file = 1;
    }
    else if (strcmp(argv[6] , "audio") == 0) {
        file = 2;
    }
    else if (strcmp(argv[6] , "video") == 0) {
        file = 3;
    }
    else if (strcmp(argv[6] , "message") == 0) {
        file = 4;
    }
    else {
        printf("wrong file type\n");
    }





    printf("you input %s,%s,%s,%s\n", argv[1], argv[2], argv[3], argv[4]);

    switch (protocol) {
    case 0:
        printf("protocol: tcp\n");
        switch (type) {
        case 0:
            printf("type: send\n");
            switch (file) {
            case 0:
                printf("type: txt\n");
                tcp_server_anytype(argc, argv);
                break;
            case 1:
                printf("type: img\n");
                tcp_server_anytype(argc, argv);
                break;
            case 2:
                printf("type: audio\n");
                tcp_server_anytype(argc, argv);
                break;
            case 3:
                printf("type: video\n");
                tcp_server_anytype(argc, argv);
                break;
            }
            break;

        case 1:
            printf("type: recv\n");
            switch (file) {
            case 0:
                printf("type: txt\n");
                tcp_client_anytype(argc, argv);
                break;
            case 1:
                printf("type: img\n");
                tcp_client_anytype(argc, argv);
                break;
            case 2:
                printf("type: audio\n");
                tcp_client_anytype(argc, argv);
                break;
            case 3:
                printf("type: video\n");
                tcp_client_anytype(argc, argv);
                break;
            }
            break;
        }
        break;


    case 1:
        printf("protocol: udp\n");
        switch (type) {
        case 0:
            printf("type: send\n");
            switch (file) {
            case 0:
                printf("type: txt\n");
                udp_server_anytype(argc, argv);
                break;
            case 1:
                printf("type: img\n");
                udp_server_anytype(argc, argv);
                break;
            case 2:
                printf("type: audio\n");
                udp_server_anytype(argc, argv);
                break;
            case 3:
                printf("type: video\n");
                udp_server_anytype(argc, argv);
                break;
            case 4:
                printf("type: message\n");
                udp_server_message(argc, argv);
                break;
            }
            break;

        case 1:
            printf("type: recv\n");
            switch (file) {
            case 0:
                printf("type: txt\n");
                udp_client_anytype(argc, argv);
                break;
            case 1:
                printf("type: img\n");
                udp_client_anytype(argc, argv);
                break;
            case 2:
                printf("type: audio\n");
                udp_client_anytype(argc, argv);
                break;
            case 3:
                printf("type: video\n");
                udp_client_anytype(argc, argv);
                break;
            case 4:
                printf("type: message\n");
                udp_client_message(argc, argv);
                break;
            }
            break;
        }

        break;

    default:
        printf("wrong");
        break;

    }

    return 0;

}



/************************************************tcp send img&audio&video*********************************************************************
***************************************************************************************************************************
***************************************************************************************************************************
**************************************************************************************************************************/




void tcp_server_anytype(int argc, char *argv[]) {
    printf("void tcp_server\n");
    int sockfd, newsockfd, portno;
    socklen_t clilen;
    char buffer[BUFSIZ];
    struct sockaddr_in serv_addr, cli_addr;
    int n;
    int fd;
    int i;
    struct stat file_stat;
    int size;
    char filename[1024];
    ssize_t len;
    off_t offset = 0;
    int remain_data;
    int sent_bytes = 0;
    if (argc < 2) {
        fprintf(stderr, "ERROR, no port provided\n");
        exit(1);
    }

//TCP socket
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        fprintf(stderr, "Error creating socket --> %s", strerror(errno));//add error message
        exit(EXIT_FAILURE);
    }

// zero out the structure
    memset(&serv_addr, 0, sizeof(serv_addr));
    portno = atoi(argv[4]);
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(portno);

// sockopt   prevent address already in use
    int on = 1;
    if ((setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on))) < 0)
    {
        perror("setsockopt failed");
        exit(EXIT_FAILURE);
    }




//binding
    if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
        die("bind()");
        fprintf(stderr, "Error on bind --> %s", strerror(errno)); //add error message
        exit(EXIT_FAILURE);
    }

// listening
    // listen(sockfd, 5);
    if ((listen(sockfd, 5)) == -1) {
        fprintf(stderr, "Error on listen --> %s", strerror(errno));

        exit(EXIT_FAILURE);
    }


    clilen = sizeof(cli_addr);



// connect
    newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
    if (newsockfd < 0) {
        fprintf(stderr, "Error on accept --> %s", strerror(errno));

        exit(EXIT_FAILURE);
    }

    fprintf(stdout, "Accept peer --> %s\n", inet_ntoa(cli_addr.sin_addr));

    int numbytes;
    FILE *fp;
    strcpy(filename, argv[5]);
    if ( lstat(filename, &file_stat) < 0) {
        exit(1);
    }
    printf("The file size is %lu\n", file_stat.st_size);

    fp = fopen(filename, "rb");
    fseek(fp, 0, SEEK_END);
    size = ftell(fp);
    fseek(fp, 0, SEEK_SET);
//Send audio Size
    printf("Sending audio Size %d\n", size);
    write(newsockfd, &size, sizeof(size));

    //Sending file
    while (!feof(fp)) {
        numbytes = fread(buffer, sizeof(char), sizeof(buffer), fp);
        printf("%d fread %d bytes, ", i, numbytes);
        numbytes = write(newsockfd, buffer, numbytes);
        printf("Sending %d bytes\n", numbytes);
        i++;
    }


    close(newsockfd);
    close(sockfd);

}




void tcp_client_anytype(int argc, char *argv[]) {
    printf("void tcp_client\n");
    int sockfd, portno, n;
    struct sockaddr_in serv_addr;
    struct hostent *server;

    char buffer[BUFSIZ];
    struct stat file_stat;
    int size;
    float count;
    float throughput;
    float get_persent;
    int persent;
    int log_persent = 5;
    int i = 0;
    char filename[1024];
    clock_t t1, t2, t3, t4, t5, t6;
    FILE *received_file;
    int remain_data = 0;
    ssize_t len;

    if (argc < 3) {
        fprintf(stderr, "usage %s hostname port\n", argv[0]);
        exit(0);
    }
    portno = atoi(argv[4]);

// TCP socket
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
        error("ERROR opening socket");


    server = gethostbyname(argv[3]);
    if (server == NULL) {
        fprintf(stderr, "ERROR, no such host\n");
        exit(0);
    }
    bzero((char *) &serv_addr, sizeof(serv_addr));

// intial connect to port
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr,
          (char *)&serv_addr.sin_addr.s_addr,
          server->h_length);
    serv_addr.sin_port = htons(portno);


// connect to  sever
    if (connect(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
        error("ERROR connecting");

    FILE *fp;
    int numbytes;
    strcpy(filename, argv[5]);
    //Open file
    if ( (fp = fopen(filename, "wb")) == NULL) {
        perror("fopen");
        exit(1);
    }

    //Receive file from server
    read(sockfd, &size, sizeof(int));
    printf("file size = %d \n", size);
    t1 = clock();
    while (1) {
        numbytes = read(sockfd, buffer, sizeof(buffer));
        if (numbytes == 0) {
            t5 = clock();
            break;
        }
        // printf("%d read %d bytes, ", i, numbytes);//debug mode
        numbytes = fwrite(buffer, sizeof(char), numbytes, fp);
        // printf("fwrite %d bytes\n", numbytes);//debug mode
        count += numbytes;
        get_persent = count / size;
        persent = get_persent * 100;
        if (persent >= log_persent && persent < log_persent + 1) {
            t6 = clock();
            printf("    pass %lf s get %f persent  %d \n", (t6 - t1) / (double)(CLOCKS_PER_SEC), get_persent, persent);
            log_persent += 5;
        }

        i++;
    }
    printf("finish receive audio file!\n");

    throughput = count / ((t5 - t1) / (double)(CLOCKS_PER_SEC));
    printf("throughput = %f bytes/s  \n", throughput);

    fclose(fp);
    close(sockfd);


}




/************************************************udp_message*********************************************************************
*************************************************************************************************************************
**************************************************************************************************************************
**************************************************************************************************************************
**************************************************************************************************************************
**************************************************************************************************************************
**************************************************************************************************************************
**************************************************************************************************************************
**************************************************************************************************************************
**************************************************************************************************************************
***************************************************************************************************************************/


void udp_server_message(int argc, char *argv[]) {


    struct sockaddr_in serv_addr, cli_addr;

    int sockfd, i, slen = sizeof(cli_addr) , recv_len, portno;
    char buf[BUFSIZ];

    //create a UDP socket
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1) {
        die("socket");
    }

    // zero out the structure
    memset((char *) &serv_addr, 0, sizeof(serv_addr));
    portno = atoi(argv[4]);
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(portno);
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    //bind socket to port
    if ( bind(sockfd , (struct sockaddr*)&serv_addr, sizeof(serv_addr) ) == -1) {
        die("bind");
    }

    //keep listening for data
    while (1) {
        printf("Waiting for data...");
        fflush(stdout);

        //try to receive some data, this is a blocking call
        if ((recv_len = recvfrom(sockfd, buf, BUFSIZ, 0, (struct sockaddr *) &cli_addr, &slen)) == -1) {
            die("recvfrom()");
        }

        //print details of the client/peer and the data received
        printf("Received packet from %s:%d\n", inet_ntoa(cli_addr.sin_addr), ntohs(cli_addr.sin_port));
        printf("Data: %s\n" , buf);

        //now reply the client with the same data
        if (sendto(sockfd, buf, recv_len, 0, (struct sockaddr*) &cli_addr, slen) == -1) {
            die("sendto()");
        }
    }

    close(sockfd);

}


void udp_client_message(int argc, char *argv[]) {

    printf("void udp_client\n");
    struct sockaddr_in cli_addr;
    int sockfd, i, slen = sizeof(cli_addr), portno;
    char buf[BUFSIZ];
    char message[BUFSIZ];
    struct hostent *server;


    if ( (sockfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1) {
        die("socket");
    }

    portno = atoi(argv[4]);
    memset((char *) &cli_addr, 0, sizeof(cli_addr));
    cli_addr.sin_family = AF_INET;
    cli_addr.sin_port = htons(portno);

    server = gethostbyname(argv[3]);
    bcopy((char *)server->h_addr,
          (char *)&cli_addr.sin_addr.s_addr,
          server->h_length);


    while (1) {
        printf("Enter message : ");
        gets(message);

        //send the message
        if (sendto(sockfd, message, strlen(message) , 0 , (struct sockaddr *) &cli_addr, slen) == -1) {
            die("sendto()");
        }

        //receive a reply and print it
        //clear the buffer by filling null, it might have previously received data
        memset(buf, '\0', BUFSIZ);
        //try to receive some data, this is a blocking call
        if (recvfrom(sockfd, buf, BUFSIZ, 0, (struct sockaddr *) &cli_addr, &slen) == -1) {
            die("recvfrom()");
        }

        puts(buf);
    }

    close(sockfd);
}



void die(char *s)
{
    perror(s);
    exit(1);
}



/***********************************************udp_anyfile**************************************************************************
**************************************************************************************************************************
**************************************************************************************************************************
**************************************************************************************************************************
**************************************************************************************************************************
**************************************************************************************************************************
***************************************************************************************************************************/




void udp_server_anytype(int argc, char *argv[]) {
    printf("void udp_server_anytype\n");

    FILE *input;
    FILE *checkfile;
    int n;
    int count = 0;

    struct sockaddr_in remote, local;
    int len_local = sizeof(local), portno;
    int s;
    int n_num = 0;
    struct hostent *server;

    struct package {
        unsigned int num;
        unsigned int size;
        unsigned char buffer[BUFFER_SIZE];
    };
    struct stat file_stat;
    char filename[1024];

    unsigned char buffer[BUFFER_SIZE];
    char check[100];
    int nbrecv = 0, i;
    int lost_num;
    int size;
    int total_pack;
    printf("Press ENTER to send file!\n");
    fgets(check, 100, stdin);

    strcpy(filename, argv[5]);
    if ( lstat(filename, &file_stat) < 0) {
        exit(1);
    }
    printf("The file size is %lu\n", file_stat.st_size);


    input = fopen(filename, "r");
    fseek(input, 0, SEEK_END);
    size = ftell(input);
    fseek(input, 0, SEEK_SET);
    total_pack = size / 1024.0;
    if (size % 1024 != 0) {
        total_pack++;
    }
    struct package *pack;
    int *lost_pack_num;
    pack = (struct package*)malloc(total_pack * sizeof(struct package));
    lost_pack_num = (int *)malloc(total_pack * sizeof(int));

    if (input) {

        if ((s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1) {
            fprintf(stderr, "Error getting a socket! \n");
            exit(-1);
        }

        memset((char *) &remote, 0, sizeof(remote));
        portno = atoi(argv[4]);
        remote.sin_family = AF_INET;
        remote.sin_port = htons(portno);
        server = gethostbyname(argv[3]);
        bcopy((char *)server->h_addr,
              (char *)&remote.sin_addr.s_addr,
              server->h_length);

        if (sendto(s, &size, sizeof(size), 0, &remote, sizeof(remote)) == -1) {
            fprintf(stderr, "error while sending data!\n");
            exit(-1);
        }
        printf("file size = %d\n", size);

        while (!feof(input)) {
            n = fread(pack[n_num].buffer, 1, BUFFER_SIZE, input);
            pack[n_num].num = n_num;
            // printf("f n_num = %d num = %d ", n_num, pack[n_num].num); //debug mode
            pack[n_num].size = n;
            // printf("n = %d size = %d \n\n", n, pack[n_num].size); //debug mode

            if (sendto(s, (struct package*)&pack[n_num], sizeof(pack[n_num]), 0, &remote, sizeof(remote)) == -1) {
                fprintf(stderr, "error while sending data!\n");
                exit(-1);
            }

            // printf("n_num = %d num = %d size = %d ", pack[n_num].num, n_num, pack[n_num].size);//debug mode
            // printf("n = %d\n", n);//debug mode

            n_num++;
            count += n;

        }
        printf("%d bytes sent. \n", count);

    }
    else {
        printf("error while opening input file!");
    }
    printf("File first sent over!!\n\n");
    int send_times = 2;

    while (1) {
        printf("Resent the %d times \n", send_times);

        if ((nbrecv =  recvfrom(s, &lost_num, sizeof(lost_num), 0, (struct sockaddr *) &remote, &len_local)) == -1) {
            fprintf(stderr, "fail while receiving data! \n");
            exit(-1);
        }
        printf("still lost %d pack\n", lost_num);
        if (lost_num == 0) {
            printf("send file finish!\n ");
            break;
        }
        if ((nbrecv =  recvfrom(s, lost_pack_num, 4 * lost_num, 0, (struct sockaddr *) &remote, &len_local)) == -1) {
            fprintf(stderr, "fail while receiving data! \n");
            exit(-1);
        }
        // printf("size of lost pack = %d\n", sizeof(lost_pack_num));//debug mode
        // for (i = 0; i < lost_num; i++) {
        //     printf("get %d lost_pack_num %d\n", i, lost_pack_num[i]);//debug mode
        // }

        printf("File %d start sent!!\n", send_times);

        for (i = 0; i < lost_num; i++) {
            if (sendto(s, (struct package*)&pack[lost_pack_num[i]], sizeof(pack[lost_pack_num[i]]), 0, &remote, sizeof(remote)) == -1) {
                die("sendto()");
            }
            // printf("total send %d: lost_num %d: send lost package_number %d size = %d \n", i, lost_pack_num[i] , pack[lost_pack_num[i]].num, pack[lost_pack_num[i]].size );//debug mode

        }
        printf("resent %d times  over\n\n", send_times);
        send_times++;
    }
    free(pack);
    free(lost_pack_num);

    close(s);
    fclose(input);

}



void udp_client_anytype(int argc, char *argv[]) {
    printf("void udp_client_anytype\n");
    int n;
    FILE *output;

    struct sockaddr_in local, remote;
    int portno;
    int len_remote = sizeof(remote);
    int s;
    float count = 0;// get file bytes
    int n_num = 0;
    int testnum = 0; //receive times
    float get_persent = 0;// get persent in float
    int persent;// get persent 100%
    int log_persent = 5;
    float total_lost_pack_number;
    int total_get_pack_number;
    float lost_pack_rate;
    float throughput;
    struct package {
        unsigned int num;
        unsigned int size;
        unsigned char buffer[BUFFER_SIZE];
    };
    struct stat file_stat;
    char filename[1024];
    struct hostent *server;


    strcpy(filename, argv[5]);
    char buffer[BUFFER_SIZE];
    struct package temp;

    output = fopen(filename, "w");

    if ((s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1) {
        die("socket()");
        fprintf(stderr, "Error getting a socket!\n");
        exit(-1);
    }
    memset((char *) &remote, 0, sizeof(remote));
    memset((char *) &local, 0, sizeof(remote));

    server = gethostbyname(argv[3]);
    if (server == NULL) {
        die("server()");
        fprintf(stderr, "ERROR, no such host\n");
        exit(0);
    }
    portno = atoi(argv[4]);
    local.sin_family = AF_INET;
    local.sin_port = htons(portno);
    // local.sin_addr.s_addr = htonl(INADDR_ANY);
    bcopy((char *)server->h_addr,
          (char *)&local.sin_addr.s_addr,
          server->h_length);

    if (bind(s, &local, sizeof(local)) == -1) {
        die("bind()");
        fprintf(stderr, "Cannot connect to service!\n");
        exit(-1);
    }
    int flag = 1;
    int nbrecv = 0;
    int lost_num = 1;//default
    int lost_num_temp = 0;
    int pnum = 0, i, j = 0, k = 0, t = 0, lost, total_pack;
    int size;
    clock_t t1, t2, t3, t4, t5, t6;
    struct timeval tv;
    tv.tv_sec = 0;
    tv.tv_usec = 100000;
    t1 = clock();
    if ((nbrecv =  recvfrom(s, &size, sizeof(size), 0, (struct sockaddr *) &remote, &len_remote)) == -1) {
        fprintf(stderr, "fail while receiving data! \n");
        exit(-1);
    }


    total_pack = size / 1024.0;
    if (size % 1024 != 0) {
        total_pack++;
    }
    struct package *pack;
    int *lost_pack_num;
    int *lost_pack_num_temp;
    pack = (struct package*)malloc(total_pack * sizeof(struct package));
    lost_pack_num = (int *)malloc(total_pack * sizeof(int));
    lost_pack_num_temp = (int *)malloc(total_pack * sizeof(int));
    lost_num = total_pack;
    printf("total size = %d bytes, total_pack = %d \n\n", size, total_pack);
    for (i = 0; i < total_pack; i++) {
        lost_pack_num[i] = i;
    }

    while (1) {
        printf("receive %d times\n", testnum);
        printf("%f bytes get. \n", count);
        printf("still lost %d packages \n\n", lost_num);
        total_get_pack_number += lost_num;

        if (testnum != 0) {
            if (sendto(s, &lost_num, sizeof(lost_num), 0, (struct sockaddr*) &remote, len_remote) == -1) {
                fprintf(stderr, "error while sending data!\n");
                exit(-1);
            }
            // timeout

            if (sendto(s, lost_pack_num, 4 * lost_num, 0, (struct sockaddr*) &remote, len_remote) == -1) {
                fprintf(stderr, "error while sending data!\n");
                exit(-1);
            }
            // printf("size of lost pack = %d\n", sizeof(lost_pack_num));//debug mode

            // timeout
            // for (i = 0; i < lost_num; i++) {
            //     printf("%d send lost_pack_num %d\n", i, lost_pack_num[i]);
            // }//debug mode
            // printf("send lost data over \n");//debug mode
        }
        if (lost_num == 0) {
            t5 = clock();
            break;
        }

        lost_num_temp = lost_num;
        for (i = 0; i < lost_num_temp; i++) {
            memset((struct package*)&temp, '\0', sizeof(temp));

            if (setsockopt(s, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv)) < 0) {
                die("setsockopt()");
            }

            if ((nbrecv = recvfrom(s, (struct package*)&temp, sizeof(temp), 0, &remote, &len_remote)) == -1) {
                printf("timeout\n");
                j = i;
                while (lost_pack_num[j] != lost_pack_num[lost_num_temp ]) {
                    lost_pack_num_temp[k] = lost_pack_num[j];
                    // printf("    total lost %d pack:  still lost package num %d\n", k, lost_pack_num[j]);//debug mode
                    j++;
                    k++;
                    total_lost_pack_number++;
                }
                i = j;
                // printf("count lost tail over\n");//debug mode

                goto losttail;
                die("recvfrom()");
            }
            pack[temp.num].num = temp.num;
            pack[temp.num].size = temp.size;
            for (t = 0; t < BUFFER_SIZE; t++) {
                pack[temp.num].buffer[t] = temp.buffer[t];
            }
            // printf("    get temp num %d\n", temp.num);//debug mode
            // printf("total get %d pack: lost_num %d: get lost pack_num %d size = %d \n", i , lost_pack_num[i], pack[lost_pack_num[i]].num, pack[lost_pack_num[i]].size );//debug mode

            if (lost_pack_num[i] != pack[lost_pack_num[i]].num ) {
                j = i;
                while (lost_pack_num[j] != pack[temp.num].num) {
                    lost_pack_num_temp[k] = lost_pack_num[j];
                    // printf("    total lost %d pack:  still lost package num %d\n", k, lost_pack_num[j]);//debug mode
                    j++;
                    k++;
                    total_lost_pack_number++;
                }
                i = j;
                // printf("count lost over\n");//debug mode
            }

            count += pack[lost_pack_num[i]].size;
            get_persent = count / size;
            persent = get_persent * 100;
            // printf("pass %lf s get %d persent\n", (t6 - t1) / (double)(CLOCKS_PER_SEC), persent);
            if (persent >= log_persent && persent < log_persent + 1) {
                t6 = clock();
                printf("    pass %lf s get %f persent  %d \n", (t6 - t1) / (double)(CLOCKS_PER_SEC), get_persent, persent);
                log_persent += 5;
            }

        }

losttail:
        for (i = 0; i < lost_num; i++) {
            lost_pack_num[i] = 0;
        }
        for (i = 0; i < k ; i++) {
            lost_pack_num[i] = lost_pack_num_temp[i];
        }
        lost_num = k ;
        k = 0;
        testnum++;
    }
    printf("while over\n");
    printf("total receive %d times\n", testnum);
    printf("%f bytes get. \n", count);
    printf("Got the file!\n\n");

    printf("total_get_pack_amounts = %d \n", total_get_pack_number);
    printf("total_lost_pack_amounts = %f \n", total_lost_pack_number);
    lost_pack_rate = total_lost_pack_number / total_get_pack_number;
    printf("lost_pack_rate = %f\n\n", lost_pack_rate);
    throughput = count / ((t5 - t1) / (double)(CLOCKS_PER_SEC));
    printf("throughput = %f bytes/s  \n", throughput);
    ssize_t len;
    for (i = 0; i < total_pack; i++) {
        // printf("%d write  pack num = %d pack size = %d\n", i, pack[i].num, pack[i].size);//debug mode
        len = sizeof(pack[i].buffer);
        fwrite(pack[i].buffer, 1 , pack[i].size , output);
    }
    printf("write finish. total write %f bytes\n", count);
    free(pack);
    free(lost_pack_num);
    free(lost_pack_num_temp);

    close(s);
    fclose(output);


}