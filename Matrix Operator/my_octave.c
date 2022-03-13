// Copyright Manea Dorin-Mihai 313CAb 2021-2022

#include <stdio.h>
#include <stdlib.h>

#define MOD 10007

typedef struct {
	int **a, m, n;
} matrix_t;

matrix_t *alloc_matrix(matrix_t *mat_lst, int size)
{
	matrix_t *mat;
	mat = (matrix_t *)realloc(mat_lst, (size + 1) * sizeof(matrix_t));
	if (!mat)
		return NULL;
	return mat;
}

int **new_matrix(int m, int n)
{
	int **a;
	a = malloc(m * sizeof(int *));
	if (!a)
		return NULL;
	for (int i = 0; i < m; i++) {
		a[i] = malloc(n * sizeof(int));
		if (!a[i]) {
			for (int j = 0; j < i; j++)
				free(a[j]);
			free(a);
			return NULL;
		}
	}
	return a;
}

void matrix_dim(matrix_t *mat_lst, int size, int index)
{
	if (index < 0 || index >= size) {
		printf("No matrix with the given index\n");
		return;
	}

	int m = mat_lst[index].m, n = mat_lst[index].n;
	printf("%d %d\n", m, n);
}

void print_matrix(matrix_t *mat_lst, int size, int index)
{
	if (index < 0 || index >= size) {
		printf("No matrix with the given index\n");
		return;
	}

	int m = mat_lst[index].m, n = mat_lst[index].n;
	for (int i = 0; i < m; i++) {
		for (int j = 0; j < n; j++)
			printf("%d ", mat_lst[index].a[i][j]);
		printf("\n");
	}
}

int **matrix_resize(matrix_t *mat_lst, int l, int *lin, int c, int *col,
					int index)
{
	int m = mat_lst[index].m;
	int **aux = new_matrix(l, c);
	if (!aux)
		return NULL;
	for (int i = 0; i < l; i++)
		for (int j = 0; j < c; j++)
			aux[i][j] = mat_lst[index].a[lin[i]][col[j]];
	for (int i = 0; i < m; i++)
		free(mat_lst[index].a[i]);
	free(mat_lst[index].a);

	return aux;
}

int **matrix_multiplication(matrix_t *mat_lst, int index, int index1)
{
	int m = mat_lst[index].m, n = mat_lst[index].n;
	int n1 = mat_lst[index1].n;
	int **a = (int **)calloc(m, sizeof(int *));
	if (!a)
		return NULL;
	for (int i = 0; i < m; i++) {
		a[i] = (int *)calloc(n1, sizeof(int));
		if (!a[i]) {
			for (int j = 0; j < i; j++)
				free(a[j]);
			free(a);
			return NULL;
		}
	}

	for (int i = 0; i < m; i++)
		for (int j = 0; j < n1; j++) {
			for (int k = 0; k < n; k++)
				a[i][j] += (mat_lst[index].a[i][k] *
							mat_lst[index1].a[k][j]) % MOD;
			a[i][j] %= MOD;
			if (a[i][j] < 0)
				a[i][j] += MOD;
		}
	return a;
}

void sort_matrix(matrix_t *mat_lst, int *sum, int size)
{
	int m, n;
	for (int i = 0; i < size; i++) {
		m = mat_lst[i].m;
		n = mat_lst[i].n;
		for (int j = 0; j < m; j++)
			for (int k = 0; k < n; k++) {
				sum[i] = (sum[i] + mat_lst[i].a[j][k] % MOD) % MOD;
				if (sum[i] < 0)
					sum[i] += MOD;
			}
	}

	for (int i = 0; i < size - 1; i++)
		for (int j = i + 1; j < size; j++)
			if (sum[i] > sum[j]) {
				int aux = sum[i];
				sum[i] = sum[j];
				sum[j] = aux;

				matrix_t tmp = mat_lst[i];
				mat_lst[i] = mat_lst[j];
				mat_lst[j] = tmp;
	}
}

int **matrix_transpose(matrix_t *mat_lst, int index)
{
	int m = mat_lst[index].m, n = mat_lst[index].n;
	int **aux = new_matrix(n, m);
	if (!aux)
		return NULL;
	for (int i = 0; i < m; i++)
		for (int j = 0; j < n; j++)
			aux[j][i] = mat_lst[index].a[i][j];
	for (int i = 0; i < m; i++)
		free(mat_lst[index].a[i]);
	free(mat_lst[index].a);
	return aux;
}

void remove_matrix(matrix_t *mat_lst, int *size)
{
	int index;
	scanf("%d", &index);
	if (index < 0 || index >= *size) {
		printf("No matrix with the given index\n");
		return;
	}
	int m = mat_lst[index].m;
	for (int i = 0; i < m; i++)
		free(mat_lst[index].a[i]);
	free(mat_lst[index].a);
	for (int i = index; i < *size - 1; i++)
		mat_lst[i] = mat_lst[i + 1];
	(*size)--;
}

void free_matrix(matrix_t *mat_lst, int size)
{
	for (int i = 0; i < size; i++) {
		int m = mat_lst[i].m;
		for (int j = 0; j < m; j++)
			free(mat_lst[i].a[j]);
		free(mat_lst[i].a);
	}
	free(mat_lst);
}

void L(matrix_t *mat_lst, int *size, int *size1, int *quit)
{
	int m, n;
	scanf("%d%d", &m, &n);
	mat_lst[*size].m = m;
	mat_lst[*size].n = n;

	int **tmp1 = new_matrix(m, n);
	if (!tmp1) {
		*quit = 1;	// error
		return;
	}
	mat_lst[*size].a = tmp1;
	for (int i = 0; i < m; i++)
		for (int j = 0; j < n; j++)
			scanf("%d", &mat_lst[*size].a[i][j]);
	(*size)++;
	if (*size > *size1)
		(*size1)++;
}

void C(matrix_t *mat_lst, int size, int *quit)
{
	int index;
	scanf("%d", &index);

	int l;
	scanf("%d", &l);
	int *lin = (int *)malloc(l * sizeof(int));
	if (!lin) {
		*quit = 1;	// error
		return;
	}
	for (int i = 0; i < l; i++)
		scanf("%d", &lin[i]);

	int c;
	scanf("%d", &c);
	int *col = (int *)malloc(c * sizeof(int));
	if (!col) {
		*quit = 1;	// error
		return;
	}
	for (int i = 0; i < c; i++)
		scanf("%d", &col[i]);

	if (index < 0 || index >= size) {
		printf("No matrix with the given index\n");
		free(lin);
		free(col);
		return;
	}

	int **tmp2 = matrix_resize(mat_lst, l, lin, c, col, index);
	if (!tmp2) {
		*quit = 1;	// error
		return;
	}

	mat_lst[index].a = tmp2;
	mat_lst[index].m = l;
	mat_lst[index].n = c;

	free(lin);
	free(col);
}

int M(matrix_t *mat_lst, int size, int index, int index1)
{
	if (index < 0 || index >= size || index1 < 0 || index1 >= size) {
		printf("No matrix with the given index\n");
		return 0;
	}
	int m1 = mat_lst[index1].m, n = mat_lst[index].n;
	if (n != m1) {
		printf("Cannot perform matrix multiplication\n");
		return 0;
	}
	return 1;
}

void O(matrix_t *mat_lst, int size, int *quit)
{
	int *sum = (int *)calloc(size, sizeof(int));
	if (!sum) {
		*quit = 1;	// error
		return;
	}

	sort_matrix(mat_lst, sum, size);
	free(sum);
}

void T(matrix_t *mat_lst, int size, int *quit)
{
	int index;
	scanf("%d", &index);
	if (index < 0 || index >= size) {
		printf("No matrix with the given index\n");
		return;
	}

	int **tmp4 = matrix_transpose(mat_lst, index);
	if (!tmp4) {
		*quit = 1;	// error
		return;
	}
	mat_lst[index].a = tmp4;
	int aux = mat_lst[index].m;
	mat_lst[index].m = mat_lst[index].n;
	mat_lst[index].n = aux;
}

int main(void)
{
	matrix_t *mat_lst = (matrix_t *)malloc(0), *tmp;
	int size = 0, size1 = 0, quit = 0, index, index1;
	char cmd;
	while (!quit) {
		scanf(" %c", &cmd);
		switch (cmd) {
		case 'L':
			tmp = alloc_matrix(mat_lst, size);
			if (!tmp) {
				quit = 1;	// error
				break;
			}
			mat_lst = tmp;
			L(mat_lst, &size, &size1, &quit);
			break;
		case 'D':
			scanf("%d", &index);
			matrix_dim(mat_lst, size, index);
			break;
		case 'P':
			scanf("%d", &index);
			print_matrix(mat_lst, size, index);
			break;
		case 'C':
			C(mat_lst, size, &quit);
			break;
		case 'M':
			scanf("%d%d", &index, &index1);
			int ok = M(mat_lst, size, index, index1);
			if (ok) {
				int **tmp3 = matrix_multiplication(mat_lst, index, index1);
				if (!tmp3) {
					quit = 1;	// error
					break;
				}
				tmp = alloc_matrix(mat_lst, size);
				if (!tmp) {
					quit = 1;	// error
					break;
				}
				mat_lst = tmp;
				int m = mat_lst[index].m, n1 = mat_lst[index1].n;
				mat_lst[size].m = m;
				mat_lst[size].n = n1;
				mat_lst[size].a = tmp3;
				size++;
				if (size > size1)
					size1++;
			}
			break;
		case 'O':
			O(mat_lst, size, &quit);
			break;
		case 'T':
			T(mat_lst, size, &quit);
			break;
		case 'F':
			remove_matrix(mat_lst, &size);
			if (size == size1 / 2) {
				tmp = alloc_matrix(mat_lst, size);
				if (!tmp)
					quit = 1;	// error
			}
			if (!quit) {
				mat_lst = tmp;
				size1 = size;
			}
			break;
		case 'Q':
			quit = 1;
			break;

		default:
			printf("Unrecognized command\n");
			break;
		}
	}
	free_matrix(mat_lst, size1);
	return 0;
}
