#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>

#define MAXWORD 100
#define BUFSIZE 100
#define NKEYS (sizeof(keytab) / sizeof(keytab[0]))

struct key {
	char *word;
	int count;
} keytab[] = { { "auto", 0 },	  { "break", 0 },    { "case", 0 },
	       { "char", 0 },	  { "const", 0 },    { "continue", 0 },
	       { "default", 0 },  { "do", 0 },	     { "double", 0 },
	       { "else", 0 },	  { "extern", 0 },   { "float", 0 },
	       { "for", 0 },	  { "goto", 0 },     { "if", 0 },
	       { "inline", 0 },	  { "int", 0 },	     { "long", 0 },
	       { "register", 0 }, { "restrict", 0 }, { "return", 0 },
	       { "short", 0 },	  { "signed", 0 },   { "sizeof", 0 },
	       { "static", 0 },	  { "struct", 0 },   { "switch", 0 },
	       { "typedef", 0 },  { "union", 0 },    { "unsigned", 0 },
	       { "void", 0 },	  { "volatile", 0 }, { "while", 0 } };

char buf[BUFSIZE];
int bufp = 0;

int get_word_cmd(char *word, int lim);
int get_word_file(char *word, int lim, void *input_file);
bool check_file_exist(const char *filepath);
void print_help(const char *program_name);
struct key *bin_search(char *word, struct key tab[], int keyword_index);

int get_word_cmd(char *word, int lim)
{
	int current_char;
	char *word_char = word;

	while (isspace(current_char = getchar()))
		;

	if (current_char == EOF) {
		return EOF;
	}

	*word_char++ = current_char;

	while (--lim > 0) {
		current_char = getchar();
		if (isspace(current_char) || current_char == EOF) {
			break;
		}
		*word_char++ = current_char;
	}

	*word_char = '\0';

	if (current_char == EOF && word_char == word + 1) {
		return EOF;
	}

	return word[0];
}

int get_word_file(char *word, int lim, void *input_file)
{
	int current_char;
	char *word_char = word;

	while (isspace(current_char = fgetc((FILE *)input_file)))
		;

	if (current_char == EOF) {
		return EOF;
	}

	*word_char++ = current_char;

	while (--lim > 0) {
		current_char = fgetc((FILE *)input_file);
		if (isspace(current_char) || current_char == EOF) {
			break;
		}
		*word_char++ = current_char;
	}

	*word_char = '\0';

	if (current_char == EOF && word_char == word + 1) {
		return EOF;
	}

	return word[0];
}

bool check_file_exist(const char *filepath)
{
	return access(filepath, F_OK) == 0;
}

struct key *bin_search(char *word, struct key tab[], int keyword_index)
{
	int cond;
	struct key *mid;
	struct key *low = &tab[0];
	struct key *high = &tab[keyword_index - 1];

	while (low <= high) {
		mid = low + (high - low) / 2;
		if ((cond = strcmp(word, mid->word)) < 0) {
			high = mid - 1;
		} else if (cond > 0) {
			low = mid + 1;
		} else {
			return mid;
		}
	}
	return NULL;
}

int main(int argc, char *argv[])
{
	FILE *input_file = NULL;

	int opt;
	char *mode = NULL;
	char *filepath = NULL;
	char word[MAXWORD];

	struct key *p;

	if (argc < 2) {
		fprintf(stderr, "[ERROR] Unexpected number of arguments\n");
		print_help(argv[0]);
	}

	printf("============== KEYWORD COUNTER ==============\n");
	while ((opt = getopt(argc, argv, "m:f:")) != -1) {
		switch (opt) {
		case 'm':
			mode = optarg;
			if (strcmp(mode, "cmdmode") != 0 &&
			    (strcmp(mode, "filemode") != 0)) {
				fprintf(stderr,
					"[ERROR] Unexpected value for -m parameter\n");
				print_help(argv[0]);
			}
			break;
		case 'f':
			filepath = optarg;
			if (!check_file_exist(filepath)) {
				fprintf(stderr,
					"[ERROR] File does not exist - check -f parameter\n");
				print_help(argv[0]);
			}
			break;
		default:
			print_help(argv[0]);
		}
	}

	if (strcmp(mode, "cmdmode") == 0) {
		printf("Mode: %s\n", mode);
	}
	if (strcmp(mode, "filemode") == 0) {
		if (filepath == NULL) {
			fprintf(stderr,
				"[ERROR] Filepath not provided - please provide -f parameter\n");
			print_help(argv[0]);
		}
		printf("Mode: %s\n", mode);
		printf("Filepath: %s\n", filepath);
	}

	printf("---------------------------------------------\n");
	if (strcmp(mode, "cmdmode") == 0) {
		while (get_word_cmd(word, MAXWORD) != EOF) {
			if (isalpha(word[0]) &&
			    (p = bin_search(word, keytab, NKEYS)) != NULL) {
				p->count++;
			}
		}
	}

	if (strcmp(mode, "filemode") == 0) {
		input_file = fopen(filepath, "r");

		if (input_file == NULL) {
			fprintf(stderr, "Unable to read provided input_file\n");
			exit(EXIT_FAILURE);
		}

		while (get_word_file(word, MAXWORD, input_file) != EOF) {
			if (isalpha(word[0]) &&
			    (p = bin_search(word, keytab, NKEYS)) != NULL) {
				p->count++;
			}
		}

		fclose(input_file);
	}

	printf("\n\nSummary:\n");
	for (p = keytab; p < keytab + NKEYS; p++) {
		if (p->count > 0) {
			printf("%4d %s\n", p->count, p->word);
		}
	}

	printf("=============================================\n");

	return EXIT_SUCCESS;
}

void print_help(const char *program_name)
{
	printf("Help:\n");
	printf("    %s\n", program_name);
	printf("    Options:\n");
	printf("      -m [cmdmode | filemode]\n");
	printf("      -f [filepath (only for filemode)]\n");
	exit(EXIT_FAILURE);
}
