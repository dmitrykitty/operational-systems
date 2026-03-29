#include <stdlib.h>
#include <stdio.h>
#include <dlfcn.h>

// pointer to function taking (float*, int) and returning void 
typedef void (*sort_ptr)(float*, int);

int main(){
    float arr_source[] = {64, 34, 25, 12, 22, 11, 90};
    int n = sizeof(arr_source) / sizeof(arr_source[0]);


    float* arr = calloc(n, sizeof(arr_source[0])); 

    if(arr == NULL){
        fprintf(stderr, "Memory allocation error occured\n");
        return 1; 
    }

    for(int i = 0; i < n; i++){
        arr[i] = arr_source[i]; 
    }

    //dlopen - openes mylib
    void *handle = dlopen("./libsort.so", RTLD_LAZY);
    if (handle == NULL){
        fprintf(stderr, "Library not available\n");
        free(arr);
        return 1;
    }

    sort_ptr bubble_sort = (sort_ptr)dlsym(handle, "bubble_sort"); 
    //or selection sort: sort_ptr selection_sort = (sort_ptr)dlsym(handle, "selection_sort")
    bubble_sort(arr, n); 

    for(int i = 0; i < n; i++){
        printf("%.2f ", arr[i]);
        if(i == n - 1){
            printf("\n");
        }
    }

    dlclose(handle); 
    free(arr); 


    return 0; 
}