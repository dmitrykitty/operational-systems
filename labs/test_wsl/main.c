#include <stdio.h>
#include "sort.h"

//gcc main.c -o main.out
//./main.out

//to compile few files in one 
//gcc main.c sort.c -o main.out 
int main(){
    printf("Hello World\n");
    float arr[] = {64, 34, 25, 12, 22, 11, 90};
    int n = sizeof(arr) / sizeof(arr[0]);

    int steps_selection = selection_sort(arr, n);
    for(int i = 0; i < n; i++){
        printf("%.2f ", arr[i]);
        if(i == n - 1){
            printf("\n");
        }
    }
    printf("Steps during selection sort: %d\n", steps_selection);

    float arr2[] = {64, 34, 25, 12, 22, 11, 90};
    int steps_bubble = bubble_sort(arr2, n);
    for(int i = 0; i < n; i++){
        printf("%.2f ", arr[i]);
        if(i == n - 1){
            printf("\n");
        }
    }
    printf("Steps during bubble sort: %d\n", steps_bubble);

    return 0;
}