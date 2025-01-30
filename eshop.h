#ifndef ESHOP_H
#define ESHOP_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <sys/mman.h>

#define SERVER_IP "127.0.0.1"
#define PORT 8080
#define MAX_PRODUCTS 20
#define MAX_CLIENTS 5
#define MAX_ORDERS 10

// Δομή για την αναπαράσταση ενός προϊόντος
typedef struct {
    char description[50];  // Περιγραφή προϊόντος
    float price;           // Τιμή προϊόντος
    int item_count;        // Διαθέσιμος αριθμός αποθέματος
    int order_requests;    // Αριθμός αιτημάτων για αυτό το προϊόν
    int orders_completed;  // Αριθμός επιτυχώς ολοκληρωμένων παραγγελιών
    char failed_clients[256]; // Πελάτες που δεν κατάφεραν να παραγγείλουν λόγω έλλειψης αποθέματος
} Product;

// Κοινόχρηστο catalog pointer
extern Product *catalog;

// functions
void init_catalog();
void handle_client(int client_sock, int client_id);
void generate_report();

#endif // ESHOP_H
