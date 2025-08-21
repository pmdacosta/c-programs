Let’s build a **tiny TCP echo server** in C called `echod`. It’s a classic systems-prog task: sockets, I/O, basic error handling — perfect next step. We’ll go slow like before, then I’ll give you a related challenge after we finish.

## Plan (at a glance)

1. v0: minimal program (no sockets)
2. Parse/validate port
3. Create socket, bind `127.0.0.1:PORT`, listen
4. Accept **one** client and echo back what it sends
5. Handle multiple clients **sequentially** (loop accept)
6. Handle multiple clients **simultaneously** with `select()`
7. Add a very simple Makefile and a `clean` target
8. Graceful shutdown on `Ctrl+C` (SIGINT)

---

# Step 1 — smallest possible program

### 1) Create `echod.c`

```bash
vim echod.c
```

Paste this and save (`:wq`):

```c
#include <stdio.h>

/* echod v0: just a stub */
int main(void) {
    puts("echod v0: hello");
    return 0;
}
```

### 2) Compile & run (no Makefile yet)

```bash
gcc -std=c17 -Wall -Wextra -O2 -o echod echod.c
./echod
```

You should see:

```
echod v0: hello
```

