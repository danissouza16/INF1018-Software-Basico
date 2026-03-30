/* Lab 1 - EDA */
/* Daniel Santana Souza - 2310995 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main(void) {
    int p[10];
    int ant;
    int i,j;
    srand(time(NULL));
    printf("vetor sem ordenação:\n");
    for (i=0;i<10;i++) {
        p[i] = rand() % 101;
        printf("%d ", p[i]);
    }

    for (i=10;i != 0;i--) {
        for (j=0;j<i-1;j++) {
            if (p[j] > p[j+1]) {
                ant = p[j];
                p[j] = p[j+1];
                p[j+1] = ant;
            }
        }
        
    }
    printf("vetor com ordenação:\n");
    for (i=0;i<10;i++) {
        printf("%d ", p[i]);
    }
    return 0;
}