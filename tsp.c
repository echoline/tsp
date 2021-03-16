#include <u.h>
#include <libc.h>

void
usage(char *argv0)
{
	sysfatal("usage:\n\t%s size datafile cpu [cpu ...]", argv0);
}

unsigned long long
factorial(unsigned long long n)
{
	if (n > 1)
		return n * factorial(n-1);
	return 1;
}

void
main(int argc, char **argv)
{
	int ncpu;
	unsigned long long npaths;
	unsigned long long parts;
	int i, r, d;
	char start[64];
	char chunksize[64];
	char **buf;
	int **pfds;
	double shortest;
	double f;

	if (argc < 4)
		usage(argv[0]);

	ncpu = argc - 3;
	npaths = factorial(atoll(argv[1]));
	parts = npaths / ncpu;
	pfds = malloc(ncpu * sizeof(int*));
	shortest = 1000000000.0;

	for (i = 0; i < ncpu; i++) {
		pfds[i] = malloc(2 * sizeof(int));
		pipe(pfds[i]);
		close(1);
		dup(pfds[i][1], 1);

		r = fork();
		if (r == -1)
			sysfatal("fork: %r");
		if (r == 0) {
			snprint(chunksize, 63, "%llud", parts);
			snprint(start, 63, "%llud", parts * i);
			execl("/bin/rcpu", "rcpu", "-h", argv[i+3], "-c", "worker", argv[1], argv[2], chunksize, start, nil);
		}
	}

	buf = malloc(sizeof(char*) * ncpu);

	d = 0;

	for (i = 0; i < ncpu; i++) {
		buf[i] = malloc(256 * sizeof(char));
		r = read(pfds[i][0], buf[i], 255);
		buf[i][r] = '\0';
		f = strtod(buf[i], nil);
		if (f < shortest) {
			shortest = f;
			d = i;
		}
		waitpid();
	}

	fprint(2, "%s\n", buf[d]);
}
