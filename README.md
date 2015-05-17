# File Management System

# Description
	The File Management System abbreviated as "FMS"; The FMS has a server-
	side and a client-side, for provides upload and download service on the
	command line.

# Functionality
	1. FMS provides a simple authentication mechanism, which same as linux.
	2. FMS provides the following command:
		ls    cd    pwd    mkdir   rm    upload    download    exit
	3.configure file support options:
		port	passfile    rootdir
		[default]
			port=40325
			passfile=passwd	(relative to current working directory)
			rootdir=/var/fms
# Installation
	[Server]	
				$ make
				$ cp fmspasswd	 /path/to/fms
				$ cp fmsserver   /path/to/fms
				$ cp server.conf /path/to/fms
	[Client]
				$ make
				$ cp fmsclient   /bin/
# Feture functionality:
	1. Add the echo server to interactive command line;


