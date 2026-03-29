#include <stdio.h>
#include "sort.h"

int main(){
    float arr[] = {64, 34, 25, 12, 22, 11, 90};
    int n = sizeof(arr) / sizeof(arr[0]);

    selection_sort(arr, n);
    for(int i = 0; i < n; i++){
        printf("%.2f ", arr[i]);
        if(i == n - 1){
            printf("\n");
        }
    }

    float arr2[] = {64, 34, 25, 12, 22, 11, 90};
    bubble_sort(arr2, n);
    for(int i = 0; i < n; i++){
        printf("%.2f ", arr2[i]);
        if(i == n - 1){
            printf("\n");
        }
    }
}