default:
	g++ -Wall Project3Client.cpp Packet.cpp -std=c++11 -o ProjectClient
	echo ///////////////////////////////////////////////////////////////////
	g++ -Wall Project3Server.cpp Packet.cpp -std=c++11 -o ProjectServer

client:
	./ProjectClient -h 141.166.207.147 -p 31201 -t 10 -i 3 -d mathcs04

client41:
	./Project3Client -h 141.166.207.147 -p 31201 -t 10 -i 3 -d mathcs04

client1:
	./ProjectClient -h mathcs01 -p 31201 -t 10 -i 3 -d mathcs04

#	./Project3Client -f Are -l Oelsner -s :141.166.207.147:31201

client41d:
	./Project3Client -f Are -l Oelsner -s mathcs04:31201

client42:
	./Project3Client -f Are -l Oelsner -s :141.166.207.147:31202

clientA:
	./Project3Client -f Are -l Oelsner -s :172.29.170.209:31201

client41n:
	./Project3Client -f Are -l Oelsner -s :mathcs04:31201

client41n1:
	./Project3Client -f Are -l Oelsner -s mathcs04:31201

server:
	./Project3Server -p 31201 -d database.dat

echo:
	./StdoutEchoServer 31201 
	#StdoutEchoServer <port> #replace <>

o:
	vim -p Project3Server.cpp Project3Client.cpp Packet.cpp

gdb:
	gdb Project3Client 

clean:
	rm Project3Client
