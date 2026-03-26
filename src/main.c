#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h> // Untuk isdigit

// --- Struktur Data ---

// Log aktivitas
typedef struct Log {
    int log_id;
    char type[10]; // "receive", "ship", "restock"
    int quantity;
    struct Log *next;
} Log;

// Item barang
typedef struct Item {
    char id[100];
    char name[100];
    int quantity;
    double price;
    Log *logs; // Linked list of logs for this item
    struct Item *next;
} Item;

// Linked list global untuk semua barang
Item *head_item = NULL;
int global_log_id_counter = 0; // Counter global untuk log_id

// --- Fungsi Utilitas ---

// Fungsi untuk membuat node log baru
Log *create_log(const char *type, int quantity) {
    Log *new_log = (Log *)malloc(sizeof(Log));
    if (new_log == NULL) {
        fprintf(stderr, "Memory allocation failed for log.\n");
        exit(EXIT_FAILURE);
    }
    new_log->log_id = ++global_log_id_counter;
    strncpy(new_log->type, type, sizeof(new_log->type) - 1);
    new_log->type[sizeof(new_log->type) - 1] = '\0';
    new_log->quantity = quantity;
    new_log->next = NULL;
    return new_log;
}

// Fungsi untuk menambahkan log ke item
void add_log_to_item(Item *item, const char *type, int quantity) {
    Log *new_log = create_log(type, quantity);
    if (item->logs == NULL) {
        item->logs = new_log;
    } else {
        Log *current_log = item->logs;
        while (current_log->next != NULL) {
            current_log = current_log->next;
        }
        current_log->next = new_log;
    }
}

// Fungsi untuk membuat node item baru
Item *create_item(const char *id, const char *name, int quantity, double price) {
    Item *new_item = (Item *)malloc(sizeof(Item));
    if (new_item == NULL) {
        fprintf(stderr, "Memory allocation failed for item.\n");
        exit(EXIT_FAILURE);
    }
    strncpy(new_item->id, id, sizeof(new_item->id) - 1);
    new_item->id[sizeof(new_item->id) - 1] = '\0';
    strncpy(new_item->name, name, sizeof(new_item->name) - 1);
    new_item->name[sizeof(new_item->name) - 1] = '\0';
    new_item->quantity = quantity;
    new_item->price = price;
    new_item->logs = NULL; // Inisialisasi log kosong
    new_item->next = NULL;
    return new_item;
}

// Fungsi untuk mencari item berdasarkan ID
Item *find_item(const char *id) {
    Item *current = head_item;
    while (current != NULL) {
        if (strcmp(current->id, id) == 0) {
            return current;
        }
        current = current->next;
    }
    return NULL; // Item tidak ditemukan
}

// Fungsi untuk membersihkan memori (penting untuk menghindari memory leak)
void free_all_memory() {
    Item *current_item = head_item;
    while (current_item != NULL) {
        Item *next_item = current_item->next;
        Log *current_log = current_item->logs;
        while (current_log != NULL) {
            Log *next_log = current_log->next;
            free(current_log);
            current_log = next_log;
        }
        free(current_item);
        current_item = next_item;
    }
    head_item = NULL;
}

// --- Implementasi Perintah ---

// Perintah receive
void handle_receive(char *token) {
    char *id = strtok(token, "#");
    char *name = strtok(NULL, "#");
    char *quantity_str = strtok(NULL, "#");
    char *price_str = strtok(NULL, "#");

    if (id == NULL || name == NULL || quantity_str == NULL || price_str == NULL) {
        // Input tidak valid, abaikan
        return;
    }

    int quantity = atoi(quantity_str);
    double price = atof(price_str);

    if (quantity <= 0 || price <= 0) { // Validasi quantity dan price positif
        return;
    }

    Item *existing_item = find_item(id);
    if (existing_item != NULL) {
        // Jika barang sudah ada, update quantity dan price
        existing_item->quantity += quantity;
        existing_item->price = price; // Update price dengan yang terbaru
        add_log_to_item(existing_item, "receive", quantity);
    } else {
        // Jika barang baru, tambahkan ke linked list
        Item *new_item = create_item(id, name, quantity, price);
        if (head_item == NULL) {
            head_item = new_item;
        } else {
            Item *current = head_item;
            while (current->next != NULL) {
                current = current->next;
            }
            current->next = new_item;
        }
        add_log_to_item(new_item, "receive", quantity);
    }
}

// Perintah ship
void handle_ship(char *token) {
    char *id = strtok(token, "#");
    char *quantity_str = strtok(NULL, "#");

    int quantity = atoi(quantity_str);

    if (quantity <= 0) { 
        return;
    }

    Item *item = find_item(id);
    if (item != NULL) {
        if (item->quantity >= quantity) { // Stok cukup
            item->quantity -= quantity;
            add_log_to_item(item, "ship", quantity);
        }
        // else: Stok tidak cukup, abaikan
}

// Perintah restock
void handle_restock(char *token) {
    char *id = strtok(token, "#");
    char *quantity_str = strtok(NULL, "#");

    if (id == NULL || quantity_str == NULL) {
        return;
    }

    int quantity = atoi(quantity_str);

    if (quantity <= 0) { 
        return;
    }

    Item *item = find_item(id);
    if (item != NULL) {
        item->quantity += quantity;
        add_to_item(item, "restock", quantity);
    }
    // else: Barang tidak ditemukan, abaikan

// Perintah audit
void handle_audit(char *token) {
    char *id = strtok(token, "#");

    if (id == NULL) {
        return;
    }

    