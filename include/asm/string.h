#ifndef _I386_STRING_H_
#define _I386_STRING_H_

char * strcpy(char * dest,const char *src);
char * strncpy(char * dest,const char *src,size_t count);
char * strcat(char * dest,const char * src);
char * strncat(char * dest,const char * src,size_t count);
int strcmp(const char * cs,const char * ct);
int strncmp(const char * cs,const char * ct,size_t count);
char * strchr(const char * s, int c);
char * strrchr(const char * s, int c);
size_t strspn(const char * cs, const char * ct);
size_t strcspn(const char * cs, const char * ct);
char * strpbrk(const char * cs,const char * ct);
char * strstr(const char * cs,const char * ct);
size_t strlen(const char * s);
char * strtok(char * s,const char * ct);
void * __memcpy(void * to, const void * from, size_t n);
void * __constant_memcpy(void * to, const void * from, size_t n);
void * memmove(void * dest,const void * src, size_t n);
void * memchr(const void * cs,int c,size_t count);
void * memset(void * s,int c, size_t count);
size_t strnlen(const char * s, size_t count);
void * memscan(void * addr, int c, size_t size);
#endif
