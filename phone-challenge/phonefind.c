#define _GNU_SOURCE // needed for strcasestr
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define INITIAL_CAPACITY 16
#define NAME_LEN 100
#define PHONE_LEN 20
#define SEARCH_ANY 0
#define SEARCH_PHONE 1
#define SEARCH_NAME 2

int g_case = 0;
int g_search = SEARCH_ANY;

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

int contains(const char *haystack, const char *needle) {
    return g_case ? (strstr(haystack, needle) != NULL)
        : (strcasestr(haystack, needle) != NULL);
}


int check_matches_name(const char *search_term, struct Contact *contact) {
    return contains(contact->name, search_term);
}

int check_matches_phone(const char *search_term, struct Contact *contact) {
    return contains(contact->phone, search_term);
}

int check_matches_any(const char *search_term, struct Contact *contact) {
    if(contains(contact->name, search_term)) return 1;
    return contains(contact->phone, search_term);
}

// returns 1 if matches, 0 otherwise
int check_matches(const char *search_term, struct Contact *contact) {
    if (g_search == SEARCH_NAME) {
        return check_matches_name(search_term, contact);
    } else if (g_search == SEARCH_PHONE) {
        return check_matches_phone(search_term, contact); 
    } else {
        return check_matches_any(search_term, contact);
    }
    return 0;
}

struct Contact *find_contacts(struct Contact *contacts, int contacts_size,
                              const char *search_term, int *out_count) {
    int count = 0;
    struct Contact *matches = malloc(contacts_size * sizeof(struct Contact));
    if(!matches) {
        puts("Error allocating memory in find_contacts");
        *out_count = 0;
        return NULL;
    }

    for (int i = 0; i < contacts_size; i++) {
        if (check_matches(search_term, &contacts[i])) {
            matches[count] = contacts[i];
            count++;
        }
    }
    *out_count = count;
    return matches;
}

void print_usage(void) {
    puts("Usage: phonefind <csv-file> <search-term> [--case] [--search=name|phone]");
}


int main(int argc, char *argv[]) {
    if (argc < 3) {
        print_usage();
        return 1;
    }

    if (argc > 3) {
        for(int i = 3; i < argc; i++) {
            if (strcmp(argv[i], "--case") == 0) {
                g_case = 1;
            } else if (strcmp(argv[i], "--search=name") == 0) {
                g_search = SEARCH_NAME;
            } else if (strcmp(argv[i], "--search=phone") == 0) {
                g_search = SEARCH_PHONE;
            } else {
                print_usage();
                return 1;
            }
        }
    }

    char *filename = argv[1];
    char *search_term = argv[2];

    int count = 0;
    struct Contact *contacts = read_contacts(filename, &count);

    if (!contacts) {
        perror("Error reading contacts");
        return 1;
    }

    int match_count = 0;
    struct Contact *matches =
        find_contacts(contacts, count, search_term, &match_count);

    if (!match_count) {
        puts("No contacts found");
        free(contacts);
        free(matches);
        return 0;
    }

    for (int i = 0; i < match_count; i++) {
        printf("id: %d | name: %s | phone: %s\n", matches[i].id,
               matches[i].name, matches[i].phone);
    }

    free(contacts);
    free(matches);
    return 0;
}
