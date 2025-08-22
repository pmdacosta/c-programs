/* Program 1.2 - Quick-union solution to connectivity problem
 * union operation O(1)
 * find operation O(N)
 * Input: p - q pairs
 * Output: if p - q is a new connection print p - q else print nothing
 *
 * id[i] points to a connected node in the tree
 * if we follow each connected note we reach the root
 * if the roots are equal they are in the same set
 * if the roots are different they are in different sets
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


    for (i = 0; i < N; i++) id[i] = i; // initialize id array

    while (scanf("%d %d\n", &p, &q) == 2) {
        for (i = p; i != id[i]; i = id[i]); // find the root of p
        for (j = q; j != id[j]; j = id[j]); // find the root of q
                                            
        if (i == j) continue; // same root -> same set
        id[i] = j; // make current root i point to j
        printf(" %d %d\n", p, q);
    }

    return 0;
}
