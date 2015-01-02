#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>

#define CMP_DIFFER 1
#define CMP_EQUALS 2

static int verbose;

static void kurt_err(const char *msg)
{
	if (!msg)
		printf("ERROR\n");
	else
		printf("ERROR: %s\n", msg);

	exit(EXIT_FAILURE);
}

static int compare(FILE *f0, FILE *f1)
{
	int res = CMP_EQUALS;
	int c0, c1;
	unsigned int cnt = 1;
	unsigned int num_bytes_differ = 0;

	if (!f0 || !f1)
		return -EINVAL;

	while (42) {
		c0 = fgetc(f0);
		c1 = fgetc(f1);

		// end of file one
		if (c0 == EOF && c1 != EOF) {
			if (ferror(f0))
				kurt_err("IO Error!");
			if (verbose)
				printf("File one is shorter than file two.\n");
			res = CMP_DIFFER;
			break;
		}
		// end of file two
		if (c0 != EOF && c1 == EOF) {
			if (ferror(f1))
				kurt_err("IO Error!");
			if (verbose)
				printf("File two is shorter than file one.\n");
			res = CMP_DIFFER;
			break;
		}
		// end?
		if (c0 == EOF && c1 == EOF)
			break;

		// cmp
		if (c0 != c1) {
			if (verbose)
				printf("Byte %u differ [ 0x%02x - 0x%02x ]\n", cnt, c0, c1);
			++num_bytes_differ;
			res = CMP_DIFFER;
		}

		++cnt;
	}

	if (verbose)
		printf("Bytes compared: %u, bytes differ: %u\n", cnt - 1, num_bytes_differ);

	return res;
}

int main(int argc, char **argv)
{
	FILE *f0, *f1;
	int c, res;

	// check args
	if (argc < 3)
		kurt_err("usage: cmp [-v] <file0> <file1>");

	// parse
	while ((c = getopt(argc, argv, "v")) != -1) {
		switch (c) {
		case 'v':
			verbose = 1;
			break;
		case '?':
			kurt_err("usage: cmp [-v] <file0> <file1>");
		default:
			kurt_err("w00t");
		}
	}

	// files
	f0 = fopen(argv[optind + 0], "r");
	f1 = fopen(argv[optind + 1], "r");
	if (!f0 || !f1)
		kurt_err(strerror(errno));

	res = compare(f0, f1);
	if (res == CMP_EQUALS)
		printf("Files are equal!\n");
	else
		printf("Files differ!\n");

	return EXIT_SUCCESS;
}
