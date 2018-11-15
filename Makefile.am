bin_PROGRAMS = server
server_SOURCES = src/main.c src/hashmap.c

#agent_client_SOURCES = src/agent_client.c

#miio_agent_LDADD =  -lm
server_CFLAGS = -I./inc -s

#agent_client_CFLAGS = -s

distclean-local:
	rm -rf Makefile.in compile autom4te.cache aclocal.m4 config.h.in configure depcomp install-sh missing
