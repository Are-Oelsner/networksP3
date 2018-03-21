default:
	gcc -Wall Project3Client.cpp -std=c++11 -o Project3Client
	gcc -Wall Project3Server.cpp -std=c++11 -o Project3Server

client:
	./Project3Client -f Are -l Oelsner

client41:
	./Project3Client -f Are -l Oelsner -s :141.166.207.147:31201

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
	./Project3Server -s MYCOOKIE -p 31201
	#Project3Server -s <cookie> -p <port> #replace <>

echo:
	./StdoutEchoServer 31201 
	#StdoutEchoServer <port> #replace <>

o:
	vim -p Project3Server.cpp Project3Client.cpp

gdb:
	gdb Project3Client 

clean:
	rm Project3Client
