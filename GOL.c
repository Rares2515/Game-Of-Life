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

//Task 4

typedef struct{
    int row,col;
}Coord;


typedef struct {
    int** adj;
    Coord* coords;
    int size;
} Graph;

int is_near(Coord a, Coord b)
{
    if(abs(a.row-b.row)<=1 && abs(a.col-b.col)<=1 && !(a.row == b.row && a.col == b.col)) return 1;
    return 0;
}

int compare_paths(const Coord* coords, const int* path1, int len1,const int* path2, int len2) {
    int min_len = (len1 < len2) ? len1 : len2;
    for (int i = 0; i < min_len; i++) {
        Coord c1 = coords[path1[i]];
        Coord c2 = coords[path2[i]];

        if (c1.row < c2.row) return -1;
        if (c1.row > c2.row) return 1;

        if (c1.col < c2.col) return -1;
        if (c1.col > c2.col) return 1;
    }
    if (len1 < len2) return -1;
    if (len1 > len2) return 1;

    return 0;
}

Graph* create_graph(char** table, int n, int m)
{
    Graph* g=(Graph*)malloc(sizeof(Graph));
    g->coords=(Coord*)malloc(n*m*sizeof(Coord));
    
    int number_of_cells=0;

    for(int i=0;i<n;i++)
        for(int j=0;j<m;j++)
            if(table[i][j]==ALIVE){
                g->coords[number_of_cells].row=i;
                g->coords[number_of_cells].col=j;
                number_of_cells++;
            }

    g->size=number_of_cells;
    g->adj=(int**)malloc(g->size * sizeof(int*));
    for (int i =0; i<g->size; i++) {
        g->adj[i]=(int*)calloc(g->size, sizeof(int));
    }

    for(int i=0; i<g->size; i++)
        for(int j=i+1; j<g->size; j++)
            if(is_near(g->coords[i],g->coords[j])){
                g->adj[i][j]=1;
                g->adj[j][i]=1;
            }

    return g;
}

int in_component(int node, const int* component, const int comp_size) {
    for (int i=0; i<comp_size; i++)
        if (component[i]==node) return 1;
    return 0;
}

void dfs_component(Graph* g, int node, int* visited, int* component, int* size) {
    visited[node]=1;
    component[(*size)++]=node;

    for (int i=0; i<g->size; i++) {
        if (g->adj[node][i] && !visited[i]) {
            dfs_component(g, i, visited, component, size);
        }
    }
}

void sort_neighbors(int* neighbors, int count, const Graph* g) {
    for (int i=0; i<count-1; i++) {
        for (int j=i+1; j<count; j++) {
            int na=neighbors[i], nb=neighbors[j];
            if (g->coords[nb].row < g->coords[na].row || (g->coords[nb].row == g->coords[na].row && g->coords[nb].col < g->coords[na].col)) {
                int temp=neighbors[i];
                neighbors[i]=neighbors[j];
                neighbors[j]=temp;
            }
        }
    }
}

void dfs(Graph* g, int* visited, int current, int* nodes_path, int depth, int comp_size, int* best_length, int* best_path, int* component) {
    if (visited[current]) return;

    visited[current]=1;
    nodes_path[depth]=current;

    if (depth+1>*best_length || (depth+1 == *best_length && compare_paths(g->coords, nodes_path, depth+1, best_path, *best_length) < 0)) {
        *best_length = depth+1;
        for (int i=0; i<=depth; i++) {
            best_path[i]=nodes_path[i];
        }
    }
    if (depth+1 == comp_size) {
        visited[current]=0;
        return;
    }

    int* neighbors=malloc(g->size * sizeof(int));
    int neighbors_count=0;
    for (int i=0; i<g->size; i++) {
        if (g->adj[current][i] && !visited[i] && in_component(i, component, comp_size)) {
            neighbors[neighbors_count++]=i;
        }
    }

    sort_neighbors(neighbors,neighbors_count,g);

    for (int i=0; i<neighbors_count; i++) {
        dfs(g, visited, neighbors[i], nodes_path, depth+1, comp_size, best_length, best_path, component);
    }

    free(neighbors);
}





void longest_path(Graph* g, FILE* out, int* max_length, int best_path[], int* best_length) {
    *max_length=0;
    *best_length=0;

    if (g->size==0) {
        fprintf(out, "-1\n");
        return;
    }

    int* visited_global=(int*) malloc(g->size*sizeof(int));
    int* component =(int*)malloc(g->size*sizeof(int));
    int* local_path =(int*)malloc(g->size*sizeof(int));
    int* local_best =(int*)malloc(g->size*sizeof(int));


    for (int i=0; i<g->size; i++) {
        visited_global[i]=0;
        component[i]=0;
        local_path[i]=0;
        local_best[i]=0;
    }


    for (int i=0; i<g->size; i++) {
        if (!visited_global[i]) {
            int comp_size=0;
            dfs_component(g, i, visited_global, component, &comp_size);


            for (int j=0; j<comp_size; j++) {
                int* local_visited=calloc(g->size, sizeof(int));
                int local_best_len=0;

                dfs(g, local_visited, component[j], local_path, 0, comp_size, &local_best_len, local_best, component);

                if (local_best_len > *best_length || (local_best_len == *best_length && compare_paths(g->coords, local_best, local_best_len, best_path, *best_length) < 0)) {
                    *best_length = local_best_len;
                    *max_length = local_best_len;
                    for (int k=0; k<local_best_len; k++) {
                        best_path[k]=local_best[k];
                    }
                }

                free(local_visited);
            }
        }
    }

    if (*max_length==0) {
        fprintf(out, "-1\n");
    } else {
        fprintf(out, "%d\n", *max_length - 1);
        for (int i=0; i<*best_length; i++) {
            Coord c=g->coords[best_path[i]];
            fprintf(out, "(%d,%d) ", c.row, c.col);
        }
        fprintf(out, "\n");
    }

    free(visited_global);
    free(component);
    free(local_path);
    free(local_best);
}

void free_graph(Graph* g, int n) {
    for (int i=0; i<g->size; i++) {
        free(g->adj[i]);
    }
    free(g->adj);
    free(g->coords);
    free(g);
}

Node_tree* create_tree_graph(char** table, int n, int m, int k, FILE* output_file) {
    //printf("Entering create_tree_graph with k=%d\n", k);
    if (k<0) {
        printf("k < 0, return NULL\n");
        return NULL;
    }

    // Alocarea tabelului root_table
    char** root_table = aloc_table(n, m);
    if (root_table == NULL) {
        printf("Eroare la alocarea root_table\n");
        return NULL;
    }
    copy_table(table, root_table, n, m);

    // Crearea nodului root
    Node_tree* root = createNode(root_table);
    //printf("Created node for root\n");

    // Crearea grafului
    Graph* g = create_graph(root_table, n, m);
    //printf("Graph created\n");

    int max_length = -1, best_length;
int* best_path = malloc(g->size * sizeof(int));


    longest_path(g, output_file, &max_length, best_path, &best_length);
    //printf("Longest path computed: max_len = %d\n", max_length);
    free_graph(g, n);
    //printf("Graph processed\n");

    // Crearea subarborelui stâng
    char** left_table = tree_rule_left(root_table, n, m, root);
    //printf("Left table created\n");
    root->left = create_tree_graph(left_table, n, m, k - 1, output_file);
    free_table(left_table, n);

    // Crearea subarborelui drept
    char** right_table = tree_rules_right(root_table, n, m, root);
    //printf("Right table created\n");
    root->right = create_tree_graph(right_table, n, m, k - 1, output_file);
    free_table(right_table, n);

    // Eliberarea memoriei pentru root_table
    free_table(root_table, n);
    //printf("Memory for root_table freed\n");
    free(best_path);

    return root;
}


//BONUS TASK 2
void apply_reverse_changes(char** table, Node* changes) {
    while (changes != NULL) {
        int i = changes->val.row;
        int j = changes->val.col;

        if (table[i][j] == ALIVE) {
            table[i][j] = DEAD;
        }else {
            table[i][j] = ALIVE;
}

        changes = changes->next;
    }
}

void reconstruct_generation_0(char** table, int n, int m, Node* gen_stack) {
    Node* current = gen_stack;
    while (current != NULL) {
        apply_reverse_changes(table, current);
        current = current->next;
    }
}

Node* read_stack_of_lists(FILE* input, int* last_gen) {
    Node* top = NULL;
    int k, row, col;

    while (fscanf(input, "%d", &k) == 1) {
        *last_gen = k;
        Node* gen_changes = NULL;

        while (fscanf(input, "%d %d", &row, &col) == 2) {
            Change c = {row, col};
            Node* new_node = (Node*)malloc(sizeof(Node));
            new_node->val = c;
            new_node->next = gen_changes;
            gen_changes = new_node;
        }

        Node* new_stack = (Node*)malloc(sizeof(Node));
        new_stack->val = gen_changes->val;
        new_stack->next = top;
        top = new_stack;
    }

    return top;
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
    if(T==4){
        Node_tree* root = create_tree_graph(table, N, M, K, output_file);
        free_tree(root, N);
    }
    if(T==777){
        Node* bonus_top = NULL; // Inițializezi stiva
        int gen, row, col;
        fscanf(input_file, "%d", &gen); // Citește generația (de ex. 15)
        while (fscanf(input_file, "%d %d", &row, &col) == 2) {
            Change ch = { row, col };
            push(&bonus_top, ch);
            }


        Node* gen_stack = read_stack_of_lists(bonus_top, &K);
        reconstruct_generation_0(table, N, M, gen_stack);
        print_table(output_file, table, N, M);
    }
    




    deleteStack(&top);


	fclose(input_file);
	fclose(output_file);
	free_table(table,N);


return 0;
}