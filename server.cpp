#include<iostream>
#include<sys/types.h>
#include<cstring>
#include<sys/socket.h>
#include<netdb.h>
#include <arpa/inet.h>
#include<unistd.h>
#include<string>
#include<fstream>
#include<sstream>
#include<thread>
using namespace std;

#define PORT "8080"
#define BUFF_SIZE 1024

void wait(){
    for(long long i = 0;i<1e10 + 1e9;i++){
    }

}

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

string getFileFromPath(string &path){
    if(path == "/") path = "/index.html";

    path = "./www" + path;

    string fileContent = "";

    ifstream file(path);

    if(file.is_open()){
        stringstream file_buffer;
        file_buffer<<file.rdbuf();
        fileContent = file_buffer.str();
        file.close();
    }
    return fileContent;

}


// Race condition (source : Gemini)
// You call send(...):
// Your program says, "Hey OS, here's a bunch of data to send."
// The OS copies your data from your program's memory into its own kernel-level network buffer.
// This copy is extremely fast (memory-to-memory).
// As soon as the copy is done, send() returns immediately.
// At this exact moment:
// Your program thinks, "Great, send() is done!"
// The OS is only just beginning to package up that data from its buffer to send over the slow network (e.g., Wi-Fi or Ethernet).
// You call close(...):
// This is the very next instruction. Your program, not waiting for the network, executes it.
// close() tells the OS: "I am immediately done with this socket. Drop the connection. Discard any data in its buffers that hasn't been sent."
// The Result:
// The OS obeys close() because it's a direct command.
// It discards the data it was just about to send and sends a RST (Reset) packet to the client.
// The client, which was expecting data, gets a "Connection reset by peer" error.
// close() wins the race because the send() call "finishes" (by returning) long before the network operation it triggers is finished

void handleConnection(int clifd){
    std::thread::id this_id = std::this_thread::get_id();

    char buff[BUFF_SIZE];

        
    int byteRecvd = recv(clifd,buff,BUFF_SIZE - 1,0);   // off by 1 error : sol => BUFFSIZE - 1

    if(byteRecvd <= 0){     // -1 for error and 0 for close request
        close(clifd);
        return;
    }

    buff[byteRecvd] = '\0';
    string path = getMessage(buff,25);

    string fileContent = getFileFromPath(path);

    string toSend = "HTTP/1.1 200 OK\r\n\r\n " + fileContent + "\r\n";
    if(fileContent == ""){
        toSend = "HTTP/1.1 400 Not Found\r\n\r\n";
    }

    // wait();

    cout<<"\tSent"<<endl;

    send(clifd,(toSend.c_str()),toSend.length(),0);

    shutdown(clifd,SHUT_WR);        // graceful shutdown

    close(clifd);
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

        // accept the incoming request
        int clifd = accept(sockfd,(struct sockaddr *)&their_addr,&sin_size);    // blocking call

        // handleConnection concurrently in different thread
        thread t(handleConnection,clifd);
        t.detach();                         // fire and forgot
    }
}