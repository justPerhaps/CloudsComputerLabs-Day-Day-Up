#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/sendfile.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <assert.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <iostream>

#include "httprequestparser.h"
#include "request.h"
#include "ThreadPool.h"
using namespace httpparser;

int server_port;
int thread_num;
char *ip_address;

void POST_method(std::string Name, std::string ID, int connfd,std::string str_text)
{
    std::string str_send = "";
    char OK[]="HTTP/1.1 200 OK\r\n";
    str_send += OK;
    //char server[]="Tiny Web Server\r\n";
    //str_send += server;
    char Context_type[]="Content-Type: application/x-www-form-urlencoded\r\n";
    str_send += Context_type;
    char Context_len[]="Context_length: 13\r\n\n";
    str_send += Context_len;
    //具体有多长的计算……

    //char html_title[]="<html><title>POST Method</title><body bgcolor=ffffff>\n";
    //str_send += html_title;
    char chID[]="id=";
    str_send += chID;
    str_send += ID;
    char chName[]="&name=";
    str_send += chName;
    str_send += Name;
    
    //char web[]="<hr><em>Http Web Server</em>\n</body></html>\n";
    //str_send += web;
	
    char send_buf[1024];
    sprintf(send_buf,"%s",str_send.c_str());
    write(connfd, send_buf, strlen(send_buf));

    // close(fd);
    close(connfd);
}

void NOTFOUND_method(std::string method,std::string url,int fd){

    std::string entity="<!DOCTYPE html>\n<html lang=\"en\">\n<head>\n    <meta charset=\"UTF-8\">\n    <link rel=\"stylesheet\" href=\"/styles.css\"/>\n    <title>404 Not Found</title>\n</head>\n<body>\n    <h1>\n        404 Not Found\n    </h1>\n</body>\n</html>";

    std::string str="HTTP/1.1 404 Not Found\r\nContent-Type: text/html\r\nContent-Length: "+std::to_string(entity.length())+"\r\n\r\n";
    std::string total=str+entity;
    char buf[512];
    sprintf(buf,"%s",total.c_str());
    write(fd, buf, strlen(buf));

}

//获取文件类型
char *http_get_mime_type(char *file_name) {
  char *file_extension = strrchr(file_name, '.');
  if (file_extension == NULL) {
    return "text/plain";
  }

  if (strcmp(file_extension, ".html") == 0 || strcmp(file_extension, ".htm") == 0) {
    return "text/html";
  } else if (strcmp(file_extension, ".jpg") == 0 || strcmp(file_extension, ".jpeg") == 0) {
    return "image/jpeg";
  } else if (strcmp(file_extension, ".png") == 0) {
    return "image/png";
  } else if (strcmp(file_extension, ".css") == 0) {
    return "text/css";
  } else if (strcmp(file_extension, ".js") == 0) {
    return "application/javascript";
  } else if (strcmp(file_extension, ".pdf") == 0) {
    return "application/pdf";
  } else {
    return "text/plain";
  }
}

void GET_method(std::string method,int connfd,std::string uri)
{
    char *curi = new char[uri.length() + 1];
    strcpy(curi, uri.c_str());
    curi++; //删除"/"
    int fd;

	if(uri=="/api/check")
	{
		curi="data.txt"; //默认消息体
		fd = open(curi,O_RDONLY);
		if(fd==-1) //找不到文件
		{
			NOTFOUND_method(method,uri,connfd);
		}

		
		
		//获取文件大小
		struct stat file;
		stat(curi,&file);
		int length=file.st_size;
		std::string slength = std::to_string(length);

		//构造请求
		char code[]="HTTP/1.1 200 OK\r\n";
		//char server[]="Server: Tiny Web Server\r\n";
		char type_head[]="\r\nContent-type:";
		char *type=http_get_mime_type(curi);
		char length_head[]="Content-length:";
		char const *clength = slength.c_str(); 
		char end[]="\r\n\r\n";

		//发送响应
		send(connfd,code,strlen(code),0);
		//send(connfd,server,strlen(server),0);
		send(connfd,length_head,strlen(length_head),0);
		send(connfd,clength,strlen(clength),0);
		send(connfd,type_head,strlen(type_head),0);
		send(connfd,type,strlen(type),0);
		send(connfd,end,strlen(end),0);

		sendfile(connfd,fd,NULL,2500);
		send(connfd,end,strlen(end),0);

		close(fd);
		close(connfd);
	}
	else
	{
		if(uri=="/") curi="index.html"; //默认消息体
		fd = open(curi,O_RDONLY);
		if(fd==-1) //找不到文件
		{
			NOTFOUND_method(method,uri,connfd);
		}

		//printf("send=%d\n",s);
		
		//获取文件大小
		struct stat file;
		stat(curi,&file);
		int length=file.st_size;
		std::string slength = std::to_string(length);

		//构造请求
		char code[]="HTTP/1.1 200 OK\r\n";
		//char server[]="Server: Tiny Web Server\r\n";
		char length_head[]="Content-length:";
		char const *clength = slength.c_str(); 
		char type_head[]="\r\nContent-type:";
		char *type=http_get_mime_type(curi);
		char end[]="\r\n\r\n";
		
		//发送响应
		send(connfd,code,strlen(code),0);
		//send(connfd,server,strlen(server),0);
		send(connfd,length_head,strlen(length_head),0);
		send(connfd,clength,strlen(clength),0);
		send(connfd,type_head,strlen(type_head),0);
		send(connfd,type,strlen(type),0);
		send(connfd,end,strlen(end),0);

		sendfile(connfd,fd,NULL,2500);


		close(fd);
		close(connfd);
	}
}

void NOT_Implemented(std::string method,int fd)
{
    std::string entity1="<html><title>501 Not Implemented</title><body bgcolor=ffffff>\n Not Implemented\n";
    std::string entity2="<p>Does not implement this method: "+method+"\n<hr><em>HTTP Web Server</em>\n</body></html>\n";
    std::string entity=entity1+entity2;
    std::string str="HTTP/1.1 501 Not Implemented\r\nContent-Type: text/html\r\nContent-Length: "+std::to_string(entity.length())+"\r\n\r\n";
    std::string total=str+entity;
    char buf[512];
    sprintf(buf,"%s",total.c_str());
    write(fd, buf, strlen(buf));
}

void handle_request(int connfd)
{
    char text[1024];
    recv(connfd,text,1024,0);
    text[strlen(text)+1]='\0';
    
    std::string str_text = "";
    str_text += text;

    Request request;
    HttpRequestParser parser;
    int length=strlen(text);

    HttpRequestParser::ParseResult res = parser.parse(request, text, text + length);

    if( res == HttpRequestParser::ParsingCompleted )
    {
        //std::cout<<request.method<<std::endl;
        //std::cout<<request.uri<<std::endl;

        std::string method=request.method;
        std::string uri=request.uri;
        if(method=="GET")
        {
            GET_method(method,connfd,uri);
        }
        
        else if(method!="GET"&&method!="POST")
        {
            NOT_Implemented(method,connfd);
        }
        
	else if(method=="POST")
	{
		//printf("%s", uri);
		//printf("Now we handle the POST method...\n");
		if( uri == "/api/echo")
		{
        		//'id=1&name=Foo'
        		//printf("uri = /api/echo\n");
        		int position1 = str_text.find("&name=");
        		int position2 = str_text.find("id=");
        		//printf("position1=%d, position2=%d\n",position1,position2);
        		if( position1==-1 || position2==-1 || position1<=position2)
        		{
          			//printf("not found method\n");
          			NOTFOUND_method(method,uri,connfd);
          			//printf("after found method\n");
        		}
        		else
        		{
          			//GET_method(method,connfd,uri);
				//std::cout << "db db db: /Post_show\n";
				//printf("use Post method\n");
				std::string Name,ID;
				Name=str_text.substr(position1+6);
				ID=str_text.substr(position2+3,position1-position2-3);
				//printf("name is %s, id is %s\n",Name,ID);
				POST_method(Name, ID, connfd, str_text);
        		}
        		//printf("Have used the POST_method()\n");

		}
		if(uri != "/api/echo")
		{
			//printf("the uri is not /api/echo\n");
			NOTFOUND_method(method,uri,connfd);
		}
		else
		{
			//printf("use not implemented\n");
			NOT_Implemented(method,connfd);
			//printf("after use not implemented\n");
		}
	}
    }
}

void TCP_connect(int thread_num,char *ip_address)
{
    ThreadPool pool(thread_num);

    int socketfd;
    int connfd;
    struct sockaddr_in server_address;

    socketfd = socket(AF_INET,SOCK_STREAM,0);   //建立TCP socket
    if (socketfd == -1) {
    perror("Failed to create a new socket");
    exit(errno);
    }
	
	int reuse = 1;
    if(setsockopt(socketfd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) < 0)
    {
      perror("setsockopt error\n");
      return ;
    }

    bzero(&server_address,sizeof(server_address));    //清零
    server_address.sin_family = AF_INET; 
    server_address.sin_addr.s_addr = inet_addr(ip_address);  //sin_addr存储IP地址，使用in_addr这个数据结构；s_addr按照网络字节顺序存储IP地址
    server_address.sin_port = htons(server_port);   //端口号

    if (bind(socketfd, (struct sockaddr *) &server_address,
        sizeof(server_address)) == -1) {
        perror("Failed to bind on socket");
        exit(errno);
    }

    if (listen(socketfd, 1024) == -1) 
    {
        perror("Failed to listen on socket");
        exit(errno);
    }

    printf("Listening on port %d...\n",server_port);

    while(true)
    {
        //等待连接
        struct sockaddr_in client;
        socklen_t client_addrlength = sizeof(client);
        connfd = accept(socketfd, (struct sockaddr*)&client, &client_addrlength);   //返回连接套接字描述符
        if(connfd<0)    //该描述符不为0
        {
            perror("err connfd\n");
        }
        else
        {
          pool.enqueue(handle_request,connfd);

        }
    }
	close(socketfd);

}

int main(int argc,char **argv)
{
    //默认参数s
    server_port=8888;
    thread_num=2;
    ip_address="127.0.0.1";

    if(argc < 5)
    {
        printf("./httpServer --ip ip_address --port port_number [--number-thread thread_number] \n");
        exit(1);
    }

    for(int i=1;i<argc;i++)
    {
        if(strcmp("--port",argv[i])==0)  // --port
        {
            char *server_port_str=argv[++i];
            if(!server_port_str){
                fprintf(stderr, "Expected argument after --port\n");
            }
            server_port=atoi(server_port_str);
        }
        else if (strcmp("--number-thread", argv[i]) == 0) 
        {
          char *cthread_num = argv[++i];
          if (!cthread_num) {
              fprintf(stderr, "Expected argument after --proxy\n");
          }
          thread_num=atoi(cthread_num);
        }
        else if (strcmp("--ip", argv[i]) == 0) 
        {
          char *ip = argv[++i];
          if (!ip) {
              fprintf(stderr, "Expected argument after --ip\n");
          }
          ip_address=ip;
        }
    }

    TCP_connect(thread_num,ip_address);

    return 0;
}
