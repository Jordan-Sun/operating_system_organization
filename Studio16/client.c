// declarations
#include "client.h"
// utils
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
// for socket(), bind()
#include <sys/types.h>
#include <sys/socket.h>
// for unix
#include <sys/un.h>

int err_handler(int err) {
	perror(err_desc[err]);
	return err;
}

int main(int argc, char* argv[])
{
	/// ref: man unix
	
	// stores the file descriptor for the sockets.
	unsigned int data_socket;
	// stores the unix domain socket address.
	struct sockaddr_un name;
	// stores the return value of system calls.
	int ret;
	// stores a file pointer to the socket.
	FILE * data_fp;
	// stores the data and converted data to send.
	unsigned int data;
	
	// unlink the socket in case of previous exceptions.
	unlink(SOCKET_NAME);
	
	// create a socket for local connection.
	data_socket = socket(AF_LOCAL, SOCK_STREAM, SOCKET_PROTOCAL);
	// on error, -1 is returned.
	if (data_socket < SUCCESS) return err_handler(ERR_SOCKET);
	
	// connect to the communications channel.
	memset(&name, 0, sizeof(struct sockaddr_un));
	name.sun_family = AF_LOCAL;
    strncpy(name.sun_path, SOCKET_NAME, sizeof(name.sun_path) - 1);
	ret = connect(data_socket, (const struct sockaddr *) &name,
                      sizeof(struct sockaddr_un));
	// on error, -1 is returned.
	if (ret < SUCCESS) return err_handler(ERR_CONNECT);
	
	// connected, open socket.
	data_fp = fdopen(data_socket, "r");
	// on error, NULL is returned.
	if (data_fp == NULL) return err_handler(ERR_OPEN);
	
	// send data.
	for (data = 0; data < DATA_MAX; ++data)
	{
		ret = fprintf(data_fp, "%u", data);
		if (ret < SUCCESS) return err_handler(ERR_SEND);
	}
	
	// read completed or failed.
	fclose(data_fp);
    // close and unlink the data socket.
    close(data_socket);
    unlink(SOCKET_NAME);

	// successful execution
	return SUCCESS;
}

