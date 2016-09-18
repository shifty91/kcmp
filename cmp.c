/*
 * Copyright (c) 2015, Kurt Kanzenbach
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * * Redistributions of source code must retain the above copyright notice, this
 *   list of conditions and the following disclaimer.
 *
 * * Redistributions in binary form must reproduce the above copyright notice,
 *   this list of conditions and the following disclaimer in the documentation
 *   and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <getopt.h>
#include <stdarg.h>

enum { CMP_FAILURE = 2, CMP_DIFFER = 1, CMP_EQUALS = 0 };

static struct option long_opts[] = {
	{ "verbose", no_argument, NULL, 'v' },
	{ NULL,      0,           NULL,  0  }
};

static int verbose;

__attribute__((noreturn)) static void
kurt_err(const char * restrict fmt, ...)
{
	va_list args;

	va_start(args, fmt);
	fprintf(stderr, "ERROR: ");
	vfprintf(stderr, fmt, args);
	fprintf(stderr, "\n");
	va_end(args);

	exit(CMP_FAILURE);
}

static int compare(FILE *f0, FILE *f1)
{
	size_t num_bytes_differ = 0, cnt = 1;
	int res = CMP_EQUALS;
	int c0, c1;

	if (!f0 || !f1)
		kurt_err("Invalid arguments passed to '%s'", __func__);

	while (42) {
		c0 = fgetc(f0);
		c1 = fgetc(f1);

		/* end of file one */
		if (c0 == EOF && c1 != EOF) {
			if (ferror(f0))
				kurt_err("IO Error: ", strerror(errno));
			if (verbose)
				printf("File one is shorter than file two.\n");
			res = CMP_DIFFER;
			break;
		}
		/* end of file two */
		if (c0 != EOF && c1 == EOF) {
			if (ferror(f1))
				kurt_err("IO Error: ", strerror(errno));
			if (verbose)
				printf("File two is shorter than file one.\n");
			res = CMP_DIFFER;
			break;
		}
		/* end? */
		if (c0 == EOF && c1 == EOF)
			break;

		/* cmp */
		if (c0 != c1) {
			if (verbose)
				printf("Byte %5zu differs -- [ 0x%02x - 0x%02x ]\n", cnt, c0, c1);
			++num_bytes_differ;
			res = CMP_DIFFER;
		}

		++cnt;
	}

	if (verbose)
		printf("Bytes compared: %zu, bytes differ: %zu\n", cnt - 1, num_bytes_differ);

	return res;
}

int main(int argc, char **argv)
{
	FILE *f0, *f1;
	int c, res;

	/* parse */
	while ((c = getopt_long(argc, argv, "v", long_opts, NULL)) != -1) {
		switch (c) {
		case 'v':
			verbose = 1;
			break;
		case '?':
			kurt_err("usage: kcmp [-v] <file0> <file1>");
		default:
			kurt_err("w00t");
		}
	}

	/* check args */
	if (argc - optind != 2)
		kurt_err("usage: kcmp [-v] <file0> <file1>");

	/* files */
	f0 = fopen(argv[optind + 0], "r");
	f1 = fopen(argv[optind + 1], "r");
	if (!f0)
		kurt_err("Failed to open file '%s': %s", argv[optind + 0], strerror(errno));
	if (!f1)
		kurt_err("Failed to open file '%s': %s", argv[optind + 1], strerror(errno));

	res = compare(f0, f1);
	if (res == CMP_EQUALS)
		printf("Files are equal!\n");
	else
		printf("Files differ!\n");

	return res;
}
