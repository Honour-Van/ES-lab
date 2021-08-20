#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/time.h>
#define BUFLEN 1024 
#define PORT 6666
#define LISTNUM 20
  
void server()
{
    int sockfd, newfd; 
    struct sockaddr_in s_addr, c_addr; 
    char buf[BUFLEN]; 
    socklen_t len; 
    unsigned int port, listnum; 
    fd_set rfds; 
    struct timeval tv; 
    int retval,maxfd; 
      
    /*建立socket*/ 
    if((sockfd = socket(PF_INET, SOCK_STREAM, 0)) == -1){ 
        perror("socket"); 
        exit(errno); 
    }else 
        printf("[init] socket create success\n"); 
    memset(&s_addr,0,sizeof(s_addr)); 
    s_addr.sin_family = AF_INET; 
    s_addr.sin_port = htons(PORT); 
    s_addr.sin_addr.s_addr = htons(INADDR_ANY); 
    
    /*把地址和端口帮定到套接字上*/ 
    if((bind(sockfd, (struct sockaddr*) &s_addr,sizeof(struct sockaddr))) == -1){ 
        perror("bind"); 
        exit(errno); 
    }else 
        printf("[init] bind success\n"); 
    /*侦听本地端口*/ 
    if(listen(sockfd,listnum) == -1){ 
        perror("listen"); 
        exit(errno); 
    }else 
        printf("[conn] the server is listening...\n"); 
    while(1){
        len = sizeof(struct sockaddr); 
        if((newfd = accept(sockfd,(struct sockaddr*) &c_addr, &len)) == -1){ 
            perror("accept"); 
            exit(errno); 
        }else 
            printf("[conn] client %s connecting, port:%d\n",inet_ntoa(c_addr.sin_addr),ntohs(c_addr.sin_port)); 
        while(1){ 
            FD_ZERO(&rfds);
            FD_SET(0, &rfds);
            maxfd = 0;
            FD_SET(newfd, &rfds);
            /*找出文件描述符集合中最大的文件描述符*/ 
            if(maxfd < newfd)
                maxfd = newfd;
            /*设置超时时间*/
            tv.tv_sec = 6;
            tv.tv_usec = 0;
            /*等待聊天*/
            retval = select(maxfd+1, &rfds, NULL, NULL, &tv);
            if(retval == -1){
                printf("[exit] select went wrong, client exited\n");
                break;
            }else if(retval == 0){
                printf("[wait] server waiting...\n"); 
                continue; 
            }else{
                /*用户输入信息了*/ 
                if(FD_ISSET(0, &rfds)){ 
            
                    /******发送消息*******/ 
                    memset(buf,0,sizeof(buf)); 
                    /*fgets函数：从流中读取BUFLEN-1个字符*/ 
                    fgets(buf,BUFLEN,stdin); 
                    /*打印发送的消息*/ 
                    //fputs(buf,stdout); 
                    if(!strncasecmp(buf,"quit",4)){ 
                        printf("[exit] server positively exited\n"); 
                        break; 
                    } 
                    len = send(newfd,buf,strlen(buf),0); 
                    if(len > 0) 
                        printf("[send] MV %s\n",buf); 
                    else{ 
                        printf("[exit] sending went wrong\n"); 
                        break; 
                    } 
                } 
                /*客户端发来了消息*/ 
                if(FD_ISSET(newfd, &rfds)){ 
                    /******接收消息*******/ 
                    memset(buf,0,sizeof(buf)); 
                    /*fgets函数：从流中读取BUFLEN-1个字符*/ 
                    len = recv(newfd,buf,BUFLEN,0); 
                    if(len > 0) 
                        printf("[recv] MV %s\n",buf); 
                    else{ 
                        if(len < 0 ) 
                            printf("[exit] receiving went wrong\n"); 
                        else 
                            printf("[exit] client exited\n"); 
                        break; 
                    } 
                } 
            } 
        }
        /*关闭聊天的套接字*/ 
        close(newfd); 
        /*是否退出服务器*/ 
        printf("[exit] server exiting? [Y/n] ");
        bzero(buf, BUFLEN);
        fgets(buf,BUFLEN, stdin);
        if(!strncasecmp(buf,"y",1)){
            printf("[exit] server exited");
            break;
        }
    }
    /*关闭服务器的套接字*/
    close(sockfd);
}

void client(char **argv)
{
    int sockfd;
    struct sockaddr_in s_addr;
    socklen_t len;
    unsigned int port;
    char buf[BUFLEN];
    fd_set rfds;
    struct timeval tv;
    int retval, maxfd; 
    
    /*建立socket*/
    if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1){
        perror("socket");
        exit(errno);
    }else
        printf("[init] socket create success\n");

    
    /*设置服务器ip*/
    memset(&s_addr,0,sizeof(s_addr));
    s_addr.sin_family = AF_INET;
     s_addr.sin_port = htons(PORT);
    if (inet_aton(argv[1], (struct in_addr *)&s_addr.sin_addr.s_addr) == 0) {
        perror(argv[1]);
        exit(errno);
    }
    /*开始连接服务器*/ 
    if(connect(sockfd,(struct sockaddr*)&s_addr,sizeof(struct sockaddr)) == -1){
        perror("connect");
        exit(errno);
    }else
        printf("[conn] conncet success\n");
    
    while(1){
        FD_ZERO(&rfds);
        FD_SET(0, &rfds);
        maxfd = 0;
        FD_SET(sockfd, &rfds);
        if(maxfd < sockfd)
            maxfd = sockfd;
        tv.tv_sec = 6;
        tv.tv_usec = 0;
        retval = select(maxfd+1, &rfds, NULL, NULL, &tv);
        if(retval == -1){
            printf("[exit] select wrong, client exiting\n");
            break;
        }else if(retval == 0){
            printf("[wait] client waiting...\n");
            continue;
        }else{
            /*服务器发来了消息*/
            if(FD_ISSET(sockfd,&rfds)){
                /******接收消息*******/
                bzero(buf,BUFLEN);
                len = recv(sockfd,buf,BUFLEN,0);
                if(len > 0)
                    printf("[recv] MV %s\n",buf);
                else{
                    if(len < 0 )
                        printf("[exit] receiving went wrong\n");
                    else
                        printf("[exit] server exited\n");
                break; 
                }
            }
            /*用户输入信息了,开始处理信息并发送*/
            if(FD_ISSET(0, &rfds)){ 
                /******发送消息*******/ 
                bzero(buf,BUFLEN);
                fgets(buf,BUFLEN,stdin);
               
                if(!strncasecmp(buf,"quit",4)){
                    printf("[exit] client requested for exiting\n");
                    break;
                }
                    len = send(sockfd,buf,strlen(buf),0);
                if(len > 0)
                    printf("[send] MV %s\n",buf);
                else{
                    printf("[exit] sending went wrong\n");
                    break; 
                } 
            }
        }
    
    }
    /*关闭连接*/
    close(sockfd);
}

int main(int argc, char *argv[])
{
    if (argc == 1) server();
    else if (argc == 2) client(argv);
    return 0;
}