CC = gcc
CFLAGS = -Wall -Wextra -g
LDFLAGS = -pthread

# Ονόματα executable 
CLIENT = client
SERVER = server

# Πηγαία αρχεία
CLIENT_SRC = client.c
SERVER_SRC = server.c

# Header αρχείο
HEADERS = eshop.h

# Build rules
all: $(CLIENT) $(SERVER)

$(CLIENT): $(CLIENT_SRC) $(HEADERS)
	$(CC) $(CFLAGS) -o $(CLIENT) $(CLIENT_SRC)

$(SERVER): $(SERVER_SRC) $(HEADERS)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $(SERVER) $(SERVER_SRC)

clean:
	rm -f $(CLIENT) $(SERVER)

.PHONY: all clean