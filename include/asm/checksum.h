#ifndef _I386_CHECKSUM_H
#define _I386_CHECKSUM_H

/*
 * computes the checksum of a memory block at buff, length len,
 * and adds in "sum" (32-bit)
 *
 * returns a 32-bit number suitable for feeding into itself
 * or csum_tcpudp_magic
 *
 * this function must be called with even lengths, except
 * for the last fragment, which may be odd
 *
 * it's best to have buff aligned on a 32-bit boundary
 */
unsigned int csum_partial(const unsigned char * buff, int len, unsigned int sum);

/*
 * the same as csum_partial, but copies from src while it
 * checksums
 *
 * here even more important to align src and dst on a 32-bit (or even
 * better 64-bit) boundary
 */

unsigned int csum_partial_copy( const char *src, char *dst, int len, int sum);


/*
 * the same as csum_partial_copy, but copies from user space.
 *
 * here even more important to align src and dst on a 32-bit (or even
 * better 64-bit) boundary
 */

unsigned int csum_partial_copy_fromuser(const char *src, char *dst, int len, int sum);

/*
 *	This is a version of ip_compute_csum() optimized for IP headers,
 *	which always checksum on 4 octet boundaries.
 *
 *	By Jorge Cwik <jorge@laser.satlink.net>, adapted for linux by
 *	Arnt Gulbrandsen.
 */
static inline unsigned short ip_fast_csum(unsigned char * iph,
					  unsigned int ihl) {
	unsigned int sum;

	__asm__("movl (%1), %0\n\t"
	   "subl $4, %2\n\t"
	   "jbe 2f\n\t"
	   "addl 4(%1), %0\n\t"
	   "adcl 8(%1), %0\n\t"
	   "adcl 12(%1), %0\n\t"
"1:	    adcl 16(%1), %0\n\t"
	   "lea 4(%1), %1\n\t"
	   "decl %2\n\t"
	   "jne	1b\n\t"
	   "adcl $0, %0\n\t"
	   "movl %0, %2\n\t"
	   "shrl $16, %0\n\t"
	   "addw %w2, %w0\n\t"
	   "adcl $0, %0\n\t"
	   "notl %0\n\t"
"2: "
	: "=&r" (sum), "=&r" (iph), "=&r" (ihl)
	: "1" (iph), "2" (ihl));
	return(sum);
}




/*
 * computes the checksum of the TCP/UDP pseudo-header
 * returns a 16-bit checksum, already complemented
 */

static inline unsigned short int csum_tcpudp_magic(unsigned long saddr,
						   unsigned long daddr,
						   unsigned short len,
						   unsigned short proto,
						   unsigned int sum) {
    __asm__("addl %1, %0\n\t"
	"adcl %4, %0\n\t"
	"adcl %5, %0\n\t"
	"adcl $0, %0\n\t"
	"movl %0, %1\n\t"
	"shrl $16, %1\n\t"
	"addw %w1, %w0\n\t"
	"adcl $0, %0\n\t"
	"notl %0\n\t"
	: "=&r" (sum), "=&r" (saddr)
	: "0" (daddr), "1"(saddr), "r"((ntohs(len)<<16)+proto*256), "r"(sum));
	return((unsigned short)sum);
}

/*
 *	Fold a partial checksum without adding pseudo headers
 */

static inline unsigned int csum_fold(unsigned int sum)
{
	__asm__("addl %1, %0; adcl $0xffff, %0"
		: "=r" (sum)
		: "r" (sum << 16), "0" (sum & 0xffff0000)
	);
	return (~sum) >> 16;
}
 
/*
 * this routine is used for miscellaneous IP-like checksums, mainly
 * in icmp.c
 */

static inline unsigned short ip_compute_csum(unsigned char * buff, int len) {
    unsigned int sum;

    unsigned int scratch;
    __asm__("movl %0, %1;shrl $16, %1;addw %w1, %w0;adcl $0, %0; notl %0"
	: "=a"(sum), "=r" (scratch)
	: "0" (csum_partial(buff, len, 0)));
	return(sum);
}

#endif
