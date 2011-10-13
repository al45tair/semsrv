CFLAGS=-g -W -Wall
CXXFLAGS=$(CFLAGS)
MIGFILES=semsrvUser.c semsrvServer.c semsrv.h
SRVFILES=server.cc
CLIENTFILES=client.c
OBJS=$(MIGFILES:.c=.o) $(SRVFILES:.cc=.o) $(CLIENTFILES:.c=.o)
SRVOBJS=semsrvServer.o server.o
CLIENTOBJS=client.o semsrvUser.o
MIG=mig

all:		semsrv semtest

clean:
		$(RM) semsrv semtest $(MIGFILES) $(OBJS)

semsrv:		$(SRVOBJS)
		$(CXX) $(CXXFLAGS) $(LDFLAGS) $^ $(LDLIBS) -o $@

semtest:	$(CLIENTOBJS)
		$(CC) $(CXXFLAGS) $(LDFLAGS) $^ $(LDLIBS) -o $@

$(MIGFILES):	semsrv.defs
		$(MIG) $^
