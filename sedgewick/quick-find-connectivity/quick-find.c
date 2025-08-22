/* Program 1.1 - Quick-find solution to connectivity problem
 * find operation O(1)
 * union operation O(N)
 * Input: p - q pairs
 * Output: if p - q is a new connection print p - q else print nothing
 *
 * if id[p] equals id[p] they are in the same set
 * else they are in different sets
 */

#include <stdio.h>
#define N 100

int main() {
    int p;
    int q;
    int t;
    int i;
    int id[N];


    for (i = 0; i < N; i++) id[i] = i; // initialize id array
                                           //
                                           //id[i] = i implies that each number
                                           // is only connected to itself

    while (scanf("%d %d\n", &p, &q) == 2) {
        // are p and q connected (part of the same set)?
        if (p >= N || q >= N) continue; // if a number is out of range we'll ignore it
        if (id[p] == id[q]) continue; // nothing to do if they are connected
        t = id[p]; // we'll p and q to be part of the same set
                   // (tagged as t which is the value of id[p]
        for (i = 0; i < N; i++) {
            if (id[i] == t) id[i] = id[q];
        }
        printf(" %d %d\n", p, q);
    }

    return 0;
}
