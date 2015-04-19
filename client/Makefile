PROGS=fmsclient
OBJ=err_handler.o send_request.o recv_response.o set_request.op \
	process_response.o common.o	set_request.o
FLAGS=-Wall -lcrypt -D__DEBUG__
CC=gcc

ALL: $(PROGS)

fmsclient: fmsclient.c debug.h fmsclient.h err_handler.o send_request.o \
	recv_response.o set_request.o process_response.o common.o
	$(CC) -o $@ $^ $(FLAGS)
%.o: %.c
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
