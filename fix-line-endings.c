/*
  Convert file line endings to whatever is standard for the underlying system.
  Unix/Linux/Apple = '\n'
  Windows          = '\r\n'

  Usage:  fix-line-endings [-X]  [file]...

  X is d = DOS / Windows (CR/LF) (D, w, and W are the same)
       u = UNIX (LF) (U, l, and L are the same)

  When this program is compiled, it defaults to the standard for the underlying system.

  Written by Blake McBride (blake@mcbridemail.com)
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#if defined(unix) || defined(__APPLE__)
#include <unistd.h>
#define mktemp(t)	close(mkstemp(t))
#endif

#if defined(_MSC_VER) || defined(_WIN32) || defined(_WIN64)
#define WINDOWS
#endif

#ifdef WINDOWS
#define RMODE	"rb"
#define WMODE	"wb"
#else
#define RMODE	"r"
#define WMODE	"w"
#endif

enum system_type {
	UNIX,
	WINDOWSLE
};

static	void	copy(enum system_type mode, FILE *ffp, FILE *tfp);
static	void	help(void);


int main(int argc, char *argv[])
{
	FILE	*ffp, *tfp;
	char	*ffile, tfile[20];
	enum system_type  mode;

#ifdef WINDOWS
	mode = WINDOWSLE;
#else
	mode = UNIX;
#endif

	if (argc > 1 && argv[1][0] == '-') {
		char c = argv[1][1];
		if (c == 'w' || c == 'W' || c == 'd' || c == 'D')
			mode = WINDOWSLE;
		else if (c == 'u' || c == 'U' || c == 'l' || c == 'L')
			mode = UNIX;
		else if (c == '?' || c == 'h' || c == 'H')
			help();
		argc--;
		argv++;
	}
	if (argc == 1)
		copy(mode, stdin, stdout);
	else {
		strcpy(tfile, "tfXXXXXX");
		mktemp(tfile);
		while (--argc)  {
			if (NULL == (ffp = fopen(ffile=*++argv, RMODE)))  {
				fprintf(stderr, "Can't open %s\n", ffile);
				continue;
			}
			if (NULL == (tfp = fopen(tfile, WMODE)))  {
				fprintf(stderr, "Can't create %s\n", tfile);
				fclose(ffp);
				continue;
			}
			copy(mode, ffp, tfp);
			fclose(ffp);
			fclose(tfp);
			unlink(ffile);
			rename(tfile, ffile);
		}
	}
	return 0;
}

static	void	copy(enum system_type mode, FILE *ffp, FILE *tfp)
{
	register int	c, pc;

	for (pc = -1 ; 1 ; pc = c)  {
		c = getc(ffp);
		if (c == EOF  &&  feof(ffp))
			break;
		if (pc == -1)
			continue;
		if (mode == WINDOWSLE) {
			if (pc == '\r' && c == '\n') {
				putc(pc, tfp);
				putc(c, tfp);
				c = -1;
			} else if (pc == '\n' && c == '\n') {
				putc('\r', tfp);
				putc('\n', tfp);
				putc('\r', tfp);
				putc('\n', tfp);
				c = -1;
			} else if (pc != '\r' && c == '\n') {
				putc(pc, tfp);
				putc('\r', tfp);
				putc(c, tfp);
				c = -1;
			} else if (pc == '\n' && c != '\n') {
				putc('\r', tfp);
				putc('\n', tfp);
			} else 
				putc(pc, tfp);
		} else {
			if (pc != '\r' || c != '\n')
				putc(pc, tfp);
		}
	}
	if (mode == WINDOWSLE && pc == '\n')
		putc('\r', tfp);
	if (pc > -1)
		putc(pc, tfp);
}

static void help()
{
	fprintf(stderr, "Usage: fix-line-endings  [-X]  [file ...]\n");
	fprintf(stderr, "\n\twhere X = d, D, w, or W for Windows\n");
	fprintf(stderr,   "\t        = u, U, l, or L for Linux/Unix/MacOS\n");
	fprintf(stderr, "\nDefault is appropriate for the underlying system.\n");
	fprintf(stderr, "Files are adjusted in-place.\n");
	exit(0);
}
