#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#define ALIVE 'X'
#define DEAD '+'
#define Data int



struct Change {
    int row;
    int col;
};

typedef struct Change Change;

struct Elem {
	Change val;
	struct Elem* next;
};

typedef struct Elem Node;


void push(Node** top, Change v) {
    Node* newNode = (Node*)malloc(sizeof(Node));
    newNode->val = v;
    newNode->next = *top;
    *top = newNode;
}



// Șterge întreaga stivă și eliberează memoria
void deleteStack(Node** top) {

    while ((*top) != NULL) {  // echivalent cu !isEmpty(*top)
        Node* temp;
        temp = *top;
        *top = (*top)->next;
        free(temp);
    }
}




char** aloc_table(int n, int m)
{
    char** table = (char**) malloc(n * sizeof(char*));
    if (table == NULL) {
        printf("Memory allocation failed for table rows.\n");
        exit(1);
    }
    for (int i = 0; i < n; i++) {
        table[i] = (char*) malloc(m * sizeof(char));
        if (table[i] == NULL) {
            printf("Memory allocation failed for table columns.\n");
            exit(1);
        }
    }
    return table;
}


void free_table(char** table, int n)
{
    for(int i = 0; i < n; i++) {
        if(table[i] != NULL) {
            free(table[i]);
        }
    }
    if(table != NULL) {
        free(table);
    }
}


void print_table(FILE *output_file, char** table, int n, int m)
{
	for(int i=0;i<n;i++){
		for(int j=0; j<m;j++){
			fprintf(output_file, "%c", table[i][j]);}
		fprintf(output_file,"\n");
	}

}


int count(char **table, int n, int m, int i, int j)
{
	int alivec=0;
	if(i>0 && j>0 && table[i-1][j-1] == ALIVE) alivec++;
	if(i>0 && table[i-1][j] == ALIVE) alivec++;
	if(i>0 && j<m-1 && table[i-1][j+1] == ALIVE) alivec++;
	if(j>0 && table[i][j-1] == ALIVE) alivec++;
	if(j<m-1 && table[i][j+1] == ALIVE) alivec++;
	if(i<n-1 && j>0 && table[i+1][j-1] == ALIVE) alivec++;
	if(i<n-1 && table[i+1][j] == ALIVE) alivec++;
	if(i<n-1 && j<m-1 && table[i+1][j+1] == ALIVE) alivec++;

	return alivec;
}


void rules(char** table, int n, int m, Node** top)
{
    int i, j;
    char** new_table = aloc_table(n, m);
    
    deleteStack(top);

    Change* changes = (Change*) malloc(n * m * sizeof(Change));  // Maximul posibil de schimbări
    int change_count=0;


    for(i = 0; i < n; i++) {
        for(j = 0; j < m; j++) {
            int nralive = count(table, n, m, i, j);
            
            if(table[i][j] == ALIVE) {
                if(nralive < 2 || nralive > 3) {
                    new_table[i][j] = DEAD;
                    changes[change_count].row=i;
                    changes[change_count].col=j;
                    change_count++;
                } else {
                    new_table[i][j] = ALIVE;
                }
            } else {
                if(nralive == 3) {
                    new_table[i][j] = ALIVE;
                    changes[change_count].row=i;
                    changes[change_count].col=j;
                    change_count++;
                } else {
                    new_table[i][j] = DEAD;
                }
            }
        }
    }

    for(i = 0; i < change_count; i++) {
        push(top, changes[i]);
    }

    for(i = 0; i < n; i++) {
        for(j = 0; j < m; j++) {
            table[i][j] = new_table[i][j];
        }
    }

    free_table(new_table, n);
    free(changes);
}


void print_stack(FILE *output_file, Node* top, int K)
{
    if(top == NULL){
        fprintf(output_file,"%d ",K);
        fprintf(output_file,"Stack is empty \n");
        exit(1);
    }

    fprintf(output_file,"%d",K);

    Change elements[1000];
    int count_stack=0;

    Node*temp=top;

    //using a tempory stack to keep the original order
    while (temp != NULL) {
        elements[count_stack++] = temp->val;
        temp = temp->next;
    }

    // Printing elements in revrese order to respect the stack's order
    for (int i = count_stack - 1; i >= 0; i--) {
        fprintf(output_file, " %d %d", elements[i].row, elements[i].col);
    }



    
}





int main(int argc, const char *argv[])
{

	if (argc < 3) {
        printf("Usage: %s <input_file> <output_file>\n", argv[0]);
        return 1; 
    }

    FILE *input_file = fopen(argv[1], "r");
    FILE *output_file = fopen(argv[2], "w");


    if (input_file == NULL || output_file == NULL) {
        printf("Could not open files.\n");
        if (input_file != NULL) fclose(input_file);
        if (output_file != NULL) fclose(output_file);
        return 1;
    }


	//TASK 1  
	int T,N,M,K;
	fscanf(input_file, "%d", &T);
	fscanf(input_file, "%d", &N);
	fscanf(input_file, "%d", &M);
	fscanf(input_file, "%d", &K);


	char **table = aloc_table(N, M);
	int i,j;

	for(i = 0; i < N; i++) {
		for(j = 0; j < M; j++) {
			fscanf(input_file, " %c", &table[i][j]);  
		}
	}
    
    if(T==1){
	print_table(output_file, table, N, M);
    fprintf(output_file, "\n");
    }



	fclose(input_file);
	fclose(output_file);
	free_table(table,N);


return 0;
}