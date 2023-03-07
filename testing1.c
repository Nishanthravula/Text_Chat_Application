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
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>



#define MSG_SIZE 256
#define BACKLOG 5
#define STDIN 0
#define CMD_SIZE 100




struct clt_msg
{
	char cmd[20];
	char info[256];
	char ip[32];	
};
struct clt_msg rData;


struct lt_cnt
{
	int rMsg;
	int lt_port;
	int sdMsg;
	int lt_id;
	char lt_ip[32];
	int fd_skt;
	char lt_ht_name[40];
	char state[20];

}*lp[5];
struct lt_cnt send_list;

/* Structure to represent messages from the server */
struct server_msg {
    char cmd[20]; /* Command associated with the message */
    char sender_ip[32]; /* IP address of the sender */
    char info[256]; /* Additional information associated with the message */
    struct lt_cnt list_row; /* Struct to hold information about a row in a list */
};
struct server_msg server_data;

struct clt_list
{
	char buffer[1024];
	int C_id;
	char C_ip[32];
	char ip[4][32];
	
}*cp[5];
int fdsocket,clt_head_skt, clt_sock_index;
fd_set clt_master_list, clt_watch_list;

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
	
	struct lt_cnt hosts[6];
    int port = atoi(argv[2]);
    char mode = *argv[1];

    switch (mode) {
        case 'c':
            cSide(port);
            break;
        case 's':
            s_Side(port);
            break;
        default:
            exit(-1);
    }
    	return 0;
}

//clt side
void cSide(int clt_port);
void s_Side(int server_port);

void cSide(int clt_port)
{
	int bind_port=bind_the_socket(clt_port);
	int loggedin=0;
	const char *outputs[7] = {"AUTHOR", "IP", "PORT","LIST","LOGIN","REFRESH","EXIT","LOGOUT","SUCCESS"};
	if(bind_port==0)
	{
		exit(-1);
	}

	printf("\ninside clt side");
	int server=0;
	int selret;
	int j=0;
	struct clt_msg data;
	file_des();
    clt_head_skt=0;
	for(;;)
	{
		fflush(stdout);	
		file_des();
		FD_SET(server, &clt_master_list);
		clt_head_skt=server;

		memcpy(&clt_watch_list, &clt_master_list, sizeof(clt_master_list));

        selret = select(clt_head_skt + 1, &clt_watch_list, NULL, NULL, NULL);
        if(selret <= 0)
        {
            perror("select failed.");
            exit(-1);
        }

        else
        {

            for(clt_sock_index=0; clt_sock_index<=clt_head_skt; clt_sock_index+=1)
            {
                if(FD_ISSET(clt_sock_index, &clt_watch_list))
                {
                	 if (clt_sock_index == STDIN)
                    {
				        char *msg = (char*) malloc(sizeof(char)*MSG_SIZE);
				    	memset(msg, '\0', MSG_SIZE);
						if(fgets(msg, MSG_SIZE-1, stdin) == NULL) 
							exit(-1);
						
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
                                    server = connect_to_host(ip, atoi(portv), clt_port);
                                    FD_SET(server, &clt_master_list);
                                    clt_head_skt = server;
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
                        int L = recv(server, &srcv, sizeof(srcv),0); 


						if(L >= 0)

						{

							if(strcmp(srcv.cmd,outputs[3])==0)

							{	

								cse4589_print_and_log("%-5d%-35s%-20s%-8d\n",srcv.list_row.lt_id,srcv.list_row.lt_ht_name,srcv.list_row.lt_ip,srcv.list_row.lt_port);

							}
							
							else if(strcmp(srcv.cmd,"LISTOVER")==0)
							{	
								cse4589_print_and_log("[%s:END]\n",outputs[3]);
							}
										
							fflush(stdout);
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
	int head_socket;
	int selret;
	int sock_index;
	int fdaccept = 0;
	int caddr_len;
	int send_socket = 0;
    int server_socket;
	struct sockaddr_in server_addr, clt_addr;
	fd_set master_list, watch_list;
	server_socket = socket(AF_INET, SOCK_STREAM, 0);
	fdsocket=server_socket;
    (fdsocket >= 0) ? printf("\nCreated Socket successfully\n") : perror("socket cannot be created");
	bzero(&server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(server_port);
	int k = bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr));
    if(k >= 0 )
	{
		printf("\nSocket Binded Successfully\n");
    }
    else
	{
		perror("Binding the socket failed");
    }
	int m = listen(server_socket, BACKLOG);
    if(m >= 0)
	{
    	printf("\nSocket listening\n");
    }
    else
	{
		perror("Listening on port is not possible");
    }
	int i =0;
	while(i<5)
    {
    	lp[i]=(struct lt_cnt *)malloc(sizeof(struct lt_cnt));
    	lp[i]->lt_id=0; 	
		i++;
    }
	int j=0;
	while(j<5){
        if(j>=0){
            cp[j]=(struct lt_cnt *)malloc(sizeof(struct clt_list));
        }
        if(j>=0){
            cp[j]->C_id=0;
        }
		if(j>=0){
            strcpy(cp[j]->ip[0],"null");
        }    
		if(j>=0){
            strcpy(cp[j]->ip[1],"null");
        }    
		if(j>=0){
            strcpy(cp[j]->ip[2],"null");
        }    
		if(j>=0){
		    strcpy(cp[j]->ip[3],"null");
        }
        else{
            exit(-1);
        }

		j++;
	}
    FD_ZERO(&master_list);
    FD_ZERO(&watch_list);
    FD_SET(server_socket, &master_list);
    FD_SET(STDIN, &master_list);
    head_socket = server_socket;

    for(;;)
    {
        fflush(stdout);	
        memcpy(&watch_list, &master_list, sizeof(master_list));
        selret = select(head_socket + 1, &watch_list, NULL, NULL, NULL);
        if(selret <= 0)
        {
            perror("select failed.");
            exit(-1);
        }
        else
        {
        	 for(sock_index=0; sock_index<=head_socket; sock_index+=1)
            {
                fflush(stdout);
                memset(&server_data, '\0', sizeof(server_data));//mera
                if(FD_ISSET(sock_index, &watch_list))
                {
	                if (sock_index == STDIN)
	                {
	                	char *cmd = (char*) malloc(sizeof(char)*CMD_SIZE);
	                	memset(cmd, '\0', CMD_SIZE);
						if(fgets(cmd, CMD_SIZE-1, stdin) == NULL)
							exit(-1);
						int len=strlen(cmd);
						cmd[len-1]='\0';
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
	                    	sortinglt_port();
	                    	cse4589_print_and_log("[%s:SUCCESS]\n",outputs[3]);
	                        print_list();
	                        cse4589_print_and_log("[%s:END]\n",outputs[3]);
	                    }     
						free(cmd);
	                }
	                else if(sock_index == server_socket)
	                {
	                    caddr_len = sizeof(clt_addr);
	                    fdaccept = accept(server_socket, (struct sockaddr *)&clt_addr, &caddr_len);
                        fdaccept >= 0 ? printf("Accepted") : perror("Accept failed.");
	                    char ip[INET_ADDRSTRLEN];
	                    inet_ntop(AF_INET,&clt_addr.sin_addr.s_addr,ip, INET_ADDRSTRLEN);
						printf("\nRemote Host connected! with IP:-%s \n", ip);                        
						ntohs(clt_addr.sin_port);
	                    FD_SET(fdaccept, &master_list);
                        head_socket = (fdaccept > head_socket) ? fdaccept : head_socket;
	                    char host[1024];
	                    getnameinfo((struct sockaddr *)&clt_addr, caddr_len,host, sizeof(host), 0,0,0);
	                    printf("\nhost name is:-%s", host);
	                    printf("\nPort number is:-%d", ntohs(clt_addr.sin_port));
	                    int n=0;
						for(n=0;(cp[n]->C_id)!=0;n++);
                        if(n>=0){
                            cp[n]->C_id=n+1;
						    strcpy(cp[n]->C_ip,ip);
                        }
                        else{
                            exit(-1);
                        }			
                     	/*Add new clt information to the list*/
                        int m=0;
						for(m=0;(lp[m]->lt_id)!=0;m++);
                        if(m>=0)
						{
                        lp[m]->lt_id=m+1;
						}
						else
						{
							exit(-1);
						}
						if(m>=0)
                        {
							lp[m]->lt_port=ntohs(clt_addr.sin_port);
						}
						else
						{
							exit(-1);
						}
						if(m>=0)
                        {
							lp[m]->fd_skt=fdaccept;
						}
						else
						{
							exit(-1);
						}
                        if(m>=0)
                        {
							lp[m]->sdMsg=0;
						}
						else
						{
							exit(-1);
						}
                        if(m>=0)
                        {
							lp[m]->rMsg=0;
						}
						else
						{
							exit(-1);
						}
						strcpy(lp[m]->lt_ht_name,host);
						strcpy(lp[m]->lt_ip,ip);
                        strcpy(lp[m]->state,"logged-in");
                        sortinglt_port();
						strcpy(server_data.cmd,"LOGLISTOVER");
						if(send(fdaccept, &server_data, sizeof(server_data), 0) == sizeof(server_data))
	                            {
	                     
									printf("Done!\n");
	                            }
							fflush(stdout);	
	                }
	                else
	                {
	                    /* Initialize buffer to receieve response */
	                	memset(&rData, '\0', sizeof(rData));

	                    if(recv(sock_index, &rData, sizeof(rData), 0) <= 0)// recv returns length of the message on successful completion.
	                    {

	                        printf("Remote Host terminated connection!\n");

	                        FD_CLR(sock_index,&master_list);
	                    }
	                    else 
	                    {
	                    	//Process incoming data from existing clts here ...
	                    	if((strcmp(rData.cmd,outputs[3]))==0)
      	                    {
								int i=0;
	                    		while(i<5)
								{
									int k=lp[i]->lt_id;
									if(k==0)
									{	
										printf("\nError to receive data from clts\n");
									}
									else
									{
										printf("%-5d%-35s%-20s%-8d\n", lp[i]->lt_id, lp[i]->lt_ht_name, lp[i]->lt_ip, lp[i]->lt_port,lp[i]->fd_skt);
                                        set_lt_cnt(&send_list, lp[i]->lt_id, lp[i]->lt_ht_name, lp[i]->lt_ip, lp[i]->lt_port);
										send_list.lt_port=lp[i]->lt_port;
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
void set_lt_cnt(struct lt_cnt *lc, int id, char *host_name, char *ip, int port);
void set_lt_cnt(struct lt_cnt *lc, int id, char *host_name, char *ip, int port) {
    lc->lt_id = id;
    strcpy(lc->lt_ht_name, host_name);
    strcpy(lc->lt_ip, ip);
    lc->lt_port = port;
}
int bind_the_socket(int c_port);
int connect_to_host(char *server_ip, int server_port, int c_port);
file_des()
{
	FD_ZERO(&clt_master_list); 
    FD_ZERO(&clt_watch_list);
    FD_SET(STDIN, &clt_master_list);
}
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
	if(fdsocket > 0)
	{
		printf("Socked created successfully");
	}
	else
	{
		perror("Socket creation failed");
	}
  //setting up clt socket
    my_addrs.sin_family=AF_INET;
    my_addrs.sin_addr.s_addr=INADDR_ANY;
	uint16_t clt_port_network_byte_order = htons(c_port);
	my_addrs.sin_port = clt_port_network_byte_order;
    int optval=1;
    setsockopt(fdsocket, SOL_SOCKET, SO_REUSEPORT, &optval, sizeof(optval));
	int k=bind(fdsocket, (struct  sockaddr*) &my_addrs, sizeof(struct sockaddr_in));
    if(k != 0)
    {
		printf("\nTrouble in clt port binding\n");
		return 0;
    }
    else
    {
    	printf("\nThe clt successfully bound to the port\n");
    	return 1;
    }
}
int connect_to_host(char *server_ip, int server_port, int c_port)
{
    struct sockaddr_in remote_server_addr;
    bzero(&remote_server_addr, sizeof(remote_server_addr));
    remote_server_addr.sin_family = AF_INET;
	//inet_pton - convert IPv4 and IPv6 addresses from text to binary form
    inet_pton(AF_INET, server_ip, &remote_server_addr.sin_addr);
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
void sortinglt_port() {
    // Bubble sort to sort the list by port number
    for (int i = 0; i < 5 - 1; i++) {
        for (int j = 0; j < 5 - i - 1; j++) {
            if (lp[j]->lt_id == 0 || lp[j+1]->lt_id == 0) {
                // One of the elements is empty, so swap is not needed
                continue;
            }
            if (lp[j]->lt_port > lp[j+1]->lt_port) {
                // Swap the elements
                struct clt_info* temp = lp[j];
                lp[j] = lp[j+1];
                lp[j+1] = temp;
            }
        }
    }
}
void print_stats();
int isvalidIP(char *ip);
void sortinglt_port();
void print_list()
{
    int i=0;
	while(i<5)
	{
		if(lp[i]->lt_id!=0)
		{	
			cse4589_print_and_log("%-5d%-35s%-20s%-8d\n", lp[i]->lt_id, lp[i]->lt_ht_name, lp[i]->lt_ip, lp[i]->lt_port);
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

