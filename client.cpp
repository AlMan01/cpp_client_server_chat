#include <iostream>
#include <netdb.h>
#include <string>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/un.h>

#define PORT 8080
#define SA struct sockaddr

//first protocol: type::username
//sending message type is 1
std::string serialize_by_first(std::string username){
	std::string fin_str;
	fin_str += "1::";
	fin_str += username;
	return fin_str;
}

//second protocol: type::from::to::text
//sending message type is 2 
std::string serialize_by_second(std::string sender, std::string getter, std::string text){
	std::string fin_str;
	fin_str += "2::";
	fin_str += sender;
	fin_str += "::";
	fin_str += getter;
	fin_str += "::";
	fin_str += text;
	return fin_str;
}

//deserialize by second protocol
//and print
void deserialize(std::string message){
	//2::alex::arno::text;
	std::string from;
	int i = 3;
	for(;i<message.size();++i){
		if(message[i] == ':'){
			break;
		}
		from.push_back(message[i]);
	}
	i+=2;
	for(;i<message.size();++i){
		if(message[i] == ':'){
			break;
		}
	}
	i+=2;
	std::string text;
	for(;i<message.size();++i){
		text.push_back(message[i]);
	}
	std::cout << "From " << from << ": " << text << std::endl;
}	



void chat(int sockfd,std::string username){
	std::string sending_message_to;
	std::string sending_message_text;
	std::string sending_message;
	std::string incoming_message;
	while(true){
		std::cout << "To: ";
		std::cin >> sending_message_to;
		std::cout << "Text: ";
		std::cin >> sending_message_text;
		if(sending_message_to == "Me" && sending_message_text == "Exit"){
			exit(0);
		}
		sending_message = serialize_by_second(username, sending_message_to, sending_message_text);
		write(sockfd,&sending_message,sending_message.size());
		read(sockfd,&incoming_message,incoming_message.size());
		deserialize(incoming_message);
	}

	
}

int main(){
	int sockfd, connfd;
	struct sockaddr_in servaddr;

	//socket create and verification
	sockfd = socket(AF_INET, SOCK_STREAM,0);
	if(sockfd == -1){
		std::cout<<"socket creation failed..."<<std::endl;
		exit(0);
	}
	else{
		std::cout<<"socket created successfully..."<<std::endl;
	}
	bzero(&servaddr,sizeof(servaddr));
	
	//assign IP,PORT
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");
	servaddr.sin_port = htons(PORT);

	std::string username;
	std::cout << "Enter your username: ";

	//connecting client to server for checking free usernames
	if(connect(sockfd,(SA*)& servaddr, sizeof(servaddr)) != 0){
		exit(0);
	}

	while(true){
		std::cin >> username;
		//serialize username with 1st protocol
		std::string ser_str = serialize_by_first(username);
		write(sockfd,&ser_str,ser_str.size());
		std::string ver_str;
		//read verification message
		read(sockfd,&ver_str,ver_str.size());
		if(ver_str == "1"){
			break;
		}
		ver_str.clear();
		std::cout<< "Existing one. Try another username"<<std::endl;
	}
	//at this point we have finally connected to server
	std::cout << "Connected to server with username: " << username;
	//now let's start the chat
	chat(sockfd,username);
	
	//close the socket
	close(sockfd);

	return 0;
}
	

