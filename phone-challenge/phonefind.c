#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define INITIAL_CAPACITY 16
#define NAME_LEN 100
#define PHONE_LEN 20

struct Contact {
    int id;
    char name[NAME_LEN];
    char phone[PHONE_LEN];
};

struct Contact *read_contacts(const char *filename, int *out_count) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("Error reading file.");
        return NULL;
    }

    int count = 0;
    int capacity = INITIAL_CAPACITY;
    struct Contact *contacts = malloc(capacity * sizeof(struct Contact));
    if (!contacts) {
        perror("Malloc failed");
        fclose(file);
        return NULL;
    }

    char buffer[1024];
    fgets(buffer, sizeof(buffer), file); // skip the header
    while (fgets(buffer, sizeof(buffer), file)) {
        /* remove trailing new line */
        buffer[strcspn(buffer, "\n")] = '\0';

        char *id = strtok(buffer, ",");
        char *name = strtok(NULL, ",");
        char *phone = strtok(NULL, ",");

        if (id && name && phone) {
            if (count == capacity) {
                // we hit capicity, need to allocate more space
                capacity *= 2;
                struct Contact *tmp =
                    realloc(contacts, capacity * sizeof(struct Contact));
                if (!tmp) {
                    perror("realloc failed");
                    free(contacts);
                    fclose(file);
                    return NULL;
                }
                contacts = tmp;
            }
            contacts[count].id = atoi(id);
            strncpy(contacts[count].name, name, NAME_LEN - 1);
            contacts[count].name[NAME_LEN - 1] = '\0';
            strncpy(contacts[count].phone, phone, PHONE_LEN - 1);
            contacts[count].phone[PHONE_LEN - 1] = '\0';
            count++;
        }
    }

    fclose(file);
    *out_count = count;
    return contacts;
}

int main(int argc, char *argv[]) {
    if (argc < 3) {
        puts("Usage: phonefind <csv-file> <search-term>");
        return 1;
    }

    char *filename = argv[1];
    char *search_term = argv[2];

    int count = 0;
    struct Contact *contacts = read_contacts(filename, &count);

    if (!contacts) {
        perror("Error reading contacts");
        return 1;
    }

    for (int i = 0; i < count; i++) {
        printf("id: %d | name: %s | phone: %s\n", contacts[i].id,
               contacts[i].name, contacts[i].phone);
    }

    return 0;
}