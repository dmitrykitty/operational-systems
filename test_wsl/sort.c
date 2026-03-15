#include "sort.h"

int selection_sort(float* num,int n){
    int steps = 0; 
    for(int i = 0; i < n - 1; i++){
        float min_val = num[i];
        int min_idx = i;
        for(int j = i + 1; j < n; j++){
            steps++;
            if(min_val > num[j]){
                min_val = num[j]; 
                min_idx = j; 
            }
        }

        float temp = num[i];
        num[i] = min_val;
        num[min_idx] = temp;  
    }
    return steps;
}

int bubble_sort(float* num,int n){
    int steps = 0; 
    for(int i = 0; i < n - 1; i++){
        for(int j = i; j < n - 1; j++){
            steps++;
            if(num[j] > num[j + 1]){
                int temp = num[j];
                num[j] = num[j + 1]; 
                num[j + 1] = temp;  
            }
        }
    }
    return steps;
}

