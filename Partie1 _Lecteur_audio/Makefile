
#Variable

CFLAGS = -Wall -c -I ./include

#
# Liens
#

bin/lecteur: obj/lecteur.o   obj/audio.o
			   gcc -g -o bin/lecteur obj/lecteur.o  obj/audio.o


#
# Objets
#

obj/lecteur.o: src/lecteur.c 
			gcc $(CFLAGS) src/lecteur.c -o obj/lecteur.o

obj/audio.o: src/audio.c 
				 gcc $(CFLAGS) src/audio.c -o obj/audio.o

#
# supprimer les fichiers
#

clean : 
		rm -rf *.o
		

#
# Tester les chichiers
#

test : obj/lecteur.o
	   padsp ./bin/lecteur $3

#
#compiler tous les fichiers
#
all : 
		make bin/lecteur

