#ifndef __BEDULLIB_DEFS
#define __BEDULLIB_DEFS

/* integer types */
/* avoid finger aches */
#define u8  unsigned char
#define u16 unsigned int
#define u32 unsigned long int
#define u64 unsigned long long int

#define s8 signed char
#define s16 signed int
#define s32 signed long int
#define s64 signed long long int
/* integer types END */

/*---------------------------------------------------------------------------*/

/* boolean type */
#ifdef false
	#undef false
#endif
#ifdef true
	#undef true
#endif

#ifdef bool
	#undef bool
#endif

typedef enum bool{false, true} bool;
/* boolean type END */

#endif
