#include "synchro.h"
#include "ensitheora.h"


extern bool fini;

/* les variables pour la synchro, ici */

pthread_mutex_t mutex_taille;
pthread_cond_t cond_taille;
bool est_taille_pret;

pthread_mutex_t mutex_texture;
pthread_cond_t cond_texture;
bool est_texture_pret;

pthread_mutex_t mutex_fenetre;
pthread_cond_t cond_vide;
pthread_cond_t cond_plein;
int nb_textures;

/* l'implantation des fonctions de synchro ici */

// coté décodeur
void envoiTailleFenetre(th_ycbcr_buffer buffer) {

    pthread_mutex_lock(&mutex_taille);

    // Afin d’accéder aux informations de taille de la fenêtre à partir d’une
    // variable buffer de type th_ycbcr_buffer, vous utiliserez buffer[0].width
    // ainsi que buffer[0].height.
    windowsx = buffer[0].width;
    windowsy = buffer[0].height;

    est_taille_pret = true;

    pthread_cond_signal(&cond_taille);

    pthread_mutex_unlock(&mutex_taille);
}

// coté afficheur
void attendreTailleFenetre() {

    pthread_mutex_lock(&mutex_taille);

    while (!est_taille_pret) {
        pthread_cond_wait(&cond_taille, &mutex_taille);
    }
    est_taille_pret = false;

    pthread_mutex_unlock(&mutex_taille);
}

// coté afficheur
void signalerFenetreEtTexturePrete() {

    pthread_mutex_lock(&mutex_texture);

    est_texture_pret = true;
    pthread_cond_signal(&cond_texture);
    pthread_mutex_unlock(&mutex_texture);
}

// coté décodeur
void attendreFenetreTexture() {

    pthread_mutex_lock(&mutex_texture);
    while (!est_texture_pret) {
        pthread_cond_wait(&cond_texture, &mutex_texture);
    }
    est_texture_pret = false;
    pthread_mutex_unlock(&mutex_texture);
}

void debutConsommerTexture() {
    pthread_mutex_lock(&mutex_fenetre);
    while(nb_textures == 0){
        pthread_cond_wait(&cond_vide, &mutex_fenetre);
    }
    pthread_mutex_unlock(&mutex_fenetre);
}

void finConsommerTexture() {
    pthread_mutex_lock(&mutex_fenetre);
    nb_textures--;
    pthread_cond_signal(&cond_plein);
    signalerFenetreEtTexturePrete();
    pthread_mutex_unlock(&mutex_fenetre);
}


void debutDeposerTexture() {
    pthread_mutex_lock(&mutex_fenetre);
    while(nb_textures == NBTEX){
        pthread_cond_wait(&cond_plein, &mutex_fenetre);
    }
    pthread_mutex_unlock(&mutex_fenetre);
}

void finDeposerTexture() {
    pthread_mutex_lock(&mutex_fenetre);
    nb_textures++;
    pthread_cond_signal(&cond_vide);
    pthread_mutex_unlock(&mutex_fenetre);
}
