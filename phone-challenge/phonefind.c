#include <stdio.h>

#define INITIAL_CAPACITY 16
#define NAME_LEN 100
#define PHONE_LEN 20

struct Contact {
    int id;
    char name[NAME_LEN];
    char phone[PHONE_LEN];
};

int main(int argc, char *argv[]) {
    if(argc < 3) {
        puts("Usage: phonefind <csv-file> <search-term>");
        return 1;
    }

    char *filename = argv[1];
    char *search_term = argv[2];

    FILE *file = fopen(filename, "r");
    if (!file) {
        puts("Error reading file.");
        return 1;
    }

    return 0;
}