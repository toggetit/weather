all: main.o
	g++ -lpthread -lmysqlpp build/main.o -o build/weather
main.o: main.cpp
	g++ -std=c++11 -I/usr/include/mysql -c main.cpp -o build/main.o
clean:
	rm build/*.o

#Для цели инсталляции сделать:
#1. Настройки отдельного логирования syslog:
#	cp syslog-weather.conf /etc/rsyslog.d/
#	service rsyslog restart
