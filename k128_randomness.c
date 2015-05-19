#include "k128_randomness.h"

static u64 hamming_distance_64bits(u64 *, u64 *);


void k128_item1(char *input, char *pass, bool flag_print_subkeys,
		bool flag_item2)
{
	FILE *fp;
	u64 pure[16], untouched_encripted[16], touched_encripted[16];
	u8 j,k;
	u16 i;
	u64 *subkeys;
	u64 Sum[8];

	memset(Sum, 0, 8*sizeof(u64));
	fp = fopen(input, "rb");
	if(!fp)
	{
		printf("Could not open %s\n", input);
		exit(EXIT_FAILURE);
	}
	if(fread(pure, 1, 128, fp) != 128)
	{
		printf("Error: %s is not 1024 bits (256 bytes) lengh\n", input);
		exit(EXIT_FAILURE);
	}

	subkeys = generate_subkeys(pass, flag_print_subkeys);

	/* building VetC */
	k128_encrypt(pure[0] ^ 0xffffffffffffffff, /*CBC*/
				 pure[1] ^ 0xffffffffffffffff, /*note: pointer arithmetic ;)*/
			untouched_encripted, untouched_encripted + 1,
			subkeys);
	for(j=0; j<14; j += 2)
	{
		/*CBC*/
		k128_encrypt(pure[j] ^ untouched_encripted[j-2], /*CBC*/
				pure[(j+1)] ^ untouched_encripted[j-1], /*pointer arithmetic*/
				untouched_encripted + j, untouched_encripted + (j+1),
				subkeys);
	}
	/* built */

	for(i=0; i<1024; i++)
	{
		/* building VetAlterC */
		/* changing i-th bit */
		pure[i%16] ^= (1<<(i%64));
		if(flag_item2 && i<1024-8) /* if i>1024-8 were not specified*/
			pure[(i+8)%16] ^= (1<<((i+8)%64));
		k128_encrypt(pure[0] ^ 0xffffffffffffffff, /*CBC*/
					 pure[1] ^ 0xffffffffffffffff, /*note: pointer arithmetic*/
				touched_encripted, touched_encripted + 1,
				subkeys);
		for(j=2; j<14; j += 2)
		{
			/*CBC*/
			k128_encrypt(pure[j] ^ touched_encripted[j-2], /*CBC*/
					pure[(j+1)] ^ touched_encripted[j-1],/*pointer arithmetic*/
					touched_encripted + j, touched_encripted + (j+1),
					subkeys);
		}
		/* unchanging i-th bit */
		pure[i%16] ^= (1<<(i%64));
		if(flag_item2 && i<1024-8)
			pure[(i+8)%16] ^= (1<<((i+8)%64));
		/* built */
		printf("%u\n", i);

		/*nao entendi bem o que foi pedido*/
		for(k=0; k<16; k +=2)
			Sum[k/2] += hamming_distance_64bits(untouched_encripted + k, touched_encripted + k + 1);
	}

}

/*---------------------------------------------------------------------------*/

u64 hamming_distance_64bits(u64 *a, u64 *b)
{
	/* a and b parameters are numbers consisting of 128 bit */
	/* a[0] is the most significant bits (left most bits) */
	u64 val[2], tmp[2], dist;

	/* tmp variable will hold val-1 inside the loop */

	val[0] = a[0] ^ b[0];
	val[1] = a[1] ^ b[1];

	/* malabarismo aqui*/
	/*isso entra em loop infinito*/
	printf("vou entrar em loop infinito\n");
	while(val[0] || val[1])
	{
		dist++;
		if(val[1])
			tmp[0] = val[0] - 1;
		tmp[1] = val[1] - 1; /* yes, we can (do this) */

		val[0] &= tmp[0];
		val[1] &= tmp[1];
		break;
	}

	return dist;
}

/*---------------------------------------------------------------------------*/

void k128_item2(char *input, char *pass, bool flag_print_subkeys)
{
	k128_item1(input, pass, flag_print_subkeys, true);
	return;
}

/*---------------------------------------------------------------------------*/
