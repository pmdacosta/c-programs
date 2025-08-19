phonesort: phonefind.c
	gcc -std=c17 -Wall -Wextra -O2 -o phonefind phonefind.c

run:
	./phonefind contacts.csv john --search=phone
	./phonefind contacts.csv john --search=name
	./phonefind contacts.csv 274 --search=name
	./phonefind contacts.csv 274 --search=phone
	./phonefind contacts.csv John --search=name --case
	./phonefind contacts.csv John --search=phone --case
	./phonefind contacts.csv 274 --search=phone --case

clean:
	rm -f phonesort
