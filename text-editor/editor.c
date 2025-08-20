#include <stdio.h>
#include <string.h>

#define BUFFER_LEN 1024

void edit_line(char *buffer, int current_line) {
    for(int i = 0; i < current_line; i++) {
        buffer = strchr(buffer, '\n') + 1;
    }

    char *line_end = strchr(buffer, '\n');
    char saved[1024] = {0};
    strcpy(saved, line_end);
    scanf("%s", buffer);
    strcpy(buffer + strlen(buffer), saved);
}

int main(int argc, char *argv[]) {
    const char *filename = argv[1];
    FILE *file = fopen(filename, "r");
    char buffer[BUFFER_LEN] = {0};
    fread(buffer, BUFFER_LEN, 1, file);
    fclose(file);
    int current_line = 0;
    printf("%s\ned: ", buffer);
    scanf("%d", &current_line);
    current_line--;
    edit_line(buffer, current_line);
    file = fopen(filename, "w");
    fwrite(buffer, BUFFER_LEN, 1, file);
    fclose(file);

    return 0;
}
