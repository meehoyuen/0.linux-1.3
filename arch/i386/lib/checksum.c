/*
 * INET		An implementation of the TCP/IP protocol suite for the LINUX
 *		operating system.  INET is implemented using the  BSD Socket
 *		interface as the means of communication with the user level.
 *
 *		IP/TCP/UDP checksumming routines
 *
 * Authors:	Jorge Cwik, <jorge@laser.satlink.net>
 *		Arnt Gulbrandsen, <agulbra@nvg.unit.no>
 *		Tom May, <ftom@netcom.com>
 *		Lots of code moved from tcp.c and ip.c; see those files
 *		for more names.
 *
 *		This program is free software; you can redistribute it and/or
 *		modify it under the terms of the GNU General Public License
 *		as published by the Free Software Foundation; either version
 *		2 of the License, or (at your option) any later version.
 */

#include <net/checksum.h>

/*
 * computes a partial checksum, e.g. for TCP/UDP fragments
 */

unsigned int csum_partial(const unsigned char * buff, int len, unsigned int sum) {
	  /*
	   * Experiments with ethernet and slip connections show that buff
	   * is aligned on either a 2-byte or 4-byte boundary.  We get at
	   * least a 2x speedup on 486 and Pentium if it is 4-byte aligned.
	   * Fortunately, it is easy to convert 2-byte alignment to 4-byte
	   * alignment for the unrolled loop.
	   */
	__asm__("pushl %%ebx;pushl %%ecx;pushl %%edx;pushl %%esi\n\t"
	    "testl $2, %%esi\n\t"	// Check alignment.
	    "jz 2f\n\t"			// Jump if alignment is ok.
	    "subl $2, %%ecx\n\t"	// Alignment uses up two bytes.
	    "jae 1f\n\t"		// Jump if we had at least two bytes.
	    "addl $2, %%ecx\n\t"	// ecx was < 2.  Deal with it.
	    "jmp 4f\n\t"
"1:	     movw (%%esi), %%bx\n\t"
	    "addl $2, %%esi\n\t"
	    "addw %%bx, %%ax\n\t"
	    "adcl $0, %%eax\n\t"
"2:\n\t"
	    "movl %%ecx, %%edx\n\t"
	    "shrl $5, %%ecx\n\t"
	    "jz 2f\n\t"
	    "testl %%esi, %%esi\n\t"
"1:	     movl (%%esi), %%ebx\n\t"
	    "adcl %%ebx, %%eax\n\t"
	    "movl 4(%%esi), %%ebx\n\t"
	    "adcl %%ebx, %%eax\n\t"
	    "movl 8(%%esi), %%ebx\n\t"
	    "adcl %%ebx, %%eax\n\t"
	    "movl 12(%%esi), %%ebx\n\t"
	    "adcl %%ebx, %%eax\n\t"
	    "movl 16(%%esi), %%ebx\n\t"
	    "adcl %%ebx, %%eax\n\t"
	    "movl 20(%%esi), %%ebx\n\t"
	    "adcl %%ebx, %%eax\n\t"
	    "movl 24(%%esi), %%ebx\n\t"
	    "adcl %%ebx, %%eax\n\t"
	    "movl 28(%%esi), %%ebx\n\t"
	    "adcl %%ebx, %%eax\n\t"
	    "lea 32(%%esi), %%esi\n\t"
	    "dec %%ecx\n\t"
	    "jne 1b\n\t"
	    "adcl $0, %%eax\n\t"
"2:	     movl %%edx, %%ecx\n\t"
	    "andl $0x1c, %%edx\n\t"
	    "je 4f\n\t"
	    "shrl $2, %%edx\n\t"
	    "testl %%esi, %%esi\n\t"
"3:	     adcl (%%esi), %%eax\n\t"
	    "lea 4(%%esi), %%esi\n\t"
	    "dec %%edx\n\t"
	    "jne 3b\n\t"
	    "adcl $0, %%eax\n\t"
"4:	     andl $3, %%ecx\n\t"
	    "jz 7f\n\t"
	    "cmpl $2, %%ecx\n\t"
	    "jb 5f\n\t"
	    "movw (%%esi),%%cx\n\t"
	    "leal 2(%%esi),%%esi\n\t"
	    "je 6f\n\t"
	    "shll $16,%%ecx\n\t"
"5:	     movb (%%esi),%%cl\n\t"
"6:	     addl %%ecx,%%eax\n\t"
	    "adcl $0, %%eax\n\t"
"7:	    "
	    "popl %%esi;popl %%edx;popl %%ecx;popl %%ebx\n\t"
	: "=a"(sum)
	: "0"(sum), "c"(len), "S"(buff));
	return(sum);
}



/*
 * copy from fs while checksumming, otherwise like csum_partial
 */

unsigned int csum_partial_copy_fromuser(const char *src, char *dst, 
				  int len, int sum) {
    __asm__("pushl %%ebx;pushl %%ecx;pushl %%edx;pushl %%edi;pushl %%esi\n\t"
	"testl $2, %%edi\n\t"	// Check alignment.
	"jz 2\n\t"		// Jump if alignment is ok.
	"subl $2, %%ecx\n\t"	// Alignment uses up two bytes.
	"jae 1f\n\t"		// Jump if we had at least two bytes.
	"addl $2, %%ecx\n\t"	// ecx was < 2.  Deal with it.
	"jmp 4f\n\t"
"1:	 movw %%fs:(%%esi), %%bx\n\t"
	"addl $2, %%esi\n\t"
	"movw %%bx, (%%edi)\n\t"
	"addl $2, %%edi\n\t"
	"addw %%bx, %%ax\n\t"
	"adcl $0, %%eax\n\t"
"2:\n\t"
	"movl %%ecx, %%edx\n\t"
	"shrl $5, %%ecx\n\t"
	"jz 2f\n\t"
	"testl %%esi, %%esi\n\t"
"1:	 movl %%fs:(%%esi), %%ebx\n\t"
	"adcl %%ebx, %%eax\n\t"
	"movl %%ebx, (%%edi)\n\t"
	"movl %%fs:4(%%esi), %%ebx\n\t"
	"adcl %%ebx, %%eax\n\t"
	"movl %%ebx, 4(%%edi)\n\t"
	"movl %%fs:8(%%esi), %%ebx\n\t"
	"adcl %%ebx, %%eax\n\t"
	"movl %%ebx, 8(%%edi)\n\t"
	"movl %%fs:12(%%esi), %%ebx\n\t"
	"adcl %%ebx, %%eax\n\t"
	"movl %%ebx, 12(%%edi)\n\t"
	"movl %%fs:16(%%esi), %%ebx\n\t"
	"adcl %%ebx, %%eax\n\t"
	"movl %%ebx, 16(%%edi)\n\t"
	"movl %%fs:20(%%esi), %%ebx\n\t"
	"adcl %%ebx, %%eax\n\t"
	"movl %%ebx, 20(%%edi)\n\t"
	"movl %%fs:24(%%esi), %%ebx\n\t"
	"adcl %%ebx, %%eax\n\t"
	"movl %%ebx, 24(%%edi)\n\t"
	"movl %%fs:28(%%esi), %%ebx\n\t"
	"adcl %%ebx, %%eax\n\t"
	"movl %%ebx, 28(%%edi)\n\t"
	"lea 32(%%esi), %%esi\n\t"
	"lea 32(%%edi), %%edi\n\t"
	"dec %%ecx\n\t"
	"jne 1b\n\t"
	"adcl $0, %%eax\n\t"
"2:	 movl %%edx, %%ecx\n\t"
	"andl $28, %%edx\n\t"
	"je 4f\n\t"
	"shrl $2, %%edx\n\t"
	"testl %%esi, %%esi\n\t"
"3:	 movl %%fs:(%%esi), %%ebx\n\t"
	"adcl %%ebx, %%eax\n\t"
	"movl %%ebx, (%%edi)\n\t"
	"lea 4(%%esi), %%esi\n\t"
	"lea 4(%%edi), %%edi\n\t"
	"dec %%edx\n\t"
	"jne 3b\n\t"
	"adcl $0, %%eax\n\t"
"4:	 andl $3, %%ecx\n\t"
	"jz 7f\n\t"
	"cmpl $2, %%ecx\n\t"
	"jb 5f\n\t"
	"movw %%fs:(%%esi), %%cx\n\t"
	"leal 2(%%esi), %%esi\n\t"
	"movw %%cx, (%%edi)\n\t"
	"leal 2(%%edi), %%edi\n\t"
	"je 6f\n\t"
	"shll $16,%%ecx\n\t"
"5:	 movb %%fs:(%%esi), %%cl\n\t"
	"movb %%cl, (%%edi)\n\t"
"6:	 addl %%ecx, %%eax\n\t"
	"adcl $0, %%eax\n\t"
"7:\n\t"
	"popl %%esi;popl %%edi;popl %%edx;popl %%ecx;popl %%ebx\n\t"
	: "=a" (sum)
	: "0"(sum), "c"(len), "S"(src), "D" (dst));
    return(sum);
}
/*
 * copy from ds while checksumming, otherwise like csum_partial
 */

unsigned int csum_partial_copy(const char *src, char *dst, 
				  int len, int sum) {
    __asm__("pushl %%ebx;pushl %%ecx;pushl %%edx;pushl %%edi;pushl %%esi\n\t"
	"testl $2, %%edi\n\t"		// Check alignment.
	"jz 2f\n\t"			// Jump if alignment is ok.
	"subl $2, %%ecx\n\t"		// Alignment uses up two bytes.
	"jae 1f\n\t"			// Jump if we had at least two bytes.
	"addl $2, %%ecx\n\t"		// ecx was < 2.  Deal with it.
	"jmp 4f\n\t"
"1:	 movw (%%esi), %%bx\n\t"
	"addl $2, %%esi\n\t"
	"movw %%bx, (%%edi)\n\t"
	"addl $2, %%edi\n\t"
	"addw %%bx, %%ax\n\t"
	"adcl $0, %%eax\n\t"
"2:\n\t"
	"movl %%ecx, %%edx\n\t"
	"shrl $5, %%ecx\n\t"
	"jz 2f\n\t"
	"testl %%esi, %%esi\n\t"
"1:	 movl (%%esi), %%ebx\n\t"
	"adcl %%ebx, %%eax\n\t"
	"movl %%ebx, (%%edi)\n\t"
	"movl 4(%%esi), %%ebx\n\t"
	"adcl %%ebx, %%eax\n\t"
	"movl %%ebx, 4(%%edi)\n\t"
	"movl 8(%%esi), %%ebx\n\t"
	"adcl %%ebx, %%eax\n\t"
	"movl %%ebx, 8(%%edi)\n\t"
	"movl 12(%%esi), %%ebx\n\t"
	"adcl %%ebx, %%eax\n\t"
	"movl %%ebx, 12(%%edi)\n\t"
	"movl 16(%%esi), %%ebx\n\t"
	"adcl %%ebx, %%eax\n\t"
	"movl %%ebx, 16(%%edi)\n\t"
	"movl 20(%%esi), %%ebx\n\t"
	"adcl %%ebx, %%eax\n\t"
	"movl %%ebx, 20(%%edi)\n\t"
	"movl 24(%%esi), %%ebx\n\t"
	"adcl %%ebx, %%eax\n\t"
	"movl %%ebx, 24(%%edi)\n\t"
	"movl 28(%%esi), %%ebx\n\t"
	"adcl %%ebx, %%eax\n\t"
	"movl %%ebx, 28(%%edi)\n\t"
	"lea 32(%%esi), %%esi\n\t"
	"lea 32(%%edi), %%edi\n\t"
	"dec %%ecx\n\t"
	"jne 1b\n\t"
	"adcl $0, %%eax\n\t"
"2:	 movl %%edx, %%ecx\n\t"
	"andl $28, %%edx\n\t"
	"je 4f\n\t"
	"shrl $2, %%edx\n\t"
	"testl %%esi, %%esi\n\t"
"3:	 movl (%%esi), %%ebx\n\t"
	"adcl %%ebx, %%eax\n\t"
	"movl %%ebx, (%%edi)\n\t"
	"lea 4(%%esi), %%esi\n\t"
	"lea 4(%%edi), %%edi\n\t"
	"dec %%edx\n\t"
	"jne 3b\n\t"
	"adcl $0, %%eax\n\t"
"4:	 andl $3, %%ecx\n\t"
	"jz 7f\n\t"
	"cmpl $2, %%ecx\n\t"
	"jb 5f\n\t"
	"movw (%%esi), %%cx\n\t"
	"leal 2(%%esi), %%esi\n\t"
	"movw %%cx, (%%edi)\n\t"
	"leal 2(%%edi), %%edx\n\t"
	"je 6f\n\t"
	"shll $16,%%ecx\n\t"
"5:	 movb (%%esi), %%cl\n\t"
	"movb %%cl, (%%edi)\n\t"
"6:	 addl %%ecx, %%eax\n\t"
	"adcl $0, %%eax\n\t"
"7:\n\t"
	"popl %%esi;popl %%edi;popl %%edx;popl %%ecx;popl %%ebx\n\t"
	: "=a" (sum)
	: "0"(sum), "c"(len), "S"(src), "D" (dst));
    return(sum);
}
