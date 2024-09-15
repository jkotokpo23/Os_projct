/**
 * @file function.h
 * @brief Définition des constantes, structures et prototypes de fonctions pour la gestion des fichiers dans une partition simulée.
 * @date 2024-03-29
 */

#ifndef FUNCTION_H
#define FUNCTION_H

/** @def BLOCK_SIZE
 *  @brief Taille de bloc pour notre partition.
 */
#define BLOCK_SIZE 2048

/** @def PARTITION_SIZE
 *  @brief Taille de la partition en octets (32 Mo).
 */
#define PARTITION_SIZE (32 * 1024 * 1024)

/** @def PARTITION_NAME
 *  @brief Nom de la partition.
 */
#define PARTITION_NAME "myPartition.bin"

/** @def nameSize
 *  @brief Taille maximale du nom de fichier.
 */
#define nameSize 20

/* Structure de fichiers */
typedef struct {
    char fileName[nameSize];   /**< Nom du fichier. */
    int fileSize;              /**< Taille du fichier en octets. */
    int startBlock;            /**< Bloc de départ du fichier dans la partition. */
    int isOpen;                /**< Indicateur si le fichier est ouvert ou non. */
    int isUsed;                /**< Indicateur si le fichier est utilisé ou non. */
    int cursor;                /**< Position du curseur de lecture/écriture dans le fichier. */
} file;

/* Prototypes des fonctions */
int myFormat(char* partitionName);

file* myOpen(char* fileName);

void visualisation(char* partitionName);

void mySeek(file* f, int offset, int base);

int myRead(file* f, void* buffer, int nBytes);

int myWrite(file* f, void* buffer, int nBytes);

int size(file* f);

#endif /* FUNCTION_H */
