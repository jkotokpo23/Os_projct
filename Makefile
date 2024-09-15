# Compiler
CC = gcc
# Options de compilation
CFLAGS = -Wall -Wextra -g
# Fichiers source
SRCS = main.c function.c
# Fichiers objets
OBJS = $(SRCS:.c=.o)
# Exécutable
EXEC = projet

# Règle de construction de l'exécutable
$(EXEC): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(EXEC)

# Règle de construction des fichiers objets
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Nettoyer les fichiers objets et l'exécutable
clean:
	rm -f $(OBJS) $(EXEC)
