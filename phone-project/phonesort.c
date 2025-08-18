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

/* Comparison functions */
int compare_by_id(const void *a, const void *b) {
    const struct Contact *c1 = a;
    const struct Contact *c2 = b;
    return c1->id - c2->id;
}

int compare_by_name(const void *a, const void *b) {
    const struct Contact *c1 = a;
    const struct Contact *c2 = b;
    return strcmp(c1->name, c2->name);
}

int compare_by_phone(const void *a, const void *b) {
    const struct Contact *c1 = a;
    const struct Contact *c2 = b;
    long p1 = strtol(c1->phone, NULL, 10);
    long p2 = strtol(c2->phone, NULL, 10);

    if (p1 < p2)
        return -1;
    if (p1 > p2)
        return 1;
    return 0;
}

/* Reads contacs from a .csv file into a dynamic array */
struct Contact *read_contacts(const char *filename, int *out_count) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        perror("Error opening file");
        return NULL;
    }

    int count = 0;
    int capacity = INITIAL_CAPACITY;
    struct Contact *contacts = malloc(capacity * sizeof(struct Contact));
    if (!contacts) {
        perror("malloc failed");
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

void print_contacts(struct Contact *contacts, int count) {
    for (int i = 0; i < count; i++) {
        printf("ID: %d | Name: %s | Phone: %s\n", contacts[i].id,
               contacts[i].name, contacts[i].phone);
    }
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        puts("Usage: phonesort <csv-file> --sort=<id|name|phone>");
        return 1; /* return 1 = error */
    }

    const char *filename = argv[1];
    char *sort_flag = "--sort=id"; // default if no sort flag provided
    if (argc == 3) {
        sort_flag = argv[2];
    }
    int (*compare_func)(const void *, const void *) = NULL;

    if (strcmp(sort_flag, "--sort=id") == 0) {
        compare_func = compare_by_id;
    } else if (strcmp(sort_flag, "--sort=name") == 0) {
        compare_func = compare_by_name;
    } else if (strcmp(sort_flag, "--sort=phone") == 0) {
        compare_func = compare_by_phone;
    } else {
        puts("Invalid sort flag. Use --sort=id, --sort=name or --sort=phone");
        return 1;
    }

    int count = 0;
    struct Contact *contacts = read_contacts(filename, &count);
    if(!contacts) {
        perror("Error reading contacts");
        return 1;
    }
    /* Sort contacts by phone */
    qsort(contacts, count, sizeof(struct Contact), compare_func);
    print_contacts(contacts, count);

    free(contacts);
    return 0; /* success */
}
