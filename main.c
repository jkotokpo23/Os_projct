#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "function.h" // Inclusion du fichier de fonction

int main() {
    char partitionName[] = "myPartition.bin";
    char fileName[] = "fichierDeTest.txt"; 

    int choice;
    file *openedFile = NULL;

    while (1) {
        printf("\nManuel de test:\n");
        printf("1.  Créer ou formater la partition\n");
        printf("2.  Ouvrir un fichier\n");
        printf("3.  Écrire dans un fichier\n");
        printf("4.  Lire un fichier\n");
        printf("5.  Lire en faisant un décalage mySeek(f, 2, SEEK_CUR);\n");
        printf("6.  Lire en faisant un décalage mySeek(f, 0, SEEK_SET);\n");
        printf("7.  Écrire/Créer un nouveau fichier autre que notre fichier de test;\n");
        printf("8.  Taille d'un fichier\n");
        printf("9.  Visualiser l'espace occupé dans la partition\n");
        printf("10. Écrire en faisant un décalage mySeek(f, 2, SEEK_CUR);\n");
        printf("11. Écrire en faisant un décalage mySeek(f, 0, SEEK_SET);\n");
        printf("0.  Quitter\n\n");
        printf("Votre choix: ");
        scanf("%d", &choice);

        switch (choice) {
            case 1:
                // Création ou formatage de la partition
                if (myFormat(partitionName) == 0) {
                    printf("Partition créée ou formatée avec succès.\n");
                } else {
                    printf("Erreur lors de la création ou du formatage de la partition.\n");
                }
                break;
            case 2:
                // Ouverture d'un fichier
                openedFile = myOpen(fileName);
                if (openedFile != NULL) {
                    printf("Fichier '%s' ouvert avec succès.\n", openedFile->fileName);
                } else {
                    printf("Erreur lors de l'ouverture du fichier.\n");
                }
                break;
            case 3:
                // Écriture dans un fichier
                if (openedFile != NULL) {
                    char data[100];
                    printf("Entrez les données à écrire dans le fichier : ");
                    scanf(" %[^\n]s", data);
                    int bytesWritten = myWrite(openedFile, data, strlen(data));
                    if (bytesWritten != -1) {
                        printf("Écriture réussie : %d octets écrits.\n", bytesWritten);
                    } else {
                        printf("Erreur lors de l'écriture dans le fichier.\n");
                    }
                } else {
                    printf("Aucun fichier ouvert.\n");
                }
                break;
            case 4:
                // Lecture d'un fichier
                if (openedFile != NULL) {
                    char readBuffer[100];
                    int bytesRead = myRead(openedFile, readBuffer, sizeof(readBuffer));
                    if (bytesRead != -1) {
                        // Ajout du caractère de fin de chaîne
                        readBuffer[bytesRead] = '\0'; 
                        printf("Contenu du fichier : %s\n", readBuffer);
                    } else {
                        printf("Erreur lors de la lecture du fichier.\n");
                    }
                } else {
                    printf("Aucun fichier ouvert.\n");
                }
                break;
            case 5:
                // Lecture avec un décalage
                if (openedFile != NULL) {
                    char readBuffer[100];
                    mySeek(openedFile, 2, SEEK_CUR);
                    int bytesRead = myRead(openedFile, readBuffer, sizeof(readBuffer));
                    if (bytesRead != -1) {
                        // Ajout du caractère de fin de chaîne
                        readBuffer[bytesRead] = '\0'; 
                        printf("Contenu du fichier : %s\n", readBuffer);
                    } else {
                        printf("Erreur lors de la lecture du fichier.\n");
                    }
                } else {
                    printf("Aucun fichier ouvert.\n");
                }
                break;
            case 6:
                // Lecture avec un décalage au début du fichier
                if (openedFile != NULL) {
                    char readBuffer[100];
                    mySeek(openedFile, 0, SEEK_SET);
                    int bytesRead = myRead(openedFile, readBuffer, sizeof(readBuffer));
                    if (bytesRead != -1) {
                        // Ajout du caractère de fin de chaîne
                        readBuffer[bytesRead] = '\0'; 
                        printf("Contenu du fichier : %s\n", readBuffer);
                    } else {
                        printf("Erreur lors de la lecture du fichier.\n");
                    }
                } else {
                    printf("Aucun fichier ouvert.\n");
                }
                break;
            case 7:
                // Écriture ou création d'un nouveau fichier
                char newFile[20];
                free(openedFile); 
                printf("Entrez le nom du fichier : \n");
                scanf(" %[^\n]s", newFile);
                openedFile = myOpen(newFile);
                if (openedFile != NULL) {
                    printf("Fichier '%s' ouvert avec succès.\n", openedFile->fileName);
                } else {
                    printf("Erreur lors de l'ouverture du fichier.\n");
                }
                break;
            case 8:
                // Affichage de la taille d'un fichier
                if (openedFile != NULL) {
                    printf("Taille du fichier '%s' : %d octets\n", openedFile->fileName, size(openedFile));
                } else {
                    printf("Aucun fichier ouvert.\n");
                }
                break;
            case 9:
                // Visualisation de l'espace occupé dans la partition
                visualisation(partitionName);
                break;
            case 10:
                // Écriture avec un décalage
                if (openedFile != NULL) {
                    char data[20];
                    printf("Entrez les données à écrire dans le fichier : ");
                    scanf(" %[^\n]s", data);
                    mySeek(openedFile, 2, SEEK_CUR);
                    int bytesWritten = myWrite(openedFile, data, strlen(data));
                    if (bytesWritten != -1) {
                        printf("Écriture réussie : %d octets écrits.\n", bytesWritten);
                    } else {
                        printf("Erreur lors de l'écriture dans le fichier.\n");
                    }
                } else {
                    printf("Aucun fichier ouvert.\n");
                }
                break;
            case 11:
                // Écriture avec un décalage au début du fichier
                if (openedFile != NULL) {
                    char data[100];
                    printf("Entrez les données à écrire dans le fichier : ");
                    scanf(" %[^\n]s", data);
                    mySeek(openedFile, 0, SEEK_SET);
                    int bytesWritten = myWrite(openedFile, data, strlen(data));
                    if (bytesWritten != -1) {
                        printf("Écriture réussie : %d octets écrits.\n", bytesWritten);
                    } else {
                        printf("Erreur lors de l'écriture dans le fichier.\n");
                    }
                } else {
                    printf("Aucun fichier ouvert.\n");
                }
                break;
            case 0:
                // Quitter le programme
                if (openedFile != NULL) {
                    // Libération de la mémoire allouée pour le fichier ouvert
                    free(openedFile); 
                }
                printf("Au revoir !\n");
                return 0;
            default:
                printf("Choix invalide. Veuillez entrer un nombre entre 0 et 11.\n");
        }
    }
    return 0;
}
