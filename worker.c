#include <u.h>
#include <libc.h>
#include <bio.h>

double shortest;
int *path;
int chunksize;
unsigned long long start;
unsigned long long count;
unsigned long long size;

void
usage(char *argv0)
{
	sysfatal("usage:\n\t%s size datafile chunksize start", argv0);
}

double**
loadfile(char *filename)
{
	double **data;
	unsigned long long i;
	unsigned long long r;
	char *nptr, *rptr;
	Biobuf *bio;
	char *buf;

	bio = Bopen(filename, OREAD);
	if (bio == nil)
		return nil;

	data = malloc(sizeof(double*) * size);
	for (i = 0; i < size; i++)
		data[i] = malloc(sizeof(double) * size);

	r = 0;
	while(r < size && (buf = Brdline(bio, '\n')) != nil) {
		nptr = buf;

		i = 0;
		while (i < size) {
			data[r][i] = strtod(nptr, &rptr);
			nptr = rptr;
			i++;
		}

		r++;
	}

	Bterm(bio);

	return data;
}

void
swap(int *a, int *b)
{
	int temp;

	temp = *a;
	*a = *b;
	*b = temp;
}

void
length(double **mat, int *a)
{
	double l;
	int i;

	l = mat[a[size-1]][a[0]];
	for (i = 1; i < size; i++)
		l += mat[a[i-1]][a[i]];

	if (l < shortest) {
		shortest = l;
		memcpy(path, a, sizeof(int) * size);
	}
}

void
permute(double **mat, int *a, int l, int r)
{
	int i;

	if (count > chunksize)
		return;

	if (l == r) {
		count++;
		length(mat, a);
	} else for (i = l; i <= r; i++) {
		swap(&a[l], &a[i]);
		permute(mat, a, l + 1, r);
		swap(&a[l], &a[i]);
	}
}

void
ith(int *a, unsigned long long n, unsigned long long i)
{
	int j, k = 0;
	unsigned long long *fact = malloc(n * sizeof(unsigned long long));
	unsigned long long *perm = malloc(n * sizeof(unsigned long long));

	fact[k] = 1;
	while (++k < n)
		fact[k] = fact[k - 1] * k;

	for (k = 0; k < n; k++) {
		perm[k] = i / fact[n - 1 - k];
		i = i % fact[n - 1 - k];
	}

	for (k = n - 1; k > 0; k--)
		for (j = k - 1; j >= 0; j--)
			if (perm[j] <= perm[k])
				perm[k]++;

	for (k = 0; k < n; k++)
		a[k] = perm[k];

	free(perm);
	free(fact);
}

void
main(int argc, char **argv)
{
	double **mat;
	int *a;
	int i;
	char buf[0x400];

	if (argc < 5)
		usage(argv[0]);

	count = 0;
	size = atoll(argv[1]);
	chunksize = atoi(argv[3]);
	start = atoll(argv[4]);
	shortest = 1000000000.0;

	a = malloc(sizeof(int) * size);
	path = malloc(sizeof(int) * size);

	mat = loadfile(argv[2]);

	ith(a, size, start);
	permute(mat, a, 0, size - 1);

	snprint(buf, 0x400 - 1, "%f ", shortest);
	for (i = 0; i < size; i++)
		snprint(&buf[strlen(buf)], 0x400 - strlen(buf) - 1, "%d ", path[i]+1);
	print("%s", buf);
}
