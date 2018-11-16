bin_PROGRAMS = server ipc_client

server_SOURCES = src/main.c src/hashmap.c
server_LDADD =  -lev
server_CFLAGS = -I./inc -s


ipc_client_SOURCES = src/ipc_client.c
ipc_client_LDADD =  -lev
ipc_client_CFLAGS = -I./inc -s

distclean-local:
	rm -rf Makefile.in compile autom4te.cache aclocal.m4 config.h.in configure depcomp install-sh missing
