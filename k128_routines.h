#ifndef __K128_ROUTINES
#define __K128_ROUTINES


#define _GNU_SOURCE

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include "defs.h"
#include "k128_routines.h"

void k128_encrypt_file(char *, char *, char *, bool);
void k128_decrypt_file(char *, char *, char *, bool);
u64 *generate_subkeys(char *, bool);
void k128_encrypt(u64, u64, u64 *, u64 *, u64 *);

#endif
