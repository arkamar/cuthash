#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <openssl/evp.h>

#include "arg.h"

#define MIN(x, y) ((x) < (y)) ? (x) : (y)
#define MAX(x, y) ((x) > (y)) ? (x) : (y)

struct range {
	size_t min, max;
	struct range * next;
};

char * argv0;
static char * separators = "\t";
static struct range * list = NULL;

static
void
usage() {
	fprintf(stderr,
		"Usage: %s [-s separators] [-d digest_algorithm] list\n", argv0);
	exit(1);
}

static
void
insert(struct range * r) {
	struct range * l, *p, *t;

	for (p = NULL, l = list; l; p = l, l = l->next) {
		if (r->max && r->max + 1 < l->min) {
			r->next = l;
			break;
		} else if (!l->max || r->min < l->max + 2) {
			l->min = MIN(r->min, l->min);
			for (p = l, t = l->next; t; p = t, t = t->next)
				if (r->max && r->max + 1 < t->min)
					break;
			l->max = (p->max && r->max) ? MAX(p->max, r->max) : 0;
			l->next = t;
			return;
		}
	}
	if (p)
		p->next = r;
	else
		list = r;
}

static
void
parselist(char * str) {
	char * s;
	size_t n = 1;
	struct range * r;

	if (!*str) {
		fprintf(stderr, "empty list\n");
		exit(1);
	}

	for (s = str; *s; s++)
		if (*s == ',')
			n++;

	r = malloc(n * sizeof(struct range));
	if (!r) {
		perror("Cannot allocate memory");
		exit(1);
	}
	for (s = str; n; n--, s++) {
		r->min = (*s == '-') ? 1 : strtoul(s, &s, 10);
		r->max = (*s == '-') ? strtoul(s + 1, &s, 10) : r->min;
		r->next = NULL;
		if (!r->min || (r->max && r->max < r->min) || (*s && *s != ',')) {
			fprintf(stderr, "bad list value\n");
			exit(1);
		}
		insert(r++);
	}
}

int
main(int argc, char * argv[]) {
	char * line = NULL;
	char * digest_algorithm = NULL;
	char * tok;
	size_t cap = 0;
	ssize_t len;
	int i, c;

	unsigned char hash[EVP_MAX_MD_SIZE];
	unsigned int hash_len;
	EVP_MD_CTX * ctx;
	const EVP_MD * md = EVP_ripemd160();

	ARGBEGIN {
	case 's':
		separators = EARGF(usage());
		if (!*separators) {
			fprintf(stderr, "Empty separator is forbiden\n");
			exit(1);
		}
		break;
	case 'd':
		digest_algorithm = EARGF(usage());
		break;
	default:
		usage();
	} ARGEND

	if (argc != 1)
		usage();

	parselist(*argv);

	if (digest_algorithm) {
		OPENSSL_init_crypto(OPENSSL_INIT_ADD_ALL_DIGESTS, NULL);
		md = EVP_get_digestbyname(digest_algorithm);
		if (!md) {
			fprintf(stderr, "Unknown message digest '%s'\n", digest_algorithm);
			exit(1);
		}
	}

	ctx = EVP_MD_CTX_create();

	while ((len = getline(&line, &cap, stdin)) > 0) {
		EVP_DigestInit(ctx, md);

		struct range * r = list;
		tok = line + strspn(line, separators);
		for (c = 1; *tok; c++) {
			size_t tok_len = 0;
			tok_len = strcspn(tok, separators);
			if (!r)
				break;
			if (c >= r->min && c <= r->max) {
#ifdef DEBUG
				fprintf(stderr, "W[%02d,%03ld]<", c, tok_len);
				fwrite(tok, 1, tok_len, stderr);
				fputs(">\n", stderr);
#endif
				EVP_DigestUpdate(ctx, tok, tok_len);
				if (c == r->max)
					r = r->next;
			}
			tok += tok_len;
			if (!*tok) {
				break;
			}
			tok += strspn(tok, separators);
		}

		EVP_DigestFinal(ctx, hash, &hash_len);

		for (i = 0; i < hash_len; i++) {
			printf("%02x", hash[i]);
		}
		fwrite(separators, 1, 1, stdout);
		fwrite(line, 1, len, stdout);
	}

	free(line);
	free(list);

	EVP_MD_CTX_destroy(ctx);
	OPENSSL_cleanup();

	return 0;
}
