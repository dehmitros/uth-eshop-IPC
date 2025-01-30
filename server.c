#include "eshop.h"

Product *catalog;  // Δείκτης στη μνήμη κοινού καταλόγου
// Αρχικοποίηση του καταλόγου προϊόντων με τυχαίες τιμές και προεπιλεγμένες τιμές
void init_catalog() {
    // Κατανομή κοινής μνήμης για τον κατάλογο
    catalog = mmap(NULL, sizeof(Product) * MAX_PRODUCTS, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    if (catalog == MAP_FAILED) {
        perror("mmap failed");
        exit(1);
    }

    // Γέμισμα του καταλόγου με αρχικές τιμές
    for (int i = 0; i < MAX_PRODUCTS; i++) {
        sprintf(catalog[i].description, "Product_%d", i);
        catalog[i].price = (rand() % 100) + 1; // Τυχαία τιμή μεταξύ 1 και 100
        catalog[i].item_count = 2; // Προεπιλεγμένο απόθεμα για κάθε προϊόν
        catalog[i].order_requests = 0;
        catalog[i].orders_completed = 0;
        catalog[i].failed_clients[0] = '\0'; // Αρχικοποίηση ως κενή αλυσίδα χαρακτήρων
    }
}

// Διαχείριση παραγγελιών πελατών
void handle_client(int client_sock, int client_id) {
    char buffer[256];

    for (int i = 0; i < MAX_ORDERS; i++) {
        // Ανάγνωση ταυτότητας προϊόντος από τον πελάτη
        read(client_sock, buffer, sizeof(buffer));
        int product_id = atoi(buffer);

        // Επικύρωση product ID
        if (product_id < 0 || product_id >= MAX_PRODUCTS) {
            sprintf(buffer, "Invalid Product ID");
        } else {
            catalog[product_id].order_requests++; // Προσαύξηση request μετρητή
            sleep(1); // Προσομοίωση delay

            // Έλεγχος stock 
            if (catalog[product_id].item_count > 0) {
                catalog[product_id].item_count--; // Αφαίρεση από stock
                catalog[product_id].orders_completed++; // Προσαύξηση successful order μετρητή
                sprintf(buffer, "Order Successful: %s, Price: %.2f", catalog[product_id].description, catalog[product_id].price);
            } else {
                // Out of stock
                sprintf(buffer, "Order Failed: %s is out of stock", catalog[product_id].description);
                sprintf(catalog[product_id].failed_clients + strlen(catalog[product_id].failed_clients), "Client_%d ", client_id);
            }
        }
        // Απάντηση στο client
        write(client_sock, buffer, sizeof(buffer));
    }
    close(client_sock); // Κλείσιμο client connection
    exit(0); // Έξοδος child process
}

void generate_report() {
    printf("\nE-Shop Sales Report:\n");
    int total_orders = 0, successful_orders = 0, failed_orders = 0;
    float total_revenue = 0;

    // Διαβάζει τον κατάλογο και ξεκινάει το summarize
    for (int i = 0; i < MAX_PRODUCTS; i++) {
        total_orders += catalog[i].order_requests;
        successful_orders += catalog[i].orders_completed;
        failed_orders += (catalog[i].order_requests - catalog[i].orders_completed);
        total_revenue += catalog[i].orders_completed * catalog[i].price;

        // Print product πληροφορίες για sales
        printf("%s - Requests: %d, Sold: %d, Failed Clients: %s\n", 
               catalog[i].description, catalog[i].order_requests, catalog[i].orders_completed, catalog[i].failed_clients);
    }
    // Print το summary
    printf("\nTotal Orders: %d\nSuccessful Orders: %d\nFailed Orders: %d\nTotal Revenue: %.2f\n", 
           total_orders, successful_orders, failed_orders, total_revenue);
}

int main() {
    srand(time(NULL)); // Τυχαίος αριθμός
    init_catalog();

    int server_sock, client_sock;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_len = sizeof(client_addr);

    // Δημιουργία server socket
    server_sock = socket(AF_INET, SOCK_STREAM, 0);
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    // Bind το socket στο port
    bind(server_sock, (struct sockaddr*)&server_addr, sizeof(server_addr));

    // Εκκίνηση listening
    listen(server_sock, MAX_CLIENTS);
    printf("Server listening on port %d...\n", PORT);

    // Αποδοχή πολλαπλών client και δημιουργία νέου process για καθένα 
    for (int i = 0; i < MAX_CLIENTS; i++) {
        client_sock = accept(server_sock, (struct sockaddr*)&client_addr, &client_len);

        if (fork() == 0) { // Child process
            close(server_sock);
            handle_client(client_sock, i);
        }
        close(client_sock); // Parent κλείνει client socket έπειτα από forking
    }

    // Wait για όλα τα child processes να τελειώσουν
    while (wait(NULL) > 0);

    close(server_sock); // Κλείσιμο server socket
    generate_report(); // Δημιουργία final report

    return 0;
}