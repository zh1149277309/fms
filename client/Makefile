PROGS=fmsclient
OBJ=err_handler.o send_request.o recv_response.o set_request.op \
	process_response.o common.o	set_request.o
FLAGS=-Wall -lpthread -lcrypt -D__DEBUG__
CC=gcc

ALL: $(PROGS)

fmsclient: fmsclient.c debug.h fmsclient.h err_handler.o send_request.o \
	recv_response.o set_request.o process_response.o common.o
	$(CC) -o $@ $^ $(FLAGS)
err_handler.o: err_handler.c err_handler.h
	$(CC) -c $^ $(FLAGS) 
send_request.o: send_request.c send_request.h
	$(CC) -c $^ $(FLAGS) 
recv_response.o: recv_response.c recv_response.h
	$(CC) -c $^ $(FLAGS)
set_request.o: set_request.c set_request.h
	$(CC) -c $^ $(FLAGS)
process_response.o: process_response.c process_response.h
	$(CC) -c $^ $(FLAGS)
common.o: common.c common.h
	$(CC) -c $^ $(FLAGS)

install:
	mkdir /usr/share/fmsclient/
	cp ./fmsclient /usr/share/fmsclient/
	ln -s /usr/share/fmsclient/fmsclient /bin/

uninstall:
	$(RM) -r /usr/share/fmsclient/
	$(RM) /bin/fmsclient

clean:
	$(RM) $(OBJ) $(PROGS)
	$(RM) $(wildcard *.h.gch)
