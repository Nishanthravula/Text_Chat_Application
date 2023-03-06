#include <stdio.h>
#include <stdlib.h>
#include "../include/logger.h"
#include "../include/global.h"

#include <sys/socket.h>
#include <strings.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <stdlib.h>
#include <sys/socket.h>
// #include <sys/un.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netdb.h>
//#include <ifaddrs.h>
#include <unistd.h>
//#include <errno.h>


#define MSG_SIZE 256
#define BACKLOG 5
#define STDIN 0
#define CMD_SIZE 100

int fdsocket,client_head_socket, client_sock_index;
fd_set client_master_list, client_watch_list;

struct client_msg
{
	char cmd[20];
	char ip[32];
	char info[256];
};
struct client_msg rcv_data;


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

}*lp[5];
struct list_content send_list;

/* Structure to represent messages from the server */
struct server_msg {
    char cmd[20]; /* Command associated with the message */
    char sender_ip[32]; /* IP address of the sender */
    char info[256]; /* Additional information associated with the message */
    struct list_content list_row; /* Struct to hold information about a row in a list */
};
struct server_msg server_data;

struct client_list
{
	int C_id;
	char C_ip[32];
	char ip[4][32];
	char buffer[1024];
}*cp[5];

/**
 * main function
 *
 * @param  argc Number of arguments
 * @param  argv The argument list
 * @return 0 EXIT_SUCCESS
 */
int main(int argc, char **argv)
{
	/*Init. Logger*/
	cse4589_init_log(argv[2]);

	/*Clear LOGFILE*/
	fclose(fopen(LOGFILE, "w"));

	/*Start Here*/
	
	struct list_content hosts[6];
	if(argc != 3) 
	{
		exit(-1);
	}
	if(*argv[1]=='c')
	{
		cSide(atoi(argv[2]));
		
	}
	else if(*argv[1]=='s')
	{
		s_Side(atoi(argv[2]));
	}
	else
	{
		//Exiting the application
		exit(-1);
	}
	return 0;
}
//client side
void cSide(int client_port);
void s_Side(int server_port);

void cSide(int client_port)
{
	int bind_port=bind_the_socket(client_port);
	int loggedin=0;//using as bool
	const char *outputs[7] = {"AUTHOR", "IP", "PORT","LIST","LOGIN","REFRESH","EXIT","LOGOUT","SUCCESS"};
	if(bind_port==0)
	{
		exit(-1);
	}

	printf("\ninside client side");
	int server=0;//SOCKET FOR SERVER COMMUNICATION
	int selret;
	int j=0;
	struct client_msg data;
	file_des();
    client_head_socket=0;
	for(;;)
	{
		fflush(stdout);	
		file_des();
		FD_SET(server, &client_master_list);
		client_head_socket=server;

		memcpy(&client_watch_list, &client_master_list, sizeof(client_master_list));
        /* select() system call. This will BLOCK */
        selret = select(client_head_socket + 1, &client_watch_list, NULL, NULL, NULL);
        if(selret < 0)
        {
            perror("select failed.");
            exit(-1);
        }

        // if(selret > 0)
		else
        {
            /* Loop through socket descriptors to check which ones are ready */
            for(client_sock_index=0; client_sock_index<=client_head_socket; client_sock_index+=1)
            {
                if(FD_ISSET(client_sock_index, &client_watch_list))
                {
                	 if (client_sock_index == STDIN)
                    {
				        char *msg = (char*) malloc(sizeof(char)*MSG_SIZE);//MSG_SIZE=256
				    	memset(msg, '\0', MSG_SIZE);
						if(fgets(msg, MSG_SIZE-1, stdin) == NULL) //Mind the newline character that will be written to msg
							exit(-1);
						/*to get rid of '\n' added by fgets*/
						int len=strlen(msg);
						msg[len-1]='\0';

						if((strcmp(msg,outputs[0]))==0)

						{

							author_output();
						}

						else if((strcmp(msg,outputs[1]))==0)

						{

							IP_Add();
							
                            cse4589_print_and_log("[%s:END]\n",outputs[1]);
						}

						
                        else if((strcmp(msg,outputs[2]))==0)
						
                        {
						
                        	myport();
						
                        	cse4589_print_and_log("[%s:END]\n",outputs[2]);
						
                        }
						
                        else if((strcmp(msg,outputs[3]))==0 &&
						
                        		loggedin==1)
						
                        {
						
                        	strcpy(data.cmd,outputs[3]);
						
                        	if(send(server, &data, sizeof(data), 0) == sizeof(data))
						
                        	{
						
                        		cse4589_print_and_log("[%s:SUCCESS]\n",outputs[3]);
						
                        	}
						
                        	else
						
                        	{
						
                        		cse4589_print_and_log("[%s:ERROR]\n",outputs[3]);
						
                        	}


						
                        	fflush(stdout);
						
                        }
						
                        else if((strncmp(msg,outputs[4],5))==0)
						
                        {
						
                        	char ip[32];
						
                        	char portv[32];
						
                        	int k=6;
						
                        	j=0;



							for (; msg[k] != ' '; k++)

							{

								ip[j] = msg[k];

								j++;

							}

							ip[j]='\0';

							//printf("\n your ip add is %s",ip);

							if((isvalidIP(ip))==1)

							{

								j=0;

								k=k+1;



								for (; msg[k] != '\0'; k++)

								{

									portv[j]=msg[k];

									j++;

								}

								portv[j]='\0';

								int length = strlen (portv);

								int p_error=0;

								int i=0;

								while(i<length)

							    {

							        if (!isdigit(portv[i]))

							        {

							            printf ("Entered input is not a number\n");

							            p_error=1;

							        }

									i++;

							    }

								if(p_error==1)

								{

									cse4589_print_and_log("[%s:ERROR]\n",outputs[4]);

									cse4589_print_and_log("[%s:END]\n",outputs[4]);

								}

								else

								{

                                    int l = 1;
                                    int u = 65535;
                                    int port_check = atoi(portv);

                                    if (!(l <= port_check && port_check <= u)) {
                                        const char *error_msg = "[%s:ERROR]\n";
                                        cse4589_print_and_log(error_msg, outputs[4]);
                                        cse4589_print_and_log("[%s:END]\n", outputs[4]);
                                        return;
                                    }

                                    /*connect to server*/
                                    server = connect_to_host(ip, atoi(portv), client_port);
                                    FD_SET(server, &client_master_list);
                                    client_head_socket = server;
                                    loggedin = 1;
                                    cse4589_print_and_log("[%s:SUCCESS]\n",outputs[4]);

								}

							}

							else

							{

								cse4589_print_and_log("[%s:ERROR]\n",outputs[4]);

								cse4589_print_and_log("[%s:END]\n",outputs[4]);

							}



							fflush(stdout);

						}

						else if((strcmp(msg,outputs[5]))==0&&

								loggedin==0)

				        {  

				        	cse4589_print_and_log("[%s:SUCCESS]\n",outputs[5]); 

				        	cse4589_print_and_log("[%s:END]\n",outputs[5]);                 

				        }
				        
						
						// else if((strcmp(msg,"LOGOUT"))==0&&

						// 		loggedin==1)

						// {

						// 	strcpy(data.cmd,"LOGOUT");

						// 	if(send(server, &data, sizeof(data), 0) == sizeof(data))

						// 	{

						// 		printf("[LOGOUT:SUCCESS]\n");

						// 		loggedin=0;

						// 		int bind_port=bind_the_socket(client_port);

						// 		server=close(server);

						// 	}

						// 	cse4589_print_and_log("[LOGOUT:END]\n");	

						// }
						

						else if((strcmp(msg,outputs[6]))==0)

						{

							close(server);

							cse4589_print_and_log("[%s:SUCCESS]\n",outputs[6]);


							cse4589_print_and_log("[%s:END]\n",outputs[6]);

							exit(0);

						}

                    }

                    else

                    {

        				struct server_msg srcv;

        				memset(&srcv, '\0', sizeof(srcv));



						if(recv(server, &srcv, sizeof(srcv), 0) >= 0)

						{



							if(strcmp(srcv.cmd,outputs[3])==0)

							{	

								cse4589_print_and_log("%-5d%-35s%-20s%-8d\n",srcv.list_row.list_id,srcv.list_row.list_host_name,srcv.list_row.list_ip,srcv.list_row.list_port);

							}
							else if(strcmp(srcv.cmd,"LISTOVER")==0)
							{	
								cse4589_print_and_log("[%s:END]\n",outputs[3]);
							}
							
							
							fflush(stdout);//there will be no waiting for sending message
						}
                    } 
                    fflush(stdout);	
                }
            }
        }	
	}
}

//server side
void s_Side(int server_port)
{
	const char *outputs[7] = {"AUTHOR", "IP", "PORT","LIST","LOGIN","REFRESH","EXIT"};
	printf("\n Inside server side with port %d\n", server_port);
	// int server_socket, head_socket, selret, sock_index, fdaccept=0, caddr_len, send_socket=0;
	int head_socket;
	int selret;
	int sock_index;
	int fdaccept = 0;
	int caddr_len;
	int send_socket = 0;
    int server_socket;
	// struct server_msg server_data;
	// struct list_content send_list;
	/*our defined client message*/
	// struct client_msg rcv_data;
    /* Socket address*/
	struct sockaddr_in server_addr, client_addr;

    /*File discriptor list*/
	fd_set master_list, watch_list;

	server_socket = socket(AF_INET, SOCK_STREAM, 0);
	fdsocket=server_socket;// bcoz fdsocket is used by myport function
    (fdsocket >= 0) ? printf("\nSocket created successfully\n") : perror("socket cannot be created");

	/* Fill up sockaddr_in struct */
	bzero(&server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(server_port);

    /* bind*/
	int k = bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr));
    if(k >= 0 )
	{
		printf("\nSocket Binded Successfully\n");
    }
    else
	{
		perror("Binding the socket failed");
    }
    /* Listen */
	int m = listen(server_socket, BACKLOG);
    if(m >= 0)
	{
    	printf("\nSocket listening\n");
    }
    else
	{
		perror("Listening on port is not possible");
    }
    /*Initializing list*/
    // for(int i=0;i<5;i++)
	int i =0;
	while(i<5)
    {
    	lp[i]=(struct list_content *)malloc(sizeof(struct list_content));
    	lp[i]->list_id=0; 	
		i++;
    }
	// for(int i=0;i<5;i++){
	int j=0;
	while(j<5){
		cp[j]=(struct list_content *)malloc(sizeof(struct client_list));
		cp[j]->C_id=0;
		strcpy(cp[j]->ip[0],"null");
		strcpy(cp[j]->ip[1],"null");
		strcpy(cp[j]->ip[2],"null");
		strcpy(cp[j]->ip[3],"null");
		j++;
	}

    /* Zero select FD sets */
    FD_ZERO(&master_list);//Initializes the file descriptor set fdset to have zero bits for all file descriptors. 
    FD_ZERO(&watch_list);
    
    /* Register the listening socket */
    FD_SET(server_socket, &master_list);//FD stands for file discriptor
    /* Register STDIN */
    FD_SET(STDIN, &master_list);

    head_socket = server_socket;

    for(;;)
    {
fflush(stdout);	
        memcpy(&watch_list, &master_list, sizeof(master_list));

        /* select() system call. This will BLOCK */
        selret = select(head_socket + 1, &watch_list, NULL, NULL, NULL);
        if(selret < 0)
        {
            perror("select failed.");
            exit(1);
        }

        if(selret > 0)
        {
        	 for(sock_index=0; sock_index<=head_socket; sock_index+=1)
            {
                fflush(stdout);
                memset(&server_data, '\0', sizeof(server_data));//mera
                if(FD_ISSET(sock_index, &watch_list))
                {
	        		/* Check if new command on STDIN */
	                if (sock_index == STDIN)
	                {
	                	char *cmd = (char*) malloc(sizeof(char)*CMD_SIZE);
	                	memset(cmd, '\0', CMD_SIZE);
						if(fgets(cmd, CMD_SIZE-1, stdin) == NULL) //Mind the newline character that will be written to cmd
							exit(-1);

						//printf("\nI got: %s\n", cmd);
						int len=strlen(cmd);
						cmd[len-1]='\0';// to remove \n from the msg
						
						//Process PA1 commands here ...cd 
						if((strcmp(cmd,outputs[0]))==0)
						{
							author_output();
						}
	                    else if((strcmp(cmd, outputs[1]))==0)
	                    {
							IP_Add();
							cse4589_print_and_log("[%s:END]\n",outputs[1]);
	                    }
	                    else if((strcmp(cmd, outputs[2]))==0)
	                    {
							myport();
							cse4589_print_and_log("[%s:END]\n",outputs[2]);
	                    }
	                    else if((strcmp(cmd, outputs[3]))==0)
	                    {
	                    	sortinglist_port();
	                    	cse4589_print_and_log("[%s:SUCCESS]\n",outputs[3]);
	                        print_list();
	                        cse4589_print_and_log("[%s:END]\n",outputs[3]);
	                    }
	                    
	                    
						free(cmd);
	                }
	                /* Check if new client is requesting connection */
	                else if(sock_index == server_socket)
	                {
	                    caddr_len = sizeof(client_addr);
	                    fdaccept = accept(server_socket, (struct sockaddr *)&client_addr, &caddr_len);// on success accept returns an integer, that is the file descriptor for the accepted socket.
	                    //printf("\n fdaccept index value:-%d",fdaccept);

                        fdaccept >= 0 ? printf("Accepted") : perror("Accept failed.");
	                    char ip[INET_ADDRSTRLEN];
	                    inet_ntop(AF_INET,&client_addr.sin_addr.s_addr,ip, INET_ADDRSTRLEN);

						printf("\nRemote Host connected! with IP:-%s \n", ip);                        
						ntohs(client_addr.sin_port);
	                    /* Add to watched socket list */
	                    FD_SET(fdaccept, &master_list);

                        head_socket = (fdaccept > head_socket) ? fdaccept : head_socket;

	                    char host[1024];
	                    getnameinfo((struct sockaddr *)&client_addr, caddr_len,host, sizeof(host), 0,0,0);
	                    printf("\nhost name is:-%s", host);
	                    printf("\nPort number is:-%d", ntohs(client_addr.sin_port));
	                    /*Add blocking information into the available structure*/
	                    int n=0;
						// while((cp[n]->C_id)!=0)
                        // {
                        // 	n++;
                        // }
						for(n=0;(cp[n]->C_id)!=0;n++);
                        if(n>=0){
                            cp[n]->C_id=n+1;
						    strcpy(cp[n]->C_ip,ip);
                        }
                        else{
                            exit(-1);
                        }
						
                     	/*Add new client information to the list*/
                        int m=0;
                        // while((lp[m]->list_id)!=0)
                        // {
                        // 	m++;
                        // }
						for(m=0;(lp[m]->list_id)!=0;m++);
                        // lp[m]->list_id=m+1;
                        // lp[m]->list_port=ntohs(client_addr.sin_port);
                        // lp[m]->fd_socket=fdaccept;
                        // lp[m]->snd_msg=0;
                        // lp[m]->rcv_msg=0;
                        // strcpy(lp[m]->state,"logged-in");
                        // strcpy(lp[m]->list_ip,ip);
                        // strcpy(lp[m]->list_host_name,host);
                        if(m>=0)
						{
                        lp[m]->list_id=m+1;
						}
						else
						{
							exit(-1);
						}
						if(m>=0)
                        {
							lp[m]->list_port=ntohs(client_addr.sin_port);
						}
						else
						{
							exit(-1);
						}
						if(m>=0)
                        {
							lp[m]->fd_socket=fdaccept;
						}
						else
						{
							exit(-1);
						}
                        if(m>=0)
                        {
							lp[m]->snd_msg=0;
						}
						else
						{
							exit(-1);
						}
                        if(m>=0)
                        {
							lp[m]->rcv_msg=0;
						}
						else
						{
							exit(-1);
						}
						strcpy(lp[m]->list_host_name,host);
						strcpy(lp[m]->list_ip,ip);
                        strcpy(lp[m]->state,"logged-in");

                        //send list of currently loged in clients to the client
                        sortinglist_port();

						//INFORMING THAT LOGIN IS OVER
						strcpy(server_data.cmd,"LOGLISTOVER");
						if(send(fdaccept, &server_data, sizeof(server_data), 0) == sizeof(server_data))
	                            {
	                     
									printf("Done!\n");
	                            }
							fflush(stdout);	
						//end 
	                }
	                else
	                {
	                    /* Initialize buffer to receieve response */
	                	memset(&rcv_data, '\0', sizeof(rcv_data));

	                    if(recv(sock_index, &rcv_data, sizeof(rcv_data), 0) <= 0)// recv returns length of the message on successful completion.
	                    {
	                    	/*Remove from the contetn_list as well*/
	                    	// remove_from_list(sock_index);
	                        //close(sock_index);
	                        printf("Remote Host terminated connection!\n");
	                        /* Remove from watched list */
	                        FD_CLR(sock_index,&master_list);
	                    }
	                    else 
	                    {
	                    	//Process incoming data from existing clients here ...
	                    	if((strcmp(rcv_data.cmd,outputs[3]))==0)
      	                    {
								int i=0;
	                    		while(i<5)
								{
									int k=lp[i]->list_id;
									if(k==0)
									{	
										printf("\nError to receive data from clients\n");
									}
									else
									{
										printf("%-5d%-35s%-20s%-8d\n", lp[i]->list_id, lp[i]->list_host_name, lp[i]->list_ip, lp[i]->list_port,lp[i]->fd_socket);
										
										send_list.list_id=lp[i]->list_id;
										strcpy(send_list.list_host_name,lp[i]->list_host_name);
										strcpy(send_list.list_ip,lp[i]->list_ip);
										send_list.list_port=lp[i]->list_port;

										strcpy(server_data.cmd,outputs[3]);
										server_data.list_row=send_list;
										if(send(sock_index, &server_data, sizeof(server_data), 0) == sizeof(server_data))
			                            {
											printf("Done!\n");
			                            }	
									}
									i++;
								}
								//informing that the list is over
								strcpy(server_data.cmd,"LISTOVER");
								if(send(sock_index, &server_data, sizeof(server_data), 0) == sizeof(server_data))
	                            {
									printf("Done!\n");
	                            }
	                            fflush(stdout);	
								
	                    	}
							fflush(stdout);

	                    }
	             	}   
	         	}
            }
        }
    }
}

int bind_the_socket(int c_port);
int connect_to_host(char *server_ip, int server_port, int c_port);

void author_output()
{
	const char *a[2] = {"nravula","AUTHOR"};
	cse4589_print_and_log("[%s:SUCCESS]\n",a[1]);
	cse4589_print_and_log("I, %s, have read and understood the course academic integrity policy.\n",a[0]);
	cse4589_print_and_log("[%s:END]\n",a[1]);
}

void myport();
void IP_Add();
void print_list();

int bind_the_socket(int c_port)
{
	struct sockaddr_in my_addrs;
	fdsocket = socket(AF_INET, SOCK_STREAM, 0);// return socket file descriptor
    // if(fdsocket < 0)
    // {
    //    perror("Failed to create socket");
    //    return 0;
    // }

	if(fdsocket > 0)
	{
		printf("Socked created successfully");
	}
	else
	{
		perror("Socket creation failed");
	}


    //setting up client socket
    my_addrs.sin_family=AF_INET;
    my_addrs.sin_addr.s_addr=INADDR_ANY;
    // my_addrs.sin_port=htons(c_port);

	uint16_t client_port_network_byte_order = htons(c_port);
	my_addrs.sin_port = client_port_network_byte_order;

    int optval=1;
    setsockopt(fdsocket, SOL_SOCKET, SO_REUSEPORT, &optval, sizeof(optval));

	int k=bind(fdsocket, (struct  sockaddr*) &my_addrs, sizeof(struct sockaddr_in));

    if(k != 0)
    {
		printf("\nTrouble in client port binding\n");
		return 0;
    }
    else
    {
    	printf("\nThe client successfully bound to the port\n");
    	return 1;
    }
}


int connect_to_host(char *server_ip, int server_port, int c_port)
{
    // int len;
    struct sockaddr_in remote_server_addr;

    bzero(&remote_server_addr, sizeof(remote_server_addr));
    remote_server_addr.sin_family = AF_INET;
    inet_pton(AF_INET, server_ip, &remote_server_addr.sin_addr);//inet_pton - convert IPv4 and IPv6 addresses from text to binary form
    // remote_server_addr.sin_port = htons(server_port);//function converts the unsigned short integer hostshort from host byte order to network byte order.

	uint16_t server_port_network_byte_order = htons(server_port);
	remote_server_addr.sin_port = server_port_network_byte_order;

    int k=connect(fdsocket, (struct sockaddr*)&remote_server_addr, sizeof(remote_server_addr));
	if(k>=0)
	{
		printf("Login successful");
	}
	else
	{
		perror("Connection failed");
	}
	return fdsocket;
}



void sortinglist_port() {
    // Bubble sort to sort the list by port number
    for (int i = 0; i < 5 - 1; i++) {
        for (int j = 0; j < 5 - i - 1; j++) {
            if (lp[j]->list_id == 0 || lp[j+1]->list_id == 0) {
                // One of the elements is empty, so swap is not needed
                continue;
            }
            if (lp[j]->list_port > lp[j+1]->list_port) {
                // Swap the elements
                struct client_info* temp = lp[j];
                lp[j] = lp[j+1];
                lp[j+1] = temp;
            }
        }
    }
}
void print_stats();
int isvalidIP(char *ip);
void sortinglist_port();
void print_list()
{
    int i=0;
	while(i<5)
	{
		if(lp[i]->list_id!=0)
		{	
			cse4589_print_and_log("%-5d%-35s%-20s%-8d\n", lp[i]->list_id, lp[i]->list_host_name, lp[i]->list_ip, lp[i]->list_port);
		}
        i++;
	}
}

void myport()
{
	const char *outputs[1]={"PORT"};
	struct sockaddr_in portvalue;
	socklen_t len=sizeof(portvalue);
	if(getsockname(fdsocket,(struct sockaddr *)&portvalue, &len) == -1)
	{
		cse4589_print_and_log("[%s:ERROR]\n", outputs[0]);
	}
	else{
		cse4589_print_and_log("[%s:SUCCESS]\n",outputs[0]);
		cse4589_print_and_log("%s:%d\n", outputs[0],ntohs(portvalue.sin_port));
	}
}



int isvalidIP(char *ip)
{
	struct sockaddr_in temp;
	return inet_pton(AF_INET, ip, &temp.sin_addr) == 1;
}

void IP_Add()
{
    const char* google_dns_server = "8.8.8.8";
    int dns_port = 53;
     
    struct sockaddr_in serv;
    memset( &serv, 0, sizeof(serv) );
	serv.sin_family = AF_INET;
    serv.sin_addr.s_addr = inet_addr( google_dns_server );
    serv.sin_port = htons( dns_port );

    int sock = socket ( AF_INET, SOCK_DGRAM, 0);
     
    if(sock < 0)
    {
        perror("Socket error");
		return;
    }
    int err = connect( sock , (const struct sockaddr*) &serv , sizeof(serv) );
    if(err<0)
	{
		perror("Connection error");
		close(sock);
		return;
	}
    struct sockaddr_in name;
    socklen_t namelen = sizeof(name);
    err = getsockname(sock, (struct sockaddr*) &name, &namelen);
    if(err<0)
	{
		perror("Getting socket name error");
		close(sock);
		return;
	}
	char buffer[100];
    const char* p = inet_ntop(AF_INET, &name.sin_addr, buffer, 100);
    if(p == NULL)
    {
        cse4589_print_and_log("[IP:ERROR]\n");
    }
    else
    {
        cse4589_print_and_log("[IP:SUCCESS]\n");
        cse4589_print_and_log("IP:%s\n", buffer);
    }
    close(sock);
}

file_des()
{
	FD_ZERO(&client_master_list); 
    FD_ZERO(&client_watch_list);
    FD_SET(STDIN, &client_master_list);
}