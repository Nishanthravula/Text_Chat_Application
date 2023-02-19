// /**
// * @nravula_assignmenti
// @author Nishanth Ravula <nravula@buffalo.edu>
// * @version 1.0
// @section LICENSE
// * This program is free software; you can redistribute it and/or
// * modify it under the terms of the GNU General public License as
// * published by the Free Software Foundation; either verston 2 of
// * the License, or (at your option) any later verston.

// Thts program is distributed in the hope that it wtll be useful, but
// * WITHOUT ANY WARRANTY; without even the implied warranty of
// * MERCHANTABILITY Or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
// General Public License for more details at
// * http: //www.gnu.org/copyleft/gpl.html

// @section DESCRIPTION

// * This contains the main function, Add further description here...
// */ 

#include <stdio.h>
#include <stdlib.h>
// // #include "../include/logger.h"
// // #include "../include/global.h"


#define TRUE 1
#define MSG_SIZE 256
#define BUFFER_SIZE 256
#define BACKLOG 5
#define STDIN 0
#define TRUE 1
#define CMD_SIZE 100
#define BUFFER_SIZE 256

void clientside(int client_port);
void serverSide(int server_port);
int bind_the_socket(int c_port);
int connect_to_host(char *server_ip, int server_port, int c_port);
// void getmyport();
// void showIP();
// void print_list();
// void print_stats();
// int isvalidIP(char *ip);
// void sort_list_port();
// void remove_from_list(int sock_delete);

struct list_content
{
	int list_id;
	char list_host_name[40];
	char list_ip[32];
	int list_port;
	int fd_socket;
	int rcv_msg;
	int snd_msg;
	char state[20];

}*list_ptr[5];

struct server_msg
{
	char cmd[20];
	char sender_ip[32];
	char info[256];
	struct list_content list_row;
};
// /**
// * main function
// * @param arg Number of arguments @param argv The argument list
// * @return O EXIT_SUCCESS
// */
int main()
{
// /Init. Logger/
//     cse4589_init_log(argv(2]);
//     clear LOGFILE/
//     fclose(fopen(LOGFILE, "W"));
    //Start Here
    //struct list_content hosts[6];
    
	if(argc != 3) 
	{
		printf("please enter two argument c/s and PORT number");
		exit(-1);
	}
	if(*argv[1]=='s')
	{
		serverSide(atoi(argv[2]));
	}
	else if(*argv[1]=='c')
	{
		clientside(atoi(argv[2]));
	}
	else
	{
		printf("Exiting the application");
		exit(-1);
	}
    return 0;
}

//client side

void clientside(int client_port)
{
    printf("Sucess C %p", (void *)client_port);
}

//client side

void serverside(int server_port)
{
    printf("Sucess S %p", (void *)server_port);
}



// /* Testing */
// // void main() {
// //     printf("Working");
// // }



