
#Variable

CFLAGS = -Wall -c -I ./include
BINDIR = bin
SRCDIR = src
OBJDIR = obj


#
# Liens
#

$(BINDIR)/audioserver: $(OBJDIR)/audioserver.o   $(OBJDIR)/audioserver.o
			   @gcc -g -o $(BINDIR)/audioserver $(OBJDIR)/audioserver.o  $(OBJDIR)/audio.o

$(BINDIR)/audioclient: $(OBJDIR)/audioclient.o   $(OBJDIR)/audio.o
			   @gcc -g -o $(BINDIR)/audioclient $(OBJDIR)/audioclient.o  $(OBJDIR)/audio.o


#
# Objets
#

$(OBJDIR)/audioserver.o: $(SRCDIR)/audioserver.c 
			@gcc $(CFLAGS) $(SRCDIR)/audioserver.c -o $(OBJDIR)/audioserver.o


$(OBJDIR)/audioclient.o: $(SRCDIR)/audioclient.c 
			@gcc $(CFLAGS) $(SRCDIR)/audioclient.c -o $(OBJDIR)/audioclient.o


$(OBJDIR)/audio.o: $(SRCDIR)/audio.c 
				 @gcc $(CFLAGS) $(SRCDIR)/audio.c -o $(OBJDIR)/audio.o

#
# supprimer les fichiers
#

clean : 
		rm -rf *.o
		

#
# Tester les chichiers
#

test_client : $(OBJDIR)/audioclient.o
	  @ padsp ./$(BINDIR)/audioclient 

test_server : $(OBJDIR)/audioserver.o
	  @  ./$(BINDIR)/audioserver 

#
#compiler tous les fichiers
#
all : 
		 make $(BINDIR)/audioclient  $(BINDIR)/audioserver

