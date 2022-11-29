#include <stdio.h>
#include <unistd.h>
#include <assert.h>
#include <SDL2/SDL.h>

#include "stream_common.h"
#include "oggstream.h"

# include <pthread.h>

int main(int argc, char *argv[]) {
    int res;

    if (argc != 2) {
	fprintf(stderr, "Usage: %s FILE", argv[0]);
	exit(EXIT_FAILURE);
    }
    assert(argc == 2);


    // Initialisation de la SDL
    res = SDL_Init(SDL_INIT_VIDEO|SDL_INIT_AUDIO|SDL_INIT_EVENTS);
    atexit(SDL_Quit);
    assert(res == 0);

    // Dans la fonction int main(int argc, char *argv[]) du fichier
    // ensivideo.c, ajouter le lancement de deux threads qui exécutent, chacun,
    // une des fonctions theoraStreamReader et vorbisStreamReader. Chacun
    // reçoit en argument le nom du fichier à lire (argv[1]).

    pthread_t thread_theora, thread_vorbis;

    // start the two stream readers

    pthread_create(&thread_theora, NULL, theoraStreamReader, (void *)argv[1]);
    pthread_create(&thread_vorbis, NULL, vorbisStreamReader, (void *)argv[1]);

    // Il faudra ensuite, dans la fonction main, attendre la terminaison du thread
    // vorbis
    void* status;
    void* ALL_IS_OK = (void*)123456789L;

    // wait audio thread

    pthread_join(thread_vorbis, &status);
    if(status != ALL_IS_OK){
        printf("Thread % lx not ok \n", thread_vorbis); 
    }

    // 1 seconde de garde pour le son,
    sleep(1);

    // tuer les deux threads videos si ils sont bloqués
    pthread_cancel(thread_theora);
    pthread_cancel(thread_vorbis);

    // attendre les 2 threads videos
    pthread_join(thread_theora,NULL);
    pthread_join(thread_vorbis,NULL);

    
    exit(EXIT_SUCCESS);    
}
