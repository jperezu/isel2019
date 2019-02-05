#include <stdio.h>
#include <stdlib.h>

//Define total number of rows to calculate
#define N_ROWS 7

int n_elements_calculator(int n_rows);
void fill_pyramid(int * pnumbers, int n_elements);
void print_pyramid(int * pnumbers, int n_elements);


int main() {
    int * pnumbers;
    int n_elements;

    n_elements = n_elements_calculator(N_ROWS);
    pnumbers = (int *) malloc(n_elements * sizeof(int));

    fill_pyramid(pnumbers, n_elements);
    print_pyramid(pnumbers, n_elements);
    free(pnumbers);

    return 0;
}

//Calculates total number of elements for the number of rows defined
int n_elements_calculator(int n_rows){
    int i;
    int n_elements = 0;

    for (i = 0; i <= n_rows; i++){
        n_elements += i;
    }
    return n_elements;
}

//Writes the values in memory
void fill_pyramid(int * pnumbers, int n_elements){
    int i;
    int row_elem = 1;
    int current_row = 1;

    for (i = 0; i < n_elements; i++){
        if (row_elem == current_row){
            *(pnumbers + i) = 1;
            row_elem = 1;
            current_row ++;
        }
        else if (row_elem == 1) {
            *(pnumbers + i) = 1;
            row_elem ++;
        }
        else {
            *(pnumbers + i) = *(pnumbers + i - current_row) + *(pnumbers + i + 1 - current_row);
            row_elem ++;
        }

    }
}

//Print the results in a pyramid shape
void print_pyramid(int * pnumbers, int n_elements){
    int i;
    int row_elem = 1;
    int current_row = 1;

    for (i = 0; i < n_elements; i++) {
        printf("%d ", pnumbers[i]);

        if (row_elem == current_row){
            printf("\n");
            row_elem = 1;
            current_row ++;
        }
        else {
            row_elem ++;
        }
    }
}
