/*******************************************************************************
* Simple FIFO Order Server Implementation
*
* Description:
*     A server implementation designed to process client requests in First In,
*     First Out (FIFO) order. The server binds to the specified port number
*     provided as a parameter upon launch.
*
* Usage:
*     <build directory>/server <port_number>
*
* Parameters:
*     port_number - The port number to bind the server to.
*
* Author:
*     Renato Mancuso
*
* Affiliation:
*     Boston University
*
* Creation Date:
*     September 10, 2023
*
* Notes:
*     Ensure to have proper permissions and available port before running the
*     server. The server relies on a FIFO mechanism to handle requests, thus
*     guaranteeing the order of processing. For debugging or more details, refer
*     to the accompanying documentation and logs.
*
*******************************************************************************/

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

/* Include struct definitions and other libraries that need to be
 * included by both client and server */
#include "common.h"

#define BACKLOG_COUNT 100
#define USAGE_STRING				\
	"Missing parameter. Exiting.\n"		\
	"Usage: %s <port_number>\n"

/* Main function to handle connection with the client. This function
 * takes in input conn_socket and returns only when the connection
 * with the client is interrupted. */
static void handle_connection(int conn_socket)
{
	//my main function will consist soley of an infinite loop that will call an external handle_request function
	while (conn_socket) { //while(1) is equivalent to true
		handle_request(conn_socket); //allows the server to continuously process requests from the client as long as the connection is active
	}
	//printf("client disconnected");
	//fflush(stdout);

	close(conn_socket);
	
}

/* Takes an integer parameter conn_socket, similarly to */
void handle_request(int conn_socket)
{
	uint64_t request_id; //my unsigned 64 bit integer
	struct timespec sent_time, request_length, receipt_time, completion_time; //"timespec is a structure in c used to store data such as time-elapsed"- educative.io
	//so all variables declared above will be timestamps
	ssize_t bytes_received = recv(conn_socket, &request_id, sizeof(uint64_t), 0); //recv()=receive a message from a connected socket. receives data from the client through the 64bit int
	//and and stores it in the request_id var. sizeof(uint64_t) ensures correct num of bytes are read
	if (bytes_received <= 0) {//checks if any data was received
        return;
    }
	recv(conn_socket, &sent_time, sizeof(struct timespec), 0); //reads sent time 
    recv(conn_socket, &request_length, sizeof(struct timespec), 0); //reads request length
	clock_gettime(CLOCK_MONOTONIC, &receipt_time); //stores current time in receipt_time var
	busy_wait(request_length); //allows the server to wait without blocking the entire program, request_length specifies wait time
	clock_gettime(CLOCK_MONOTONIC, &completion_time); //gets time and stores in completion_time var
	send(conn_socket, &request_id, sizeof(uint64_t), 0); //sends a response of request_id and its size back to the client
	
	int id = request_id; //i previously directly used request_id in the printf statement

	//the following lines of code convert the timestamps and request lengths from struct timespec to human readable fractions and decimals
    double sentsec = sent_time.tv_sec + sent_time.tv_nsec * 1e-9;
    double lensec = request_length.tv_sec + request_length.tv_nsec * 1e-9;
    double recsec = receipt_time.tv_sec + receipt_time.tv_nsec * 1e-9;
    double compsec = completion_time.tv_sec + completion_time.tv_nsec * 1e-9;

	//"R<ID>:<sent timestamp>,<request length>,<receipt timestamp>,<completion timestamp>"
    printf("R%d:%.6f,%.6f,%.6f,%.6f\n", id, sentsec, lensec, recsec, compsec);
	fflush(stdout);
}

//perform a busy wait for the specified amount of time
void busy_wait(struct timespec wait_time) 
{
    struct timespec start_time, current_time;
    clock_gettime(CLOCK_MONOTONIC, &start_time);
    
    while (1) {
        clock_gettime(CLOCK_MONOTONIC, &current_time);
        if (((current_time.tv_sec + current_time.tv_nsec * 1e-9) - (start_time.tv_sec + start_time.tv_nsec * 1e-9)) >= (wait_time.tv_sec + wait_time.tv_nsec * 1e-9)) {
            break;
        }
    }
}

/* Template implementation of the main function for the FIFO
 * server. The s\
erver must accept in input a command line parameter
 * with the <port number> to bind the server to. */
int main (int argc, char ** argv) {
	int sockfd, retval, accepted, optval;
	in_port_t socket_port;
	struct sockaddr_in addr, client;
	struct in_addr any_address;
	socklen_t client_len;

	/* Get port to bind our socket to */
	if (argc > 1) {
		socket_port = strtol(argv[1], NULL, 10);
		printf("INFO: setting server port as: %d\n", socket_port);
	} else {
		ERROR_INFO();
		fprintf(stderr, USAGE_STRING, argv[0]);
		return EXIT_FAILURE;
	}

	/* Now onward to create the right type of socket */
	sockfd = socket(AF_INET, SOCK_STREAM, 0);

	if (sockfd < 0) {
		ERROR_INFO();
		perror("Unable to create socket");
		return EXIT_FAILURE;
	}

	/* Before moving forward, set socket to reuse address */
	optval = 1;
	setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, (void *)&optval, sizeof(optval));

	/* Convert INADDR_ANY into network byte order */
	any_address.s_addr = htonl(INADDR_ANY);

	/* Time to bind the socket to the right port  */
	addr.sin_family = AF_INET;
	addr.sin_port = htons(socket_port);
	addr.sin_addr = any_address;

	/* Attempt to bind the socket with the given parameters */
	retval = bind(sockfd, (struct sockaddr *)&addr, sizeof(struct sockaddr_in));

	if (retval < 0) {
		ERROR_INFO();
		perror("Unable to bind socket");
		return EXIT_FAILURE;
	}

	/* Let us now proceed to set the server to listen on the selected port */
	retval = listen(sockfd, BACKLOG_COUNT);

	if (retval < 0) {
		ERROR_INFO();
		perror("Unable to listen on socket");
		return EXIT_FAILURE;
	}

	/* Ready to accept connections! */
	printf("INFO: Waiting for incoming connection...\n");
	client_len = sizeof(struct sockaddr_in);
	accepted = accept(sockfd, (struct sockaddr *)&client, &client_len);

	if (accepted == -1) {
		ERROR_INFO();
		perror("Unable to accept connections");
		return EXIT_FAILURE;
	}

	/* Ready to handle the new connection with the client. */
	handle_connection(accepted);

	close(sockfd);
	return EXIT_SUCCESS;

}
