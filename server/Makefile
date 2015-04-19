PROGS=fmspasswd	fmsserver
OBJ=err_handler.o auth.o conf.o process.o recv_request.o send_response.o \
	process_request.o process_cd.o common.o process_mkdir.o process_rm.o \
	process_ls.o process_pwd.o process_download.o process_upload.o
CC=gcc
FLAGS=-Wall -lpthread -g -D__DEBUG__

ALL: $(PROGS)

fmsserver: fmsserver.c err_handler.o auth.o conf.o process_ls.o fmsserver.h \
	recv_request.o send_response.o process_request.o process_cd.o	\
	common.o process_pwd.o process_mkdir.o process_rm.o process_download.o \
	process_upload.o
	$(CC) -o $@ $^ $(FLAGS)
fmspasswd: fmspasswd.c
	$(CC) -o $@ $^ $(FLAGS) -lcrypt 
err_handler.o: err_handler.c err_handler.h
	$(CC) -c $^ $(FLAGS) 
auth.o: auth.c auth.h
	$(CC) -c $^ $(FLAGS)
conf.o: conf.c conf.h
	$(CC) -c $^ $(FLAGS)
process_ls.o: process_ls.c process_ls.h
	$(CC) -c $^ $(FLAGS)
recv_request.o: recv_request.c recv_request.h
	$(CC) -c $^ $(FLAGS)
send_response.o: send_response.c send_response.h
	$(CC) -c $^ $(FLAGS)
process_request.o: process_request.c process_request.h
	$(CC) -c $^ $(FLAGS)
process_cd.o: process_cd.c process_cd.h
	$(CC) -c $^ $(FLAGS)
process_pwd.o: process_pwd.c process_pwd.h
	$(CC) -c $^ $(FLAGS)
process_mkdir.o: process_mkdir.c process_mkdir.h
	$(CC) -c $^ $(FLAGS)
process_rm.o: process_rm.c process_rm.h
	$(CC) -c $^ $(FLAGS)
common.o: common.c common.h
	$(CC) -c $^ $(FLAGS)
process_download.o: process_download.c process_download.h
	$(CC) -c $^ $(FLAGS)
process_upload.o: process_upload.c process_upload.h	
	$(CC) -c $^ $(FLAGS)


install:
	mkdir /usr/share/fmsserver
	cp ./fmspasswd /usr/share/fmsserver/
	cp ./fmsserver /usr/share/fmsserver/
	cp server.conf /usr/share/fmsserver/
	ln -s /usr/share/fmsserver/fmspasswd /bin/
	ln -s /usr/share/fmsserver/fmsserver /bin/

uninstall:
	$(RM) -r /usr/share/fmsserver/
	$(RM) /bin/fmsserver /bin/fmspasswd

clean:
	$(RM) $(OBJ) $(PROGS)
	$(RM) $(wildcard *.h.gch)
