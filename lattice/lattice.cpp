#include "lattice.h"

node *A, *B;//[N][N], B[N][N];

node* init()
{
	node *G = (node*)malloc(sizeof(node) * N * N);
	for (int i=0; i<N; i++)
		for (int j=0; j<N; j++) {
			//G[i * N + j].id.x = i;
			//G[i * N + j].id.y = j;
			G[i * N + j].inter.x = i;
			G[i * N + j].inter.y = j;
			G[i * N + j].base[0].x = (N + i - 1) % N;
			G[i * N + j].base[0].y = j;
			G[i * N + j].base[1].x = (N + i + 1) % N;
			G[i * N + j].base[1].y = j;
			G[i * N + j].base[2].x = i;
			G[i * N + j].base[2].y = (N + j - 1) % N;
			G[i * N + j].base[3].x = i;
			G[i * N + j].base[3].y = (N + j + 1) % N;
			G[i * N + j].alive = true;
			G[i * N + j].cluster = 0;
			//G[i * N + j].gaint = 0;
		}
	return G;
}

void shuffle(point* random_list, int length)
{
	point temp;
	int i, j;

	srand(time(NULL));
	for (i=0; i<length; i++) {
		j = rand() % length;
		temp = random_list[i];
		random_list[i] = random_list[j];
		random_list[j] = temp;
	}
}

int get_rand_list(point** plist, float p)
{
	bool rand_flag[N*N];
	int i, j, k=0, rand_count=0;

	srand(time(NULL));
	for (i=0; i<N; i++) {
		for (j=0; j<N; j++) {
			float r = float(rand()) / RAND_MAX;
			if (r <= p) {
				rand_flag[i*N+j] = true;
				rand_count ++;
			} else rand_flag[i*N+j] = false;
		}
	}
	*plist = (point*)malloc(sizeof(point) * rand_count * 2);//rand_list and rand_copy

	if (*plist == NULL)
		return 0;

	for (i=0; i<N; i++) {
		for (j=0; j<N; j++) {
			if (rand_flag[i*N+j]) {
				(*plist)[k].x = i;
				(*plist)[k].y = j;
				k++;
			}
		}
	}
	return rand_count;
}

void release(point *list)
{
	if (list != NULL) {
		free(list);
		list = NULL;
	}
}

void network(point* rand_list, point* rand_copy, int rand_len)
{
	for (int i=0; i<rand_len; i++) {
		point pta, ptb;
		pta = rand_list[i];
		ptb = rand_copy[i];
		A[pta.x * N + pta.y].inter = ptb;
		B[ptb.x * N + ptb.y].inter = pta;
	}
}

void init_attack(float p)
{
	srand(time(NULL));
	for (int i=0; i<N; i++)
		for (int j=0; j<N; j++) {
			float r = float(rand()) / RAND_MAX;
			if (r <= p) {
				A[i*N+j].alive = false;
				point inter = A[i*N+j].inter;
				B[inter.x*N+inter.y].alive = false;
			}
		}
}

int dfs_recursive(node *G, point pt, int lable)
{
	int size = 0;
	node root = G[pt.x*N+pt.y];
	if (!root.alive)
		return 0;
	for (int i=0; i<LATTICE; i++)
	{
		root.cluster = lable;
		if (G[root.base[i].x * N + root.base[i].y].alive)
			size += dfs_recursive(G, root.base[i], lable);
	}
	return size + 1;
}

int dfs(node *G, point pt, int lable)
{
	int size = 1;

	stack st, *ps=&st;
	stack_init(ps);
	stack_push(ps, pt);

	while (stack_len(ps) != 0)
	{
		point tmp = *(ps->tail);
		node *top = G+tmp.x*N+tmp.y;
		if (!top->alive)
			continue;
		top->cluster = lable;
		bool changed = false;
		for (int i=0; i<LATTICE; i++) {
			node *neighbor = G + top->base[i].x * N + top->base[i].y;
			if(neighbor->alive && neighbor->cluster == 0) {//alive and not visited
				neighbor->cluster = lable;
				size++;
				if (size==5000)
					printf("");
				stack_push(ps, top->base[i]);
				changed = true;
			}
		}
		if (!changed)
			stack_pop(ps);
	}

	stack_release(ps);
	return size;
}
int gaint_component(node *G1, node *G2)
{
	int lable = 1;
	point pt;
	int maxsize = 0, maxcluster = 0, size = 0;
	for (int i=0; i<N; i++)
		for (int j=0; j<N; j++) {
			pt.x = i;
			pt.y = j;
			if (G1[i*N+j].alive && G1[i*N+j].cluster == 0) {//alive and not visited.
				size = dfs(G1, pt, lable);
				if (size > maxsize) {				
					maxcluster = lable;
					maxsize = size;
					//printf("%d\t%d\n", maxcluster, maxsize);
				}
				lable++;
			}
		}

	for (int i=0; i<N; i++)
		for (int j=0; j<N; j++) {
			if (G1[i*N+j].cluster != maxcluster){
				G1[i*N+j].alive = false;
				point inter = G1[i*N+j].inter;
				G2[inter.x*N+inter.y].alive = false;
			}
			G1[i*N+j].cluster = 0;
		}


	return maxsize;
}

int main()
{
	point* rand_list = NULL;
	point* rand_copy = NULL;
	int rand_len = 0;
	float c, p;
	FILE *fp = fopen("data/result.dat", "w");

	for (c=0.0; c<1.1; c+=0.1) {
		for (p=0.01; p<=1.00; p+=0.01) {
			for (int k=0; k<NSAMPLE; k++) {
				A = init();
				B = init();

				rand_len = get_rand_list(&rand_list, c);
				rand_copy = (point*)malloc(sizeof(point) * rand_len);
				memcpy(rand_copy, rand_list, sizeof(point) * rand_len); 
				shuffle(rand_copy, rand_len);
				network(rand_list, rand_copy, rand_len);
				release(rand_copy);
				release(rand_list);

				init_attack(p);

				int pre_cluster_size = 0, cluster_size = 0;
				int iter = 0;
				while (1) {
					iter++;
					cluster_size = gaint_component(A, B);
					if (cluster_size == pre_cluster_size)
						break;
					pre_cluster_size = cluster_size;
					gaint_component(B, A);

				
				}
				//printf("%d\t%.1f\t%.2f\t%d\t%d\n", k, c, p, cluster_size, iter);
				fprintf(fp, "%d\t%.1f\t%.2f\t%d\t%d\n", k, c, p, cluster_size, iter);
				free(A);
				free(B);
			}
			printf("%.1f\t%.2f\n", c, p);
		}
	}
	fclose(fp);
}