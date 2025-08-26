The preprocess is the first step run in compilation process.

When we write `#include <stdio.h>` the preprocessor will copy the contents of the `stdio.h` file into our source file. 

With the `#define` directive we can define literals that will be replaced by the preprocessor in the source code.

```c
#define MAX_PERSONS 1024

int max = MAX_PERSONS;
```

will become

```
int max = 1024;
```

We call also test for definitions with `#ifdef`. We can define sets of code to only exists in the source before compilation if a define directive is set like `#define DEBUG`. 

```c
#ifdef DEBUG
    printf("WE ARE IN DEBUG MODE: %s:%d\n", __FILE__, __LINE__);
#endif
```

We can then use `#define DEBUG` or we can also pass the directive to `gcc` as such

```bash
gcc -o binary filename.c -DDEBUG
```
