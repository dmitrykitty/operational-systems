#include "sort.h"

void selection_sort(float* num,int n){
    for(int i = 0; i < n - 1; i++){
        float min_val = num[i];
        int min_idx = i;
        for(int j = i + 1; j < n; j++){
            if(min_val > num[j]){
                min_val = num[j]; 
                min_idx = j; 
            }
        }

        float temp = num[i];
        num[i] = min_val;
        num[min_idx] = temp;  
    }
}

void bubble_sort(float* num, int n){
    for(int i = 0; i < n - 1; i++){
        for(int j = 0; j < n - i - 1; j++){
            if(num[j] > num[j + 1]){
                float temp = num[j];
                num[j] = num[j + 1];
                num[j + 1] = temp;
            }
        }
    }
}

