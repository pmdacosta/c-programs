/* Program 1.4 - Quick-union-wc (weighted and compressed)  solution to connectivity problem
 * Input: p - q pairs
 * Output: if p - q is a new connection print p - q else print nothing
 *
 * id[i] points to a connected node in the tree
 * if we follow each connected note we reach the root
 * if the roots are equal they are in the same set
 * if the roots are different they are in different sets
 *
 * we connected the smaller tree to the root of the bigger tree to keep
 * the maximum number of levels of the tree small in order to have a faster
 * find operation
 *
 * We also compress the tree to optimize futher
 */

#include <stdio.h>
#define N 100

int main() {
    int p;
    int q;
    int t;
    int i;
    int j;
    int id[N];
    int size[N];


    for (i = 0; i < N; i++) {
        id[i] = i; // every node points to itself
        size[i] = 1; // every tree is size 1
    }

    while (scanf("%d %d\n", &p, &q) == 2) {
        for (i = p; i != id[i]; i = id[i]) { // find root of p
            // find root of i and compress
            int r;
            for (r = i; r != id[r]; r = id[r]);
            id[i] = r;
        }
        for (j = q; j != id[j]; j = id[j]) { // find root of q
            // find root of j and compress
            int r;
            for (r = j; r != id[r]; r = id[r]);
            id[j] = r;
        }
                                            
        if (i == j) continue; // same root -> same set
        if (size[i] < size[j]) {
            id[i] = j; // make current root i point to j
            size[j] += size[i];
        } else {
            id[j] = i; // make current root j point to i
            size[i] += size[j];
        }
        printf(" %d %d\n", p, q);
    }

    return 0;
}
