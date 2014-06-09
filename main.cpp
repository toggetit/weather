#include <iostream>
#include <cstdlib>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <errno.h>
#include <string>
#include <pthread.h>
#include <cstdio>
#include <list>
#include <mysql++/mysql++.h>
#include <syslog.h>

using namespace std;

void* readS(void* sock);

mysqlpp::Connection con( false );

bool mysqlInsert(float temp, float hum, int pres)
{
  //cout<<"Entered mysql function"<<endl;
  syslog(LOG_DEBUG, "Entering mysql inserting subroutine");
  if (con.connect( "weather_measurements", "localhost", "muxa", "852456" ))
    {
      // выполнение запроса и вывод полученных результатов
      mysqlpp::Query query = con.query();
      query<<"insert into measurements values("<<temp<<","<<hum<<","<<pres<<",now())";
      syslog(LOG_DEBUG, "Executing SQL query: %s", query.str().c_str());
      //cout<<query<<endl;
      //cout<<query.execute().info();
      query.execute();
      syslog(LOG_DEBUG, "Base answer: %s", query.error());
      con.disconnect();
    }
  else
    {
      syslog(LOG_ERR, "Couldn't connect to database: %s, exiting subroutine", con.error());
      //cerr << "Error connect" << endl;
      return 1;
    }
  
     
}


int main(int argc, char** argv)
{

  //buffer for log (error string)
  char err[256];

  openlog("weather", LOG_PID, LOG_USER);
  syslog(LOG_INFO, "*** Started program ***");

  int sock = socket(AF_INET, SOCK_STREAM, 0);
  if (sock < 0)
  {
    
    syslog(LOG_ERR, "Error creating socket: %s", strerror_r(errno, err, 256));
    //cout<<"Error creating socket: "<<strerror(errno)<<endl;
    return(EXIT_FAILURE);
  }
  syslog(LOG_INFO, "Socket created: %d", sock);

  struct sockaddr_in address;
  address.sin_family = AF_INET;
  address.sin_port = htons(4444);
  address.sin_addr.s_addr = htonl(INADDR_ANY);

  if (bind(sock, (struct sockaddr*)&address, sizeof(address)) < 0)
    {
      syslog(LOG_ERR, "Error binding socket: %s", strerror_r(errno, err, 256));
      //cout<<"Error binding socket: "<<strerror(errno)<<endl;
      return(EXIT_FAILURE);
    }

  if (listen(sock, 1) < 0)
    {
      syslog(LOG_ERR, "Error listen socket: %s", strerror_r(errno, err, 256));
      //cout<<"Can't listen socket: "<<errno<<endl;
      return(EXIT_FAILURE);
    }

  int client;

  while(true)
    {
      client = accept(sock, NULL, NULL);
    
      if (client < 0)
	{
	  syslog(LOG_ERR, "Cannot accept client: %s", strerror_r(errno, err, 256));
	  //cout<<"Cannot accept client: "<<errno;
	  return(EXIT_FAILURE);
	}
      
      pthread_t thr;
      pthread_create(&thr, NULL, readS, &client);

    }
 

  close(sock);
  syslog(LOG_INFO, "Socket closed");
  //cout<<"Socket closed"<<endl;
  closelog();
  return(EXIT_SUCCESS);
}




void* readS(void* sock)
{
  char buf[128];
  char err[256];
  string s;
  float temp = 0;
  int pres = 0;
  float humi = 0;
    
  syslog(LOG_INFO, "Accepted socket: %d", *(int*)sock);
  //cout<<"accepted socket "<<*(int*)sock<<endl;

  int answer;
  while ((answer = recv(*(int*)sock, buf, sizeof(buf), 0)) > 0) 
    {
      syslog(LOG_DEBUG, "Received %d bytes: %s", answer, buf);
      //cout<<"received "<<answer<<" bytes"<<endl;
      //printf("received: %s\n", buf);
      //syslog(LOG_DEBUG, "Recieved: %s", buf);

      s.assign(buf, sizeof(buf));
      //Clearing bur fot next receives
      memset(&buf[0], 0, sizeof(buf));
      
            
      syslog(LOG_DEBUG, "Converted to string: %s", s.c_str());
      //cout<<"String is "<<s<<endl;

      temp = stof(s.substr(2, 5));
      pres = stoi(s.substr(10, 3));
      humi = stof(s.substr(16, 5));
      //cout<<"Temperature is "<<temp<<endl;
      //cout<<"Pressure is "<<pres<<endl;
      //cout<<"Humidity is "<<humi<<endl;
      syslog(LOG_DEBUG, "Extracted TEMPERATURE: %.2f", temp);
      syslog(LOG_DEBUG, "Extracted HUMIDITY: %.2f", humi);
      syslog(LOG_DEBUG, "Extracted PRESSURE: %d", pres);
      
      //mysqlInsert(temp,pres,humi);
      mysqlInsert(stof(s.substr(2, 5)), stof(s.substr(16, 5)), stoi(s.substr(10, 3)));
      s.clear();
    }

  
    
  
  //cout<<"exit thread"<<endl;
  close(*(int*)sock);
  syslog(LOG_INFO, "Closed socket %d, exiting thread", *(int*)sock);

  pthread_exit(NULL);

}
