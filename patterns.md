## Read file line by line

- [StackOverflow: C read file line by line](https://stackoverflow.com/questions/3501338/c-read-file-line-by-line)

```c
FILE* filePointer;
int bufferLength = 255;
char buffer[bufferLength]; /* not ISO 90 compatible */

filePointer = fopen("file.txt", "r");

while(fgets(buffer, bufferLength, filePointer)) {
    printf("%s\n", buffer);
}

fclose(filePointer);
```
