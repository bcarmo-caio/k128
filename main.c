#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include "defs.h"
#include <time.h>
#include "k128_randomness.h"
#include "k128_routines.h"

static bool validatepassword(char *);
static void removefile (char *);
static struct action *treat_option(int, char **);
static void print_usage(char **);
static void print_passwd_message(void);

/*---------------------------------------------------------------------------*/

struct action {
	bool encrypt, decrypt, item_1, item_2; /*functions*/
	bool flag_erase_infile, flag_print_subkeys; /*flags*/
	char *in,*out,*pass; /*arguments*/
};

/*---------------------------------------------------------------------------*/

int main (int argc, char **argv)
{
	struct action *a;
	a = treat_option(argc, argv);
	if(!validatepassword(a->pass))
	{ print_passwd_message(); exit(EXIT_FAILURE); }


	srand(time(NULL));

	if(a->encrypt) k128_encrypt_file(a->in, a->out, a->pass, a->flag_print_subkeys);
	if(a->decrypt) k128_decrypt_file(a->in, a->out, a->pass, a->flag_print_subkeys);
	if(a->item_1)  k128_item1(a->in, a->pass, a->flag_print_subkeys, false);
	if(a->item_2)  k128_item2(a->in, a->pass, a->flag_print_subkeys);

	if(a->flag_erase_infile) removefile(a->in);
	free(a);

	return(EXIT_SUCCESS);
}

/*---------------------------------------------------------------------------*/

static void print_passwd_message(void)
{
	puts("Password must have at least 8 digits lenght, 16 maximum.");
	puts("Must have at least two decimal numbers and at least two letters");
}


/*---------------------------------------------------------------------------*/

static struct action *treat_option(int argc, char **argv)
{
	struct action *a;
	int opt;
	bool errflag;

	a = malloc(sizeof(struct action));

	/*default is not erase input file*/
	a->flag_erase_infile = false;
	a->flag_print_subkeys = false;

	a->encrypt = a->decrypt = a->item_1 =a->item_2 = false;
	a->in = a->out = a->pass = NULL;
	errflag = false;

	while((opt = getopt(argc, argv, "i:o:p:acd12k")) != -1)
		switch(opt)
		{
			case 'c':
				a->encrypt = true;
				if(a->decrypt || a->item_1 || a->item_2) errflag = true;
				break;
			case 'd':
				a->decrypt = true;
				if(a->encrypt || a->item_1 || a->item_2) errflag = true;
				break;
			case 'a':
				a->flag_erase_infile = true;
				if(a->decrypt || a->item_1 || a->item_2) errflag = true;
				break;
			case 'i':
				a->in = optarg;
				break;
			case 'o':
				a->out = optarg;
				if(a->item_1 || a->item_2) errflag = true;
				break;
			case 'p':
				a->pass = optarg;
				break;
			case '1':
				a->item_1 = true;
				a->in = optarg;
				if(a->encrypt || a->decrypt || a->item_2) errflag = true;
				break;
			case '2':
				a->in = optarg;
				a->item_2 = true;
				if(a->encrypt || a->decrypt || a->item_1) errflag = true;
				break;
			case 'k':
				a->flag_print_subkeys = true;
				break;
			default: /*?*/
				errflag = true;
				break;
		}

	/*QM algorithm? Maybe some other day hehe*/
	if(!a->pass) errflag = true;
	if((a->encrypt || a->decrypt) && !a->out) errflag = true;
	if((a->item_1 || a->item_2) && !a->in) errflag = true;
	if(!(a->encrypt || a->decrypt || a->item_1 || a->item_2)) errflag = true;

	if(errflag)
	{
		print_usage(argv);
		free(a);
		exit(EXIT_FAILURE);
	}
	return a;
}

/*---------------------------------------------------------------------------*/

static void print_usage(char **argv)
{
	printf("Usage:\n");
	printf(" To encrypt a file:\n");
	printf("  %s -c -i <in file> -o <out file> -p <password> [-a]\n", argv[0]);
	printf("     -a flag writes zero in <in file> and deletes it.\n");
	printf(" To decrypt a file:\n");
	printf("  %s -d -i <in file> -o <out file> -p <password>\n", argv[0]);
	printf(" To calculate randomness using method 1 as described in pdf:\n");
	printf("  %s -1 -i <in file> -p <password> | not working yet.\n", argv[0]);
	printf(" To calculate randomness using method 2 as described in pdf:\n");
	printf("  %s -2 -i <in file> -p <password> | not workgin yet.\n", argv[0]);
}

/*---------------------------------------------------------------------------*/

static bool validatepassword(char *pass)
{
	u32 i, letter, algarism, passsize;

	passsize = strlen(pass);
	if(passsize < 8) return false;
	letter = algarism = 0;
	for(i=0; i < passsize; i++)
	{
		if( (pass[i] >= 'a' && pass[i] <= 'z') ||
			 (pass[i] >= 'A' && pass[i] <= 'Z') )
			letter++;
		else
			if(pass[i] >= '0' && pass[i] <= '9')
				algarism++;
	}
	if(letter < 2 || algarism < 2)
		return false;

	return true;
}

/*---------------------------------------------------------------------------*/

static void removefile (char *filename)
{
	FILE *fp;
	u32 i, filesize;

	puts("A funcao de remover esta implementada porem desabilitada!");
	exit(EXIT_SUCCESS);

	fp = fopen(filename, "r+");
	fseek(fp, 0, SEEK_END);
	filesize = ftell(fp);
	rewind(fp);
	for(i=0; i<filesize; i++)
		fprintf(fp, " ");
	remove(filename);
}
