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



struct N { 
	//char** table;
    //int i,j;
    Node* table_changes;
	struct  N  *left,*right; 
}; 
typedef struct N Node_tree;


void copy_table(char** source, char** destination , int n, int m)
{
    for(int i=0; i<n; i++)
        for(int j=0; j<m; j++)
            destination[i][j]=source[i][j];
}




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

    Change* changes = (Change*) malloc(n * m * sizeof(Change));  // Max number of changes
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

    copy_table(new_table, table, n, m);
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

void free_tree(Node_tree* root, int n) {
    if (root == NULL) return;
    //free_table(root->table, n);

    deleteStack(&(root->table_changes));
    free_tree(root->left, n);
    free_tree(root->right, n);

    free(root);
}


Node_tree* createNode(char** table){
    Node_tree* newNode = (Node_tree*)malloc(sizeof(Node_tree));
    //newNode->table = table;
    newNode->left = newNode->right = NULL;
    newNode->table_changes = NULL;
    return newNode;
}

char** tree_rule_left(char** table, int n, int m, Node_tree* root) {
    char** new_table = aloc_table(n, m);

    for (int i = 0; i < n; i++) {
        for (int j = 0; j < m; j++) {
            int nralive = count(table, n, m, i, j);

            if (table[i][j]==DEAD && nralive == 2) {
                new_table[i][j] = ALIVE;


                Node* new_change = malloc(sizeof(Node));
                new_change->val.row = i;
                new_change->val.col = j;
                new_change->next = root->table_changes;
                root->table_changes = new_change;
            }
            else new_table[i][j]=table[i][j];
        }
 
    }
    return new_table;
}




char** tree_rules_right(char** table, int n, int m, Node_tree* root) {
    char** new_table = aloc_table(n, m);

    for(int i = 0; i < n; i++) {
        for(int j = 0; j < m; j++) {
            int alive = count(table, n, m, i, j);

            if(table[i][j] == ALIVE) {
                if(alive < 2 || alive > 3)
                    {new_table[i][j] = DEAD;

                    Node* new_change = malloc(sizeof(Node));
                    new_change->val.row = i;
                    new_change->val.col = j;
                    new_change->next = root->table_changes;
                    root->table_changes = new_change;
                    }
                else
                    new_table[i][j] = ALIVE;
            } else {
                if(alive == 3){
                    new_table[i][j] = ALIVE;

                    Node* new_change = malloc(sizeof(Node));
                    new_change->val.row = i;
                    new_change->val.col = j;
                    new_change->next = root->table_changes;
                    root->table_changes = new_change;
            }
                else
                    new_table[i][j] = DEAD;
            }
        }
    }

    return new_table;
}

Node_tree* create_tree(char** table, int n, int m, int k, FILE* output_file) {
    if(k < 0) {
        return NULL;
    }

    char** root_table = aloc_table(n, m);
    copy_table(table, root_table, n, m);

    Node_tree* root = createNode(root_table);
    print_table(output_file, root_table, n, m);
    fprintf(output_file, "\n");

    char** left_table = tree_rule_left(root_table, n, m, root);
    root->left = create_tree(left_table, n, m, k - 1, output_file);
    free_table(left_table, n);


    char** right_table = tree_rules_right(root_table, n, m, root);
    root->right = create_tree(right_table, n, m, k - 1, output_file);
    free_table(right_table, n);
    free_table(root_table, n);


    return root;
}



// Parcurgere în preordine - recursivă
void preorder(Node_tree* root, FILE* output, int n, int m) {
    if (root) {
        //printf("Current table at this node:\n");
        //print_table(output, root->table, n, m);
        fprintf(output, "\n");
        preorder(root->left, output, n,m);         // 2. Parcurge subarborele stâng
        preorder(root->right, output, n,m);        // 3. Parcurge subarborele drept
    }
}
/*
void lenght(Node_tree* root,char** table,int n, int m, int *l,int *x, int *y)
{
    int i,j;
    *l=0;
    int l_aux;
    for(i=0;i<n;i++)
        for(j=0;j<m;j++)
    {
            l_aux=0;
            int number_of_cells=0;
            int number_of_connections=0;
            while(table[i][j]==ALIVE)
            {
                connections=count(table, n, m, i, j);
                

                int aux_i=i,aux_j=j;
                l_aux++;
                number_of_cells++;
            }
            for(x=aux_i;x<=l_aux;x++)
                for(y=aux_j;y<l_aux;y++){
                    int connections=count(table, n, m, i, j);
                    if(connections<number_of_cells/2) l_aux=0;
                }
            if(l_aux>*l) {
                *l = l_aux;  
                *x=aux_i;
                *y=aux_j;
            }              
    }
}

void print_lenght(*Node_tree root, int l, int x, int y, int n, int m, *FILE output)
{
    int l_aux=l;
    fprintf(output,"%d\n",l);
    while(l_aux>0){
    for(int i=x;i<n;i++)
        for(int j=y;j<m;j++){
            fprintf(output,"(%d,%d) ",i,j);
            l_aux--;
        }
    }
}

*/

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


    //Task 2
    Node* top=NULL;

    int K_aux=K;
	while(K>0 &&(T==1 || T==2))
	{
		rules(table,N,M,&top);
        if(T==2){
            print_stack(output_file,top,K_aux-K+1);
            fprintf(output_file,"\n");

        }
        else if(T==1){
		    print_table(output_file,table,N,M);
            fprintf(output_file,"\n");
        }

		K--;
	}
    if (T == 3) {
        //char** copy = aloc_table(N, M);
        //copy_table(table, copy, N, M);
        //Node_tree* root = create_tree(copy, N, M, K, output_file);
        Node_tree* root = create_tree(table, N, M, K, output_file);
        //preorder(root, output_file, N, M);
        free_tree(root, N);
        //free_table(copy, N);
    }
    




    deleteStack(&top);


	fclose(input_file);
	fclose(output_file);
	free_table(table,N);


return 0;
}