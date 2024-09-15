/**
 * @file function.c
 * @brief Contient les fonctions pour la gestion des fichiers dans une partition simulée.
 * @date 2024-03-29
 * @author 
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "function.h"

/** @var totalBlocks
 *  @brief Nombre total de blocs dans la partition.
 */
int totalBlocks = PARTITION_SIZE / BLOCK_SIZE;


/**
 * @brief Calcule la taille d'un fichier vide en octets pour mettre les métadonnées
 * @return La taille d'un fichier vide.
 */
int sizeOfEmptyFile(){
    file f;
    return sizeof(f.fileName) + 5 * sizeof(int);
}


/**
 * @brief Crée ou formate une partition.
 * @param partitionName Le nom de la partition.
 * @return 0 si réussi, -1 en cas d'erreur.
 */
int myFormat(char* partitionName) {
    int partition = open(partitionName, O_RDWR | O_CREAT, 0666);
    if (partition == -1) {
        return -1;
    }
    file emptyFile = {0};
    for (int i = 0; i < totalBlocks; i++) {
        write(partition, &emptyFile, sizeof(file));
    }
    emptyFile.isUsed = 0;
    for (int i = totalBlocks * sizeof(file); i < PARTITION_SIZE; i += BLOCK_SIZE) {
        write(partition, &emptyFile, sizeof(file));
    }
    close(partition);
    return 0;
}


/**
 * @brief Ouvre un fichier sur la partition.
 * @param fileName Le nom du fichier à ouvrir.
 * @return Un pointeur vers la structure de fichier ou NULL en cas d'erreur.
 */
file *myOpen(char* fileName) {
    char* partitionName = PARTITION_NAME;
    int partition = open(partitionName, O_RDWR);
    if (partition == -1) {
        return NULL;
    }
    file f;
    int found = 0;
    // Recherche de fichier et ouverture en cas de trouvaille
    for (int i = 0; i < totalBlocks - 1; i++) {
        lseek(partition, i * BLOCK_SIZE, SEEK_SET);
        read(partition, &f, sizeof(file));
        if (f.isUsed && strcmp(f.fileName, fileName) == 0) {
            found = 1;
            f.isOpen = 1;
            lseek(partition, -sizeof(file), SEEK_CUR);
            write(partition, &f, sizeof(file));
            break;
        }
    }

    //Recherche de block disponible et creation du fichier en cas d'absence
    if (!found) {
        lseek(partition, 0, SEEK_SET);
        for (int i = 0; i < totalBlocks; i++) {
            read(partition, &f, sizeof(file));
            if (!f.isUsed) {
                strncpy(f.fileName, fileName, sizeof(f.fileName) - 1);
                f.fileSize = sizeOfEmptyFile();
                f.startBlock = i * BLOCK_SIZE;                
                f.isOpen = 1;
                f.isUsed = 1;
                f.cursor = f.startBlock + sizeOfEmptyFile();
                lseek(partition, -sizeof(file), SEEK_CUR);
                write(partition, &f, sizeof(file));
                break;
            }
        }
    }

    close(partition);

    // Allouer de l'espace pour le fichier
    file* openedFile = (file*)malloc(sizeof(file));
    if (openedFile != NULL) {
        *openedFile = f; 
    }
    return openedFile;
}


/**
 * @brief Affiche la quantité d'espace utilisé dans la partition.
 * @param partitionName Le nom de la partition.
 */
void visualisation(char* partitionName){
    // Ouverture de la partition
    int partition = open(partitionName, O_RDWR);
    int counter = 0;
    if (partition == -1) {
        perror("Erreur lors de l'ouverture de la partition\n");
    }
    
    // Detection des blocs sur les quels un travail a déjà été fait
    file f;
    int found = 0;
    for (int i = 0; i < totalBlocks; i++) {
        lseek(partition, i * BLOCK_SIZE, SEEK_SET);
        read(partition, &f, sizeof(file));
        if (f.isUsed) {
            counter ++;
        }
    }

    // Un bloc a pour taille BLOCK_SIZE donc sur 32 Mo on aura utlisé BLOCK_SIZE * nbBlocs utilés
    printf("Espace utilisé : %d o / 32 Mo ; %d Blocs utilisés sur %d disponibles\n", counter * BLOCK_SIZE, counter, totalBlocks);

}


/**
 * @brief Déplace le curseur de lecture/écriture dans le fichier.
 * @param f Le pointeur vers la structure de fichier.
 * @param offset Le décalage à appliquer.
 * @param base La base à partir de laquelle le décalage est calculé (SEEK_SET, SEEK_CUR ou SEEK_END).
 */
void mySeek(file* f, int offset, int base) {
    if (f == NULL || !f->isOpen) {
        return; 
    }
    int newPosition;
    switch (base) {
        case SEEK_SET:
            // On repart au début du fichier et on se décalle offset octet(s)
            newPosition = offset + f->startBlock + sizeOfEmptyFile();
            break;
        case SEEK_CUR:
            // De la position courante dans le fichier et on se décalle offset octet(s)
            newPosition = f->cursor + offset ;
            break;
        case SEEK_END:
            // On se decale à la fin du fichier et on se décalle offset octet(s)
            newPosition = (f->fileSize - sizeOfEmptyFile()) + f->startBlock + offset;
            break;
        default:
            return; 
    }

    // S'assurer que la position ne devient pas négative (Normalement c'est impossible d'avoir ce cas)
    if (newPosition < 0) {
        newPosition = 0; 
    }

    // Mettre à jour la taille du fichier
    f->cursor = newPosition;
}


/**
 * @brief Lit des données à partir du fichier.
 * @param f Le pointeur vers la structure de fichier.
 * @param buffer Le tampon où stocker les données lues.
 * @param nBytes Le nombre d'octets à lire.
 * @return Le nombre d'octets lus ou -1 en cas d'erreur.
 */
int myRead(file* f, void* buffer, int nBytes) {
    // Verification de l'intégrité du fichier
    if (f == NULL || buffer == NULL || !f->isOpen) {
        return -1; 
    }

    int partition = open(PARTITION_NAME, O_RDONLY);
    if (partition == -1) {
        perror("Echec ouverture de la partition\n");
        return -1; 
    }

    // Positionner le curseur au début du fichier dans la partition pour écrire les métadonnées (MAJ)
    lseek(partition, f->cursor, SEEK_SET);

    // Lecture des données depuis le fichier dans la partition vers le tampon
    int bytesRead = read(partition, buffer, nBytes);

    close(partition);
    bytesRead = 0;

    // La nombre réel d'octet lu
    while(((char *)buffer)[bytesRead] != '\0'){
        bytesRead ++;
    }

    return bytesRead;
}


/**
 * @brief Ecrit des données dans le fichier.
 * @param f Le pointeur vers la structure de fichier.
 * @param buffer Le tampon contenant les données à écrire.
 * @param nBytes Le nombre d'octets à écrire.
 * @return Le nombre d'octets écrits ou -1 en cas d'erreur.
 */
int myWrite(file* f, void* buffer, int nBytes) {
    if (f == NULL || buffer == NULL || !f->isOpen) {
        return -1; 
    }

    int partition = open(PARTITION_NAME, O_RDWR);
    if (partition == -1) {
        perror("Error opening partition file");
        return -1; 
    }

    // Positionnez le pointeur de lecture dal partition au niveau du curseur du fichier
    lseek(partition, f->cursor, SEEK_SET);

    // Écrire les nouvelles données dans le fichier
    int bytesWritten = write(partition, buffer, nBytes);
    if (bytesWritten == -1) {
        perror("Error writing to file");
        close(partition);
        return -1; 
    }
    // Mettez à jour la taille du fichier dans les métadonnées
    int pos = lseek(partition, 0, SEEK_CUR);
    f->fileSize = pos - (f->startBlock);
    
    // Positionner le curseur au début du fichier dans la partition pour écrire les métadonnées (MAJ)
    lseek(partition, f->startBlock, SEEK_SET);

    // Mettre à jour les métadonnées dans la partition
    write(partition, f, sizeof(file));
    f->cursor = f->startBlock + sizeOfEmptyFile();


    close(partition); 
    return bytesWritten;
}


/**
 * @brief Calcule la taille d'un fichier en octets : la taille effective du fichier avec les métadonnées moins l'espace reservé sur le blocs pour les métadonnées
 * @param f Le pointeur vers la structure de fichier.
 * @return La taille du fichier.
 */
int size(file* f){
    if (f == NULL) {
        printf("Erreur lors de l'ouverture du fichier.\n");
        return -1;
    }
    return f->fileSize - sizeOfEmptyFile();
}


