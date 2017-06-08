#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

#define INF 1e8
#define index(i, j) (n * i) + j

int n;//矩阵行列数
int rank, size;
int *dists;
int *local;
int *counts;
int *zeroes;
int *rowK;

void load();
void setupMaster();
void goMasterNode();
void goNode();
void doPrint();

int main( int argc, char *argv[]) {
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (rank == 0) {
        goMasterNode();
        doPrint();
    } else
        goNode();

    MPI_Finalize();
	return 0;
}

void load() {
    int i, j, tmp;
    char name[10];
    printf("input file: ");
    scanf("%s", name);
    freopen(name, "r", stdin);
    scanf("%d", &n);
    dists = (int *) malloc(sizeof(int) * n * n);
    for (i = 0; i < n; ++i) {
        for (j = 0; j < n; ++j) {
            scanf("%d", &tmp);
            if (tmp == -1) tmp = INF;
            dists[index(i, j)] = tmp;
        }
    }
}

void setupMaster() {
    load();
	int rowsPerProc=n/size;//每个进程的行数
	int	i, start;

    counts = (int*) malloc(sizeof(int) * size);
    zeroes = (int*) malloc(sizeof(int) * size);
    local  = (int*) malloc(sizeof(int) * rowsPerProc * n);
    start = 0;
    for (i = 0; i <size; i++) {
        counts[i] = rowsPerProc;
        counts[i] *= n;
        zeroes[i] = start;
        start = ((i + 1) * rowsPerProc * n);
    }
    if (n % size != 0) {
        counts[size - 1] += (n % size) * n;
    }
}

void goMasterNode() {
    int i, j, k, lb, ub, owner, rowsPerProc, me, *rowKSpace;
    setupMaster();

    rowsPerProc = (n / size);
    me = rowsPerProc;
    if ((n % size != 0) && (me == size - 1)) {
        me += (n % size);
    }
    lb = (rowsPerProc) * rank;
    ub = (rowsPerProc) * (rank + 1) - 1;
    local  = (int*) malloc(sizeof(int) * me * n);
    rowKSpace = (int *) malloc(sizeof(int) * n);


    MPI_Bcast(&n, 1, MPI_INT, 0, MPI_COMM_WORLD);

    MPI_Scatterv(dists, counts, zeroes,
                 MPI_INT, local, counts[0],
                 MPI_INT, 0, MPI_COMM_WORLD);

    for (k = 0; k < n; ++k) {
        owner = k / (rowsPerProc);
        if (owner >= size) owner = size - 1;

        if (owner == rank)
            rowK = &local[(k - lb) * n];
        else
            rowK = rowKSpace;

        MPI_Bcast(rowK, n, MPI_INT, owner, MPI_COMM_WORLD);

        for (i = 0; i < me; ++i) {
            for (j = 0; j < n; ++j) {
                if (local[index(i, k)] + rowK[j] < local[index(i, j)])
                    local[index(i, j)] = local[index(i, k)] + rowK[j];
            }
        }

    }
    free(rowKSpace);

    MPI_Gatherv(local, counts[0], MPI_INT,
                dists, counts, zeroes,
                MPI_INT, 0, MPI_COMM_WORLD);

    free(local);


}

void goNode() {
    int rowsPerProc, i, j, k, me, lb, ub, owner, *rowKSpace;

    MPI_Bcast(&n, 1, MPI_INT, 0, MPI_COMM_WORLD);

    rowsPerProc = (n / size);
    me = rowsPerProc;
    if ((n % size != 0) && (rank == size - 1)) {
        me += (n % size);
    }

    local  = (int*) malloc(sizeof(int) * me * n);

    MPI_Scatterv(NULL, NULL, NULL,
                 MPI_INT, local, me * n,
                 MPI_INT, 0, MPI_COMM_WORLD);

    lb = (n / size) * rank;
    ub = (n / size) * (rank + 1) - 1;


    rowKSpace = (int *) malloc(sizeof(int) * n);

    for (k = 0; k < n; ++k) {
        owner = k / (rowsPerProc);
        if (owner >= size) owner = size - 1;

        if (owner == rank)
            rowK = &local[(k - lb) * n];
        else
            rowK = rowKSpace;

        MPI_Bcast(rowK, n, MPI_INT, owner, MPI_COMM_WORLD);

        for (i = 0; i < me; ++i) {
            for (j = 0; j < n; ++j) {
                if (local[index(i, k)] + rowK[j] < local[index(i, j)])
                    local[index(i, j)] = local[index(i, k)] + rowK[j];
            }
        }
    }
    free(rowKSpace);

    MPI_Gatherv(local, me * n, MPI_INT,
                NULL, NULL, NULL,
                MPI_INT, 0, MPI_COMM_WORLD);

    free(local);

}

void doPrint() {
    int i, j;
    for (i = 0; i < n; ++i) {
        for (j = 0; j < n; ++j) {
            printf("%d ", dists[index(i, j)]);
        }
        printf("\n");
    }
}
