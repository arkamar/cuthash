#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <openssl/evp.h>

#include "arg.h"

#if defined(DEBUG)
#	define info(...) \
	fprintf(stderr, "DBG: " __VA_ARGS__)
#else
#	define info(...)
#endif

char * argv0;
char * separators = "\t";

void
usage() {
	fprintf(stderr,
		"Usage: %s [-s separators] <fields ...>\n", argv0);
	exit(1);
}

int
main(int argc, char * argv[]) {
	char * line = NULL;
	size_t cap = 0;
	ssize_t len;
	int i;

	unsigned char hash[EVP_MAX_MD_SIZE];
	int hash_len;
	EVP_MD_CTX * ctx;
	const EVP_MD * md = EVP_ripemd160();

	ARGBEGIN {
	case 's':
		separators = EARGF(usage());
		break;
	default:
		usage();
	} ARGEND

	ctx = EVP_MD_CTX_create();

	while ((len = getline(&line, &cap, stdin)) > 0) {
		line[len--] = '\0';

		EVP_DigestInit_ex(ctx, md, NULL);
		EVP_DigestUpdate(ctx, line, len);
		EVP_DigestFinal(ctx, hash, &hash_len);

		fwrite(line, 1, len, stdout);
		fwrite("\t", 1, 1, stdout);

		for (i = 0; i < hash_len; i++)
			printf("%02x", hash[i]);
		puts("");
	}

	info("Done\n");

	free(line);

	EVP_MD_CTX_destroy(ctx);
	EVP_cleanup();

	return 0;
}
