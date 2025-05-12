#include <stdio.h>
#include <stdlib.h>
#include <omp.h>


void read_grid(const char *filename, int *rows, int *cols, char ***grid){
    FILE* file = fopen(filename, "r");
    if(!file){
        perror("error opening file");
        exit(EXIT_FAILURE);
    }

    /*getting dimensions*/
    fscanf(file, "%d %d\n", rows, cols);
    
    /*malloc for grid*/
    *grid = malloc(*rows * sizeof(char *));
    for(int i=0; i < *rows; i++){
        (*grid)[i] = malloc(*cols * sizeof(char));
    }

    /*create the grid from file*/
    char line[1024];
    for(int i=0; i< *rows; i++){
        fgets(line, sizeof(line), file);

        int colIndex = 0;
        for(int j=0; line[j] != '\0' && colIndex < *cols; j++){
            if(line[j] == '*' || line[j] == ' '){
                (*grid)[i][colIndex++] = line[j];
            }
        }  
    }
    fclose(file);
}

/*
---- thought process for the neighbours: -----
(x-1, y-1)  (x-1, y)  (x-1, y+1)
(x,   y-1)  ( x,  y)  (x,   y+1)
(x+1, y-1)  (x+1, y)  (x+1, y+1)

where (x,y) is the cell we are counting its live neighbours.
so instead of writing 8 separate if statements i use 2 arrays x and y to represent the directions
not counting myself as neighbour 
*/

int count_alive_neighbours(int row, int col, int rows, int cols, char **grid){
    int count = 0;
    int x[] = {-1, -1, -1, 0, 0, 1, 1, 1};
    int y[] = {-1, 0, 1, -1, 1, -1, 0, 1};
    
    #pragma omp simd reduction(+:count)
    for(int i=0; i < 8; i++){
        int newRow = row + x[i];
        int newCol = col + y[i];

        /*check if neighbour is within grid boundaries*/
        if(newRow >= 0 && newRow < rows && newCol >= 0 && newCol < cols){
            if(grid[newRow][newCol] == '*'){ /*if he is alive increase the alive counter*/
                count++;
            }
        }
    }
    return count;
}

void evolve(int rows, int cols, char **grid){
    /*we create a new grid to store the next generation, bc if we change the grid
    immediately it affects the cells we havent checked yet*/
    char **new_grid = malloc(rows * sizeof(char *));
    for(int i=0; i < rows; i++){
        new_grid[i] = malloc(cols * sizeof(char));
    }

    #pragma omp parallel for collapse(2)
    for(int i=0; i < rows; i++){
        for(int j=0; j < cols; j++){
            int alive_neighbours = count_alive_neighbours(i, j, rows, cols, grid);

            /*GAME OF LIFE RULES*/
            if(grid[i][j] == '*'){
                new_grid[i][j] = (alive_neighbours == 2 || alive_neighbours == 3) ? '*' : ' ';
            }else{
                new_grid[i][j] = (alive_neighbours == 3) ? '*' : ' ';
            }
        }
    }

    /*copy the new grid to the original one*/
    #pragma omp parallel for collapse(2)
    for(int i=0; i < rows; i++){
        for(int j=0; j < cols; j++){
            grid[i][j] = new_grid[i][j];
        }
    }
    /*free the new grid, we dont need it anymore*/
    #pragma omp parallel for
    for (int i = 0; i < rows; i++) {
        free(new_grid[i]); 
    }
    free(new_grid);

}

/*function to print the grid*/
void print_grid(int rows, int cols, char **grid) {
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            printf("%c ", grid[i][j]);
        }
        printf("\n");
    }
}

int main(int argc, char *argv[]) {
    if (argc != 4) {
        printf("it should be <input_file> <num_generations> <output_file>. try again:)\n");
        return EXIT_FAILURE;
    }

    int rows, cols;
    char **grid;
    int num_generations = atoi(argv[2]);
    const char *output = argv[3]; 
    double start_time, end_time;

    omp_set_num_threads(4);

    read_grid(argv[1], &rows, &cols, &grid);
    
    start_time = omp_get_wtime();
    /*run the game of life for the given number of generations*/
    for(int i=0; i < num_generations; i++){
        evolve(rows, cols, grid);
    }
    end_time = omp_get_wtime();
    double time = end_time - start_time;
    printf("time: %f seconds\n", time);

    /*write the output in the file*/
    FILE *output_file = fopen(output, "w");
    if(!output_file){
        perror("error opening output file");
        return EXIT_FAILURE;
    }

    /*appropriate format*/
    for(int i=0; i < rows; i++){
        for(int j=0; j < cols; j++){
            fprintf(output_file, "|%c", grid[i][j] == ' ' ? ' ' : '*');
        }
        fprintf(output_file, "|\n"); 
    }
    fclose(output_file);
    printf("results saved in output file all good!\n");

    /*frees*/
    for (int i = 0; i < rows; i++) {
        free(grid[i]);
    }
    free(grid);

    return 0;
}