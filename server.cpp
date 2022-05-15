#include <iostream>
#include <netdb.h>
#include <netinet/in.h>
#include <string>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <vector>
#include <sys/un.h>

#define PORT 8080
#define SA struct sockaddr

struct User {
	std::string username;
	int sockfd;
};
//2::alex::arno::text
void deserialize(std::string& incoming_message, std::vector<User>& users) {
	std::string from;
	int i = 3;
	for(;i < incoming_message.size(); ++i) {
		if(incoming_message[i] == ':') {
			break;
		}
		from.push_back(incoming_message[i]);
	}
	i += 2;
	std::string to;
	for(;i < incoming_message.size(); ++i) {
		if(incoming_message[i] == ':') {
			break;
		}
	}
	i += 2;
	std::string text;
	for(;i < incoming_message.size(); ++i) {
		text.push_back(incoming_message[i]);
	}
	for(int i = 0; i < users.size(); ++i) {
		if(users[i].username == to) {
			write(users[i].sockfd, &text, sizeof(text));
		}
	}
}

void func(std::string& incoming_message, std::vector<User>& users, int sockfd) {
	std::string username;
	if(incoming_message[0] == '1') {
		std::string tmp(incoming_message.begin() + 3, incoming_message.end());
		username = tmp;
		for(int i = 0; i < users.size(); ++i) {
			if(users[i].username == username) {
				std::string buff = "2";
				write(sockfd, &buff, sizeof(buff));
				return;
			}
		}
		std::string buff = "1";
		write(sockfd, &buff, sizeof(buff));
	}
	deserialize(incoming_message, users);
}

// Function designed for chat between client and server.
void chat(int connfd, std::vector<User>& users) {
	std::string incoming_message;
	std::string sending_message;
	while(true) {
		read(connfd, &incoming_message, sizeof(incoming_message));
		func(incoming_message, users, connfd);
	}
}
int main() {
	int sockfd;
	std::vector<User> users;
	struct sockaddr_in clt_addr;
	socklen_t len;
	
	//bzero(&srv_addr, sizeof(srv_addr));

	struct addrinfo *p, *servinfo;
	
	//loop trough all the results and bind first we can
	User tmp;
	for(p = servinfo; p != NULL; p = p->ai_next) {
		if((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
			perror("server: socket");
			continue;
		}
		if(bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
			close(sockfd);
			perror("server: bind");
			continue;
		}
		tmp.sockfd = sockfd;
		users.push_back(tmp);
	}

	len = sizeof(clt_addr);
	// Accept the data packet from client and verification
	for(int i = 0; i < users.size(); ++i) {
		users[i].sockfd = accept(sockfd, (SA*)&clt_addr, &len);
		if (users[i].sockfd < 0) {
			std::cout << "server accept failed...\n";
			exit(0);
		}
		else {
			std::cout << "server accept the client...\n";
		}
		// Function for chatting between client and server
		chat(users[i].sockfd, users);
	}
	// After chatting close the socket
	for(int i = 0; i < users.size(); ++i) {
		close(users[i].sockfd);
	}
	return 0;
}
