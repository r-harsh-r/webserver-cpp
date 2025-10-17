#include<iostream>
#include<sys/types.h>
#include<cstring>
#include<sys/socket.h>
#include<netdb.h>
#include <arpa/inet.h>
#include<unistd.h>
#include<string>
using namespace std;

#define PORT "8080"

string getMessage(char*buff,int size){
    string msg = "";
    
    int stIdx= 0;
    for(stIdx= 0;stIdx < size;stIdx++){
        if(buff[stIdx] == '/') break;
        // cout<<buff[stIdx];
    }
    // cout<<endl;
    int enIdx = stIdx;
    for(enIdx ; enIdx + 3 < size ; enIdx++){
        string isHttp;
        isHttp.push_back(buff[enIdx]);
        isHttp.push_back(buff[enIdx + 1]);
        isHttp.push_back(buff[enIdx + 2]);
        isHttp.push_back(buff[enIdx + 3]);

        if(isHttp == "HTTP"){
            break;
        }
    }

    // cout<<stIdx<<" "<<enIdx<<endl;

    for(int idx = stIdx ; idx + 1< enIdx ; idx++){
        msg += buff[idx];
    }

    return msg;

}

int main(){
    int sockfd;

    // getting the address
    struct addrinfo hints;
    struct addrinfo * res;

    memset(&hints,0,sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    int status = getaddrinfo(NULL,PORT,&hints,&res);

    if(status != 0){
        fprintf(stderr , "gai error : %s\n",gai_strerror(status));
        exit(1);
    }

    int flg = 0;
    for(struct addrinfo * ptr = res;ptr != NULL;ptr = ptr->ai_next){
        sockfd = socket(res->ai_family,res->ai_socktype,res->ai_protocol);

        if(sockfd == -1){
            perror("sockif error");
            continue;
        }
        
        if(bind(sockfd,res->ai_addr,res->ai_addrlen) == -1){
            perror("binding error");
            continue;
        }

        cout<<"\t Server started, waiting for connection..."<<endl;
        flg = 1;
        break;
    }

    freeaddrinfo(res);

    if(!flg){
        cout<<"Failed starting server"<<endl;
        exit(1);
    }

    // listen
    listen(sockfd,10);

    while(true){
        struct sockaddr_storage their_addr;
        socklen_t sin_size = sizeof their_addr;
        int clifd = accept(sockfd,(struct sockaddr *)&their_addr,&sin_size);

        char buff[25];

        
        int byteRecvd = recv(clifd,buff,25,0);
        buff[byteRecvd] = '\0';
        string message = getMessage(buff,25);
        // cout<<"Message received from client end : "<<buff<<endl;
        cout<<"\tmessage recieved : "<<message<<endl;

        string toSend = "HTTP/1.1 200 OK\r\n\r\nRequested path: " + message + "\r\n";
        send(clifd,(toSend.c_str()),toSend.length(),0);
        // send(clifd,"HTTP/1.1 hello\n",25,0);
        // break;
    }


}