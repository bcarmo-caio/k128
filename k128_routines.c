#include "k128_routines.h"

static void k128_decrypt(u64, u64, u64 *, u64 *, u64 *);
static u64 ball(u64, u64);
static u64 inv_ball(u64, u64);
static u64 cshl(u64, u64, u64);
static void treat_pass(char *,	u64 *, u64 *);

#define R 12
/*---------------------------------------------------------------------------*/

#ifndef BUILD_TABLE

int loga[256] = {
	128,0,176,9,96,239,185,253,16,18,
	159,228,105,186,173,248,192,56,194,101,
	79,6,148,252,25,222,106,27,93,78,
	168,130,112,237,232,236,114,179,21,195,
	255,171,182,71,68,1,172,37,201,250,
	142,65,26,33,203,211,13,110,254,38,
	88,218,50,15,32,169,157,132,152,5,
	156,187,34,140,99,231,197,225,115,198,
	175,36,91,135,102,39,247,87,244,150,
	177,183,92,139,213,84,121,223,170,246,
	62,163,241,17,202,245,209,23,123,147,
	131,188,189,82,30,235,174,204,214,53,
	8,200,138,180,226,205,191,217,208,80,
	89,63,77,98,52,10,72,136,181,86,
	76,46,107,158,210,61,60,3,19,251,
	151,81,117,74,145,113,35,190,118,42,
	95,249,212,85,11,220,55,49,22,116,
	215,119,167,230,7,219,164,47,70,243,
	97,69,103,227,12,162,59,28,133,24,
	4,29,41,160,143,178,90,216,166,126,
	238,141,83,75,161,154,193,14,122,73,
	165,44,129,196,199,54,43,127,67,149,
	51,242,108,104,109,240,2,40,206,221,
	155,234,94,153,124,20,134,207,229,66,
	184,64,120,45,58,233,100,31,146,144,
	125,57,111,224,137,48
};

int pot[256] = {
	1,45,226,147,190,69,21,174,120,3,
	135,164,184,56,207,63,8,103,9,148,
	235,38,168,107,189,24,52,27,187,191,
	114,247,64,53,72,156,81,47,59,85,
	227,192,159,216,211,243,141,177,255,167,
	62,220,134,119,215,166,17,251,244,186,
	146,145,100,131,241,51,239,218,44,181,
	178,43,136,209,153,203,140,132,29,20,
	129,151,113,202,95,163,139,87,60,130,
	196,82,92,28,232,160,4,180,133,74,
	246,19,84,182,223,12,26,142,222,224,
	57,252,32,155,36,78,169,152,158,171,
	242,96,208,108,234,250,199,217,0,212,
	31,110,67,188,236,83,137,254,122,93,
	73,201,50,194,249,154,248,109,22,219,
	89,150,68,233,205,230,70,66,143,10,
	193,204,185,101,176,210,198,172,30,65,
	98,41,46,14,116,80,2,90,195,37,
	123,138,42,91,240,6,13,71,111,112,
	157,126,16,206,18,39,213,76,79,214,
	121,48,104,54,117,125,228,237,128,106,
	144,55,162,94,118,170,197,127,61,175,
	165,229,25,97,253,77,124,183,11,238,
	173,75,34,245,231,115,35,33,200,5,
	225,102,221,179,88,105,99,86,15,161,
	49,149,23,7,58,40
};
#endif

/*---------------------------------------------------------------------------*/

#ifdef BUILD_TABLE /*security reason. Better not having loga and pot as
global variables...*/ /*TODO there is a better (faster) way to build this!*/

#include <math.h>

static int my_pow(int x);
static int search_pow(int x);
static void build_log_table(void);
static void build_log_pow(void);

int loga[256];
int pot[256];
/*---------------------------------------------------------------------------*/

static int my_pow(int x)
{
	int i, result;

	if(x == 128) return 0;
	if(x == 1) return 45;
	if(x == 0) return 1;

	for(result = 45, i = 1; i < x; i++)
		result = (result*45) % 257;

	return result;
}

/*---------------------------------------------------------------------------*/

static int search_pow(int x)
{
	int i;
	for(i=0; i< 256; i++)
		if(pot[i] == x)
			return i;
	return -1;
}

/*---------------------------------------------------------------------------*/

static void build_log_table (void)
{
	int i;
	for(i=0; i<256; i++)
		loga[i] = search_pow(i);
}

/*---------------------------------------------------------------------------*/

static void build_log_pow (void)
{
	int i;
	for(i=0; i<256; i++)
		pot[i] = my_pow(i);
}

/*---------------------------------------------------------------------------*/
#endif

/*---------------------------------------------------------------------------*/

static void treat_pass(char *pass,	u64 *L0, u64 *L1)
{
	short int i, len;
	char buf[4]; /*"\n\0"*/
	u64 tmp;

	len = strlen(pass);

	*L0 = *L1 = 0x0000000000000000;

	for(i=7; i>=0; i--)
	{
		tmp=0;
		sprintf(buf, "%hx\n", pass[7-i]);
		sscanf(buf, "%llx", &tmp);
		*L0 |= (u64) ((tmp) << 8*i);

		tmp = 0;
		sprintf(buf, "%hx", pass[(15-i)%len]);
		sscanf(buf, "%llx", &tmp);
		*L1 |= (u64) ((tmp) << 8*i);
	}

	/*erasing unnecessary info*/
	tmp = 0;
	for(i=0; i<len; i++)
		pass[i] = '0';

	/*FIXME: esse free da pau*/
	/*free(pass);*/
	buf[0] =  buf[1] = '0';

	return;
}

/*---------------------------------------------------------------------------*/

static u64 cshl(u64 x, u64 L, u64 N)
/* x -> rotating number
 * L -> bits to rotate
 * N -> x has N bits*/
{ L %= N; return (x << L) | (x >> (N - L)); }

/*---------------------------------------------------------------------------*/

void k128_encrypt(u64 Xl, u64 Xr, u64 *Yl, u64 *Yr, u64 *subkeys)
{
	/*Xl is the 64 most signficant bits of the pure text*/
	/*Yl is the 64 most signficant bits of the encrypted text*/
	u8 k,i;
	u64 Y1, Y2, Z;

	*Yl = Xl;
	*Yr = Xr;

	/*XXX esse vetor comeca em 1*/
	for(k=i=1; i<=R; i++)
	{
		/*primeira parte*/
		*Yl = ball(*Yl, subkeys[k++]);
		*Yr = *Yr + subkeys[k++];

		/*segunda parte*/
		Y1 = *Yl ^ *Yr;
		Y2 = ball(ball(subkeys[k], Y1) + Y1, subkeys[k+1]);
		Z  = ball(subkeys[k], Y1) + Y2;
		k += 2;

		*Yl ^= Z;
		*Yr ^= Z;
	}

	/*ultima transformacao*/
	Y1 = *Yl; /*usado como temporario*/
	*Yl = ball(*Yr, subkeys[k++]);
	*Yr = Y1 + subkeys[k];
	return;
}

/*---------------------------------------------------------------------------*/

static void k128_decrypt(u64 Yl, u64 Yr, u64 *Xl, u64 *Xr, u64 *subkeys)
{
	u8 k,i;
	u64 Y1, Y2, Z;

	k = 50;

	*Xl = Yr - subkeys[k--];
	*Xr = inv_ball(Yl, subkeys[k--]);

	for(i=R; i>=1; i--)
	{
		Y1 = *Xr ^ *Xl;
		Y2 = ball(ball(subkeys[k-1], Y1) + Y1, subkeys[k]);
		Z  = ball(subkeys[k-1], Y1) + Y2;

		*Xr ^= Z;
		*Xl ^= Z;

		*Xl = inv_ball(*Xl, subkeys[k-3]);
		*Xr = *Xr - subkeys[k-2];

		k -= 4;
	}
	return;
}
/*---------------------------------------------------------------------------*/

void k128_encrypt_file(char *input, char *output, char *pass,
		bool flag_print_subkeys)
{
	FILE *fp_input, *fp_output;
	u64 *sub_keys;
	u8 buf[16], restinho;
	u64 Pl, Pr, El, Er; /*Pure, Encrypted*/
	u64 filesize;
	u64 i;

	fp_input = fopen(input, "rb");
	if(!fp_input)
	{
		perror("Error while processing input");
		exit(EXIT_FAILURE);
	}

	fp_output = fopen(output, "wb");
	if(!fp_output)
	{
		perror("Error while processing output");
		exit(EXIT_FAILURE);
	}

	fseek(fp_input, 0, SEEK_END);
	filesize = ftell(fp_input);
	restinho = filesize % 16;
	rewind(fp_input);

	sub_keys = generate_subkeys(pass, flag_print_subkeys);

	/* restinho, in fact, needs 4 bits. So the first 4 bits will always
	 * be zero.
	 * For security reason, we will replace these four left most bits
	 * with something*/
	restinho |= (rand()%16)<<4;
	fwrite(&restinho, 1, 1, fp_output);
	restinho &= 0x0f;
	Er = El = 0xffffffffffffffff; /*CBC*/
	for(i=0; i<filesize/16; i++)
	{
		fread((void *)&Pl, 8, 1, fp_input);
		fread((void *)&Pr, 8, 1, fp_input);
		Pl ^= El; Pr ^= Er; /*CBC*/
		k128_encrypt(Pl, Pr, &El, &Er, sub_keys);
		fwrite(&El, 8, 1, fp_output);
		fwrite(&Er, 8, 1, fp_output);
	}

	/*restinho*/
	if(restinho)
	{
		i=0;
		memset(buf, 0xff, 16);
		fseek(fp_input, -restinho, SEEK_END);
		while(fread((void *) (buf + i++), 1, 1, fp_input));
		i--; /*i has to be in [1..15]*/
		if(i >= 16 || i < 1)
		{
			printf("essa leitura nao deveria ocorrer. Abortando\n");
			exit(EXIT_FAILURE);
		}
		memcpy((void *)  &Pl, buf, 8);
		memcpy((void *)  &Pr, buf+8, 8);
		Pl ^= El; Pr ^= Er; /*CBC*/
		k128_encrypt(Pl, Pr,  &El, &Er, sub_keys);
		fwrite(&El, 8, 1, fp_output);
		fwrite(&Er, 8, 1, fp_output);
	}

	free(sub_keys);
	fclose(fp_input);
	fclose(fp_output);

	return;
}

/*---------------------------------------------------------------------------*/

void k128_decrypt_file(char *input, char *output, char *pass,
		bool flag_print_subkeys)
{
	FILE *fp_input, *fp_output;
	u64 *sub_keys;
	u8 /*buf[16],*/ restinho;
	u64 Pl, Pr, El, Er; /*Pure, Encrypted*/
	u64 Vl, Vr; /*CBC*/
	u64 filesize;
	u64 i;

	fp_input = fopen(input, "rb");
	if(!fp_input)
	{
		perror("Error while processing input");
		exit(EXIT_FAILURE);
	}

	fp_output = fopen(output, "wb");
	if(!fp_output)
	{
		perror("Error while processing output");
		exit(EXIT_FAILURE);
	}

	fseek(fp_input, 0, SEEK_END);
	filesize = ftell(fp_input);
	filesize--; /*first byte is not part of original plain text*/
	rewind(fp_input);

	sub_keys = generate_subkeys(pass, flag_print_subkeys);

	fread(&restinho, 1, 1, fp_input);
	/* Cleaning restinho */
	restinho &= 0x0f;

	Vl = Vr = 0xffffffffffffffff; /*CBC*/

	for(i=0; i<filesize/16; i++)
	{
		if(i){ Vl = El;	Vr = Er; } /*CBC*/ /* this if really annoys me*/
		fread((void *)&El, 8, 1, fp_input);
		fread((void *)&Er, 8, 1, fp_input);
		k128_decrypt(El, Er, &Pl, &Pr, sub_keys);
		Pl ^= Vl;
		Pr ^= Vr;
		fwrite(&Pl, 8, 1, fp_output);
		fwrite(&Pr, 8, 1, fp_output);
	}

	free(sub_keys);
	fclose(fp_input);
	fclose(fp_output);
	if(restinho) truncate(output, filesize - (16-restinho));

	return;
}

/*---------------------------------------------------------------------------*/

u64 *generate_subkeys(char *pass, bool flag_print_subkeys)
{
	u64 L0,L1; /*parametros*/
	u64 A,B;
	/*NAO USAR BUFFER*/
	u64 *K, *L;
	unsigned int i,j,s;

	/*pass will be erased here and freed*/
	treat_pass(pass, &L0, &L1);

	/*FIXME*/
	K = malloc((4*R +2 +1) * sizeof(u64));
	L = malloc((4*R +2 +1) * sizeof(u64));

	L[0] = L0;
	L[1] = L1;

	/*erasing unnecessary info*/
	L0 = L1 = 0x0000000000000000;

	for(j=2; j <= 4*R +2; j++)
		L[j] = L[j-1] + 0x9e3779b97f4a7c15;

	K[0] = 0x8aed2a6bb7e15162;
	for(j=1; j <= 4*R +2; j++)
		K[j] = K[j-1] + 0x7c159e3779b97f4a;
	A = B = i = j = 0;
	for(s=1; s <= 4*R +3; s++) /* +3 ???? +2 mislead k[4*R +2]*/
	{
		K[i] = cshl( K[i] + A + B, 3    , 64); A = K[i++];
		L[j] = cshl( L[j] + A + B, A + B, 64); B = L[j];
		/*erasing unnecessary info*/
		L[j++] = 0x0000000000000000;
	}

	free(L);
	if(flag_print_subkeys)
		for(i=1; i <= 4*R +2; i++)
			printf("k[%u] = %llx\n", i, K[i]);

	return K;
}

/*---------------------------------------------------------------------------*/

static u64 ball(u64 a, u64 b)
{
	u64 c;
	u8 a8,a7,a6,a5,a4,a3,a2,a1;
	u8 b8,b7,b6,b5,b4,b3,b2,b1;
	u8 c8,c7,c6,c5,c4,c3,c2,c1;

	/*from right to left, Aj is the j-th byte of A*/
	/*Thus A8 is the most significant byte*/

#if __BYTE_ORDER == __BIG_ENDIAN
	a8 = a;		b8 = b;
	a7 = a>>8;	b7 = b>>8;
	a6 = a>>16;	b6 = b>>16;
	a5 = a>>24; b5 = b>>24;
	a4 = a>>32; b4 = b>>32;
	a3 = a>>40; b3 = b>>40;
	a2 = a>>48; b2 = b>>48;
	a1 = a>>56; b1 = b>>56;
#elif __BYTE_ORDER == __LITTLE_ENDIAN
	a1 = a;		b1 = b;
	a2 = a>>8;	b2 = b>>8;
	a3 = a>>16;	b3 = b>>16;
	a4 = a>>24; b4 = b>>24;
	a5 = a>>32; b5 = b>>32;
	a6 = a>>40; b6 = b>>40;
	a7 = a>>48; b7 = b>>48;
	a8 = a>>56; b8 = b>>56;
#endif

	c1 = pot[a1] ^ pot[b1];
	c2 = pot[a2] ^ pot[b2];
	c3 = pot[a3] ^ pot[b3];
	c4 = pot[a4] ^ pot[b4];
	c5 = pot[a5] ^ pot[b5];
	c6 = pot[a6] ^ pot[b6];
	c7 = pot[a7] ^ pot[b7];
	c8 = pot[a8] ^ pot[b8];

	c=0;
	c  = c8; c = c<<8;
	c |= c7; c = c<<8;
	c |= c6; c = c<<8;
	c |= c5; c = c<<8;
	c |= c4; c = c<<8;
	c |= c3; c = c<<8;
	c |= c2; c = c<<8;
	c |= c1;

	return (c);
}

/*---------------------------------------------------------------------------*/

static u64 inv_ball(u64 c, u64 b)
{
	u64 a;
	u8 a8,a7,a6,a5,a4,a3,a2,a1;
	u8 b8,b7,b6,b5,b4,b3,b2,b1;
	u8 c8,c7,c6,c5,c4,c3,c2,c1;

	/*from right to left, Aj is the j-th byte of A*/
	/*Thus A8 is the most significant byte*/

#if __BYTE_ORDER == __BIG_ENDIAN
	c8 = c;		b8 = b;
	c7 = c>>8;	b7 = b>>8;
	c6 = c>>16;	b6 = b>>16;
	c5 = c>>24; b5 = b>>24;
	c4 = c>>32; b4 = b>>32;
	c3 = c>>40; b3 = b>>40;
	c2 = c>>48; b2 = b>>48;
	c1 = c>>56; b1 = b>>56;
#elif __BYTE_ORDER == __LITTLE_ENDIAN
	c1 = c;		b1 = b;
	c2 = c>>8;	b2 = b>>8;
	c3 = c>>16;	b3 = b>>16;
	c4 = c>>24; b4 = b>>24;
	c5 = c>>32; b5 = b>>32;
	c6 = c>>40; b6 = b>>40;
	c7 = c>>48; b7 = b>>48;
	c8 = c>>56; b8 = b>>56;
#endif

	a1 = loga[pot[b1] ^ c1];
	a2 = loga[pot[b2] ^ c2];
	a3 = loga[pot[b3] ^ c3];
	a4 = loga[pot[b4] ^ c4];
	a5 = loga[pot[b5] ^ c5];
	a6 = loga[pot[b6] ^ c6];
	a7 = loga[pot[b7] ^ c7];
	a8 = loga[pot[b8] ^ c8];

	a=0;
	a  = a8; a = a<<8;
	a |= a7; a = a<<8;
	a |= a6; a = a<<8;
	a |= a5; a = a<<8;
	a |= a4; a = a<<8;
	a |= a3; a = a<<8;
	a |= a2; a = a<<8;
	a |= a1;

	return (a);
}
