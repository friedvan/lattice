
#include "random.h"
//#include "visual.h"

node *A, *B;//[N][N], B[N][N];
stack *sps;

void clear(node *G)
{
	for (int k = 0; k < N; k++) {
		//G[k].inter = k;
		G[k].inter = -1;			//not defaultly set
		G[k].alive = true;
		G[k].cluster = 0;
		G[k].type = MONOMER;
		//stack_clear(G[k].base);
	}
}

node* init()
{
	node *G = (node*)malloc(sizeof(node)* N);
	if (G == NULL) {
		printf("memory error");
		exit(0);
	}
	for (int k = 0; k < N; k++)
		G[k].base = stack_init();
	clear(G);
	return G;
}

void release(node *G)
{
	for (int k = 0; k < N; k++)
		stack_release(G[k].base);
	free(G);
}							  		  

//ER graph with N nodes and connection probablity p
void ER(node *G, double p)
{
	double r = 0.0;
	for (int i = 0; i < N; i++) {
		//time_t tr = 0, tp = 0;
		for (int j = i + 1; j < N; j++) {
			//time_t t3, t2, t1 = clock();
			r = (double)rand() / RAND_MAX;
			//t2 = clock();
			//tr += t2 - t1;
			//rand();
			//printf("%f\n", r);
			if (r < p) {
				stack_push(G[i].base, j);
				stack_push(G[j].base, i);
			}
/*			t3 = clock();
			tp += t3 - t2;   */			
		}
		//printf("%I64d\t%I64d\n", tr, tp);
	}
}
void shuffle(int* random_list, int length)
{
	int temp;
	int i, j;

	//srand(time(NULL));
	for (i=0; i<length; i++) {
		j = rand() % length;
		temp = random_list[i];
		random_list[i] = random_list[j];
		random_list[j] = temp;
	}
}

int get_rand_list(int** plist, double c)
{
	bool *rand_flag = (bool *)malloc(sizeof(bool)* N);
	int rand_count = 0;
	for (int j = 0; j < N; j++) {
		double r = (double)rand() / RAND_MAX;
		if (r < c) {
			rand_flag[j] = true;
			rand_count++;
		}
		else {
			rand_flag[j] = false;
		}
	}
	*plist = (int*)malloc(sizeof(int) * rand_count * 2);//rand_list and rand_copy

	if (*plist == NULL)
		return 0;

	int k = 0;	
	for (int j = 0; j < N; j++) {
		if (rand_flag[j])
			(*plist)[k++] = j;
	}
	free(rand_flag);
	return rand_count;
}

void release(int *list)
{
	if (list != NULL) {
		free(list);
		list = NULL;
	}
}

gcsize get_size(node *G)
{
	gcsize s;
	memset(&s, 0, sizeof(s));

	for (int j = 0; j < N; j++) {
		if (G[j].alive) {
			s.maxsize++;
			if (G[j].type == MONOMER)
				s.monosize++;
			else
				s.dimersize++;
		}
	}
	return s;
}

void couple(int* rand_list, int* rand_copy, int rand_len)
{
	for (int i=0; i<rand_len; i++) {
		int pta, ptb;
		pta = rand_list[i];
		ptb = rand_copy[i];
		A[pta].inter = ptb;
		A[pta].type = DIMER;
		B[ptb].inter = pta;
		B[ptb].type = DIMER;
	}
}

void init_attack(double p)
{
	srand(rand());

	for (int j = 0; j < N; j++) {
		double r = (double)rand() / RAND_MAX;
		if (r < p) {
			A[j].alive = false;
			if (A[j].inter != -1)	   //if has interdependent node
				B[A[j].inter].alive = false;
		}
		//img_print(A, true);
		//img_print(B, false);
	}
}


int dfs(node *G, int pt, int lable)
{
	int size = 1;
	stack_clear(sps);
	stack_push(sps, pt);

	while (stack_len(sps) != 0) {
		node *top = G + *(sps->tail);
		if (!top->alive)
			continue;
		top->cluster = lable;
		bool changed = false;
		for (int i=0; i<stack_len(top->base); i++) {
			node *neighbor = G + top->base->head[i];
			if(neighbor->alive && neighbor->cluster == 0) {//alive and not visited
				neighbor->cluster = lable;
				size++;
				stack_push(sps, top->base->head[i]);
				changed = true;
			}
		}
		if (!changed)
			stack_pop(sps);
	}
	return size;
}
void gaint_component(node *G1, node *G2)
{
	int lable = 1;
	int maxsize = 0, maxcluster = -1, size = 0;//, monosize=0, dimersize=0;

	for (int j = 0; j<N && size <= N / 2 + 1; j++) {
		if (G1[j].alive && G1[j].cluster == 0) {//alive and not visited.
			size = dfs(G1, j, lable);
			if (size > maxsize) {
				maxcluster = lable;
				maxsize = size;
			}
			lable++;
		}
	}


	for (int j = 0; j < N; j++) {
		if (G1[j].alive) {
			if (G1[j].cluster != maxcluster){
				G1[j].alive = false;
				if (G1[j].inter != -1)
					G2[G1[j].inter].alive = false;
				//img_print(A, true);
				//img_print(B, false);
			}
			G1[j].cluster = 0;
		}
	}
}

void avg_degree(node *G)
{
	int sum = 0;
	for (int i = 0; i < N; i++) {
		sum += stack_len(G[i].base);
	}
	printf("%f\n", (double)sum / N);
}
int main()
{
	srand(time(NULL));
	FILE *fp = fopen("data/result.dat", "w");
	
	A = init();
	B = init();

	sps = stack_init();

	//ER network with probability p
	//!!!!!!!!for speed consideration, I moved it out of loop.
	//However, random network should regenerate every relazation
	ER(A, 2.5 / N);
	ER(B, 2.5 / N);
	//avg_degree(A);

	for (double c=0.5; c<=1.0; c+=0.1) {
		for (double p=0.31; p<=1.0; p+=0.01) {
			for (int k=0; k<NSAMPLE; k++) {
				gcsize s;
				int* rand_list = NULL;
				int* rand_copy = NULL;
				int rand_len = 0;

				clear(A);	
				clear(B);

				rand_len = get_rand_list(&rand_list, c);
				rand_copy = (int*)malloc(sizeof(int) * rand_len);
				memcpy(rand_copy, rand_list, sizeof(int) * rand_len); 
				shuffle(rand_copy, rand_len);
				couple(rand_list, rand_copy, rand_len);
				release(rand_copy);
				release(rand_list);
				
				init_attack(p);

				int pre_cluster_size = 0, cluster_size = 0;
				
				int iter = 0;
				gcsize s1, s2;
				memset(&s1, 0, sizeof(s1));
				memset(&s2, 0, sizeof(s2));
				while (1) {
					iter++;
					gaint_component(A, B);
					s=get_size(A);
					//img_print(A, true);
					cluster_size = s.maxsize;
					if (cluster_size == pre_cluster_size)
						break;
					pre_cluster_size = cluster_size;
					gaint_component(B, A);
					//img_print(B, false);
				}
				s=get_size(A);
				fprintf(fp, "%d\t%f\t%f\t%d\t%d\t%d\t%d\n", k, c, p, cluster_size, s.monosize, s.dimersize, iter);
			}
			printf("%.2f\t%.2f\n", c, p);
		}
	}

	release(A);
	release(B);
	stack_release(sps);
	fclose(fp);
}