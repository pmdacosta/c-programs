Alright — let’s give you something that:

* Builds on what you already learned (CSV parsing, structs, dynamic arrays, sorting)
* Introduces **a few new concepts** without jumping into overly advanced territory
* Is small enough to finish in a few focused sessions

---

## **Challenge:** Contact Search Tool

**Description:**
Write a C program called `phonefind` that:

1. **Reads** the same kind of CSV file as before (`id,name,phone`)
2. **Takes a search term** as a command-line argument (could be a name or a phone number)
3. **Finds and displays** all matching contacts (case-insensitive for names)
4. If no match is found, print `"No contacts found."`
5. (Optional bonus) Allow partial matches — e.g., searching `"Joh"` finds `"John Smith"` and `"Johanna Doe"`

---

### **Example usage**

```bash
./phonefind contacts.csv "john"
```

Output:

```
ID: 8743 | Name: Emma Johnson | Phone: 2025892741
ID: 4217 | Name: John Peterson | Phone: 2023942847
```

```bash
./phonefind contacts.csv "202589"
```

Output:

```
ID: 8743 | Name: Emma Johnson | Phone: 2025892741
```

---

### **Skills you’ll practice**

* Reading CSV (already know this)
* Command-line argument handling (already know this)
* **Case-insensitive string comparison**
* **Partial string matching** with `strstr()` or `strcasestr()`
* More **conditional logic**

---

If you want, I can also **add an extra twist**:
make it so the program **can read multiple CSV files at once** and search through them all.

Do you want me to keep it single-file for now or add the multi-CSV twist?

