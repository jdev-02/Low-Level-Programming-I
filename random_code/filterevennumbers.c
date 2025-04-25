#include <stdlib.h>
#include <stdio.h>
#include <errno.h>

int *filter_even_num(int*array,int original_size, int* new_size);

int main() {
    int arr[] = {1,2,3,4,5,6};
    int result_size;

    int *result = filter_even_num(arr,6,&result_size);
    for (int i=0;i<result_size;i++) {
        printf("%d\n",result[i]);
    }
    free(result);
    result = NULL;

    return 0;
}

int *filter_even_num(int *array,int original_size, int*new_size) {
    int even_count = 0;
    for (int i=0;i<original_size;i++) {
        if (array[i] % 2 == 0) {
            even_count ++;
        }
    }
    *new_size = even_count;

    int *even_array = malloc(sizeof(int) *even_count);
    errno = 0;
    if (even_array == NULL || errno !=0) {
        perror("error with malloc");
        return NULL;
    }

    int j = 0;
    for (int i =0;i<original_size;i++) {
        if (array[i] %2 == 0) {
            even_array[j] = array[i];
            j++;
        }
    }
    return even_array;

}
