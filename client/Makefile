PROGS=fmsclient
OBJ=fmsclient.o err_handler.o send_request.o recv_response.o set_request.o \
	process_response.o common.o set_request.o fms_completion.o
FLAGS=-Wall -lcrypt -lreadline -lncurses $(DEBUG)
CC=gcc


all: $(PROGS)

debug: all
debug: DEBUG=-g -D__DEBUG__

fmsclient: $(OBJ)
	$(CC) -o $@ $^ $(FLAGS)
%.o: %.c %.h
	$(CC) -c $^ $(FLAGS)

install:
	mkdir /usr/share/fmsclient/
	cp fmsclient /usr/share/fmsclient/
	ln -s /usr/share/fmsclient/fmsclient /bin/

uninstall:
	$(RM) -r /usr/share/fmsclient/
	$(RM) /bin/fmsclient

clean:
	$(RM) $(PROGS)
	$(RM) $(wildcard *.h.gch)
	$(RM) $(wildcard *.o)
