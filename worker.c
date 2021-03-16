#include <u.h>
#include <libc.h>
#include <bio.h>

double shortest;
int *path;
int chunksize;
int start;
unsigned long long count;
unsigned long long size;

void
usage(char *argv0)
{
	sysfatal("usage:\n\t%s size datafile chunksize start", argv0);
}

double**
loadfile(unsigned long long size, char *filename)
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
	while((buf = Brdline(bio, '\n')) != nil) {
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
	int i, begin;

	l = 0;

	begin = a[0];

	for (i = 1; i < size; i++)
		l += mat[begin][a[i]];
	l += mat[a[size-1]][begin];

	if (l < shortest) {
		shortest = l;
		memcpy(path, a, sizeof(int) * size);
	}
}

void
permute(double **mat, int *a, int l, int r)
{
	int i;

	if (l == r) {
		count++;
		if (count > start && count <= (start + chunksize))
			length(mat, a);
	} else for (i = l; i <= r; i++) {
		swap(&a[l], &a[i]);
		permute(mat, a, l + 1, r);
		swap(&a[l], &a[i]);
	}
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

	start = atoi(argv[4]);

	shortest = 1000000000.0;

	a = malloc(sizeof(int) * size);

	path = malloc(sizeof(int) * size);

	for (i = 0; i < size; i++)
		a[i] = i;

	mat = loadfile(size, argv[2]);

	permute(mat, a, 0, size - 1);

	snprint(buf, 0x400, "%f ", shortest);

	for (i = 0; i < size; i++)
		snprint(&buf[strlen(buf)], 0x400 - strlen(buf), "%d ", path[i]);

	print("%s", buf);
}
