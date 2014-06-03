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

using namespace std;

void* readS(void* sock);

mysqlpp::Connection con( false );

bool mysqlInsert(float temp, int hum, float pres)
{
  cout<<"Entered mysql function"<<endl;
  if (con.connect( "weather_measurements", "localhost", "muxa", "852456" ))
    {
      // выполнение запроса и вывод полученных результатов
      mysqlpp::Query query = con.query();
      query<<"insert into measurements values("<<temp<<","<<hum<<","<<pres<<",now())";
      cout<<query<<endl;
      cout<<query.execute().info();
      con.disconnect();
    }
  else
    {
      cerr << "Error connect" << endl;
      return 1;
    }
  
     
}


int main(int argc, char** argv)
{
  //mysqlInsert(1,1,1);
  //cout<<sizeof(int);

  int sock = socket(AF_INET, SOCK_STREAM, 0);
  if (sock < 0)
  {
    cout<<"Error creating socket: "<<errno<<endl;
    return(EXIT_FAILURE);
  }
  cout<<"Socket created: "<<sock<<endl;

  struct sockaddr_in address;
  address.sin_family = AF_INET;
  address.sin_port = htons(4444);
  address.sin_addr.s_addr = htonl(INADDR_ANY);

  if (bind(sock, (struct sockaddr*)&address, sizeof(address)) < 0)
    {
      cout<<"Error binding socket: "<<errno<<endl;
      return(EXIT_FAILURE);
    }

  if (listen(sock, 1) < 0)
    {
      cout<<"Can't listen socket: "<<errno<<endl;
      return(EXIT_FAILURE);
    }

  int client;

  while(true)
    {
      client = accept(sock, NULL, NULL);
    
      if (client < 0)
	{
	  cout<<"Cannot accept client: "<<errno;
	  return(EXIT_FAILURE);
	}
      
      pthread_t thr;
      pthread_create(&thr, NULL, readS, &client);

    }
 

  close(sock);
  cout<<"Socket closed"<<endl;
  return(EXIT_SUCCESS);
}

char buf[128];
string s;
float temp = 0;
int pres = 0;
float humi = 0;

void* readS(void* sock)
{

    
  cout<<"accepted socket "<<*(int*)sock<<endl;

  int answer;
  while ((answer = recv(*(int*)sock, buf, sizeof(buf), 0)) > 0) 
    {
      
      cout<<"received "<<answer<<" bytes"<<endl;
      printf("received: %s\n", buf);

      //*** NOT NECESSARY - REMOVE

      //Remove EOL symbol
      //buf[sizeof(buf)-1] = 0;

      string s(buf);
      //Remove \n
      //s.pop_back();

      //***

      //Clearing bur fot next receives
      memset(&buf[0], 0, sizeof(buf));
      
      
      //istringstream iss(s);
      //string temp;

      //vector<string> v;
      //while (iss >> temp) v.push_back(temp);
      //vector<string> v = split(s, ' ');

      //for (const string &item: v)
      //{
      //  cout<<item<<endl;
      //}
      /*
      while (!iss.eof())
      	{
      	  string sub;
      	  //stringstream ss;

      	  iss >> sub;
      	  //int temp;
      	  //ss << sub;
      	  //ss >> temp;
      	  cout << "Substring: " << sub << endl;
      	}
      */
      
      //string s(buf);
      //cout<<s;
      //mysqlInsert((int)(buf[0]-'0'),(int)(buf[1]-'0'),(int)(buf[2]-'0'));
    //printf("received: ", s);
    //cout<<s;
      //buf[0] = '\0';
      
      cout<<"String is "<<s<<endl;
      /*
      cout<<s.substr(2, 5);
      cout<<s.substr(15, 5);
      */
      temp = stof(s.substr(2, 5));
      pres = stoi(s.substr(10, 3));
      humi = stof(s.substr(16, 5));
      cout<<"Temperature is "<<temp<<endl;
      cout<<"Pressure is "<<pres<<endl;
      cout<<"Humidity is "<<humi<<endl;
      mysqlInsert(temp,pres,humi);
    }

  
    
  cout<<"exit thread"<<endl;
  close(*(int*)sock);
  pthread_exit(NULL);

}
