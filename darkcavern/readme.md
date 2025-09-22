## Lazy Foo Productions SDL2 tutorial

- [tutorial](https://lazyfoo.net/tutorials/SDL/index.php#Getting%20an%20Image%20on%20the%20Screen)


### Extra

#### Code navigation using vim

Good question 👍 — navigating C code in Vim can be really smooth once you know the right commands. Here’s a solid set of the **most useful navigation commands and motions** for C (and similar languages):

---

### 📌 Moving Around Code

* `w` → move forward to the beginning of the next word.
* `e` → move forward to the end of the current/next word.
* `b` → move backward to the beginning of the current/previous word.
* `%` → jump between matching braces `{} / [] / ()` (very useful in C!).
* `[[` → jump backward to the beginning of a function.
* `]]` → jump forward to the beginning of the next function.
* `[{` → jump to the beginning of the current block `{}`.
* `]}` → jump to the end of the current block.

---

### 📌 Searching & Jumps

* `*` → search forward for the word under the cursor.
* `#` → search backward for the word under the cursor.
* `/pattern` → search forward for `pattern`.
* `?pattern` → search backward for `pattern`.
* `gd` → go to local declaration of variable under cursor.
* `gD` → go to global declaration.

---

### 📌 Tags (needs `ctags`)

If you run `ctags -R .` in your project:

* `<C-]>` → jump to definition of symbol under cursor.
* `<C-t>` → jump back to where you came from.

This is great for moving between function calls and their definitions.

---

### 📌 Marks & Jumps

* `m[a-z]` → set a mark (bookmark) at the cursor.
* `` `[a-z] `` → jump to that mark.
* `''` → jump back to the line where you were before.
* `\`\`` → jump back to the exact position.

---

### 📌 Buffers & Files

* `:e filename.c` → open a file.
* `:bnext` / `:bn` → go to next buffer.
* `:bprev` / `:bp` → go to previous buffer.
* `:ls` → list open buffers.
* `:b <num>` → jump to buffer `<num>`.

---


