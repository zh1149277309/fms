PROGS=fmspasswd	fmsserver
OBJ=fmsserver.o err_handler.o auth.o conf.o recv_request.o send_response.o \
	process_request.o process_cd.o common.o process_mkdir.o process_rm.o \
	process_ls.o process_pwd.o process_download.o process_upload.o
CC=gcc
FLAGS=-Wall -lpthread $(DEBUG)

ALL: $(PROGS)

fmsserver: $(OBJ)
	$(CC) -o $@ $^ $(FLAGS)
fmspasswd: fmspasswd.c
	$(CC) -o $@ $^ $(FLAGS) -lcrypt
%.o: %.c %.h
	$(CC) -c $^ $(FLAGS)

install:
	mkdir /usr/share/fmsserver
	cp fmspasswd /usr/share/fmsserver/
	cp fmsserver /usr/share/fmsserver/
	cp server.conf /usr/share/fmsserver/
	cp passwd /usr/share/fmsserver/
	ln -s /usr/share/fmsserver/fmspasswd /bin/
	ln -s /usr/share/fmsserver/fmsserver /bin/

uninstall:
	$(RM) -r /usr/share/fmsserver/
	$(RM) /bin/fmsserver
	$(RM) /bin/fmspasswd

clean:
	$(RM) $(PROGS)
	$(RM) $(wildcard *.h.gch)
	$(RM) $(wildcard *.o)
