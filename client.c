#include "eshop.h"

int main() {
    int sock;
    struct sockaddr_in server_addr;
    char buffer[256];

    srand(time(NULL) ^ getpid()); // Διαφορετικά προϊόντα για κάθε πελάτη

    // Δημιουργία socket
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == -1) {
        perror("Socket creation failed");
        return 1;
    }

    // Ορισμός server address
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = inet_addr(SERVER_IP);

    // Connect to server
    if (connect(sock, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
        perror("Connection failed");
        return 1;
    }

    printf("Connected to server. Sending orders...\n");

    // Αποστολή πολλαπλών order
    for (int i = 0; i < MAX_ORDERS; i++) {
        int product_id = rand() % 20;  // Generate varied product IDs
        sprintf(buffer, "%d", product_id);
        
        write(sock, buffer, strlen(buffer) + 1);
        read(sock, buffer, sizeof(buffer));

        printf("Server response: %s\n", buffer);
        sleep(1);  // Simulate time between orders
    }

    // Κλείσιμο socket
    close(sock);
    printf("Client disconnected.\n");
    return 0;
}