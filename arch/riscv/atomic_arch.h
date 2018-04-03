#ifndef _INTERNAL_ATOMIC_H
#define _INTERNAL_ATOMIC_H

#include <stdint.h>

#define a_ctz_l a_ctz_l
static inline long a_ctz_l(unsigned long x)
{
	static const char debruijn32[32] = {
		0, 1, 23, 2, 29, 24, 19, 3, 30, 27, 25, 11, 20, 8, 4, 13,
		31, 22, 28, 18, 26, 10, 7, 12, 21, 17, 9, 6, 16, 5, 15, 14
	};
	return debruijn32[(x&-x)*0x076be629 >> 27];
}

#define a_ctz_64 a_ctz_64
static inline long a_ctz_64(uint64_t x)
{
	uint32_t y = x;
	if (!y) {
		y = x>>32;
		return 32 + a_ctz_l(y);
	}
	return a_ctz_l(y);
}

#define a_cas a_cas
static inline int a_cas(volatile int *p, int t, int s)
{
  /* FIXME: Temporary cas emulation */
  if(*p == t)
  {
    *p = s;
    return t;
  }

  return *p;

  /* TODO */
  /*
 a0 holds address of memory location
 a1 holds expected value
 a2 holds desired value
 v0 return value, 0 if successful, !0 otherwise
  */
/*
__asm__ __volatile__("\
  lr.w v1, a0; \
  li v0, 1; \
  bne v1, a1, return; \
  sc.w v0, a2, a0; \
  mov a0, a2; \
  return: \
  jr ra;");*/
}

#define a_cas_p a_cas_p
static inline void *a_cas_p(volatile void *p, void *t, void *s)
{
  /* FIXME: Temporary cas emulation */
  if(*((unsigned long *) p) == (unsigned long) t)
  {
    *((unsigned long *) p) = (unsigned long) s;
    return t;
  }

  return (void *) *((unsigned long *) p);
}

#define a_swap a_swap
static inline int a_swap(volatile int *x, int v)
{
	int old;
	do old = *x;
	while (a_cas(x, old, v) != old);
	return old;
}

#define a_fetch_add a_fetch_add
static inline int a_fetch_add(volatile int *x, int v)
{
	int old;
	do old = *x;
	while (a_cas(x, old, old+v) != old);
	return old;
}

#define a_inc a_inc
static inline void a_inc(volatile int *x)
{
	a_fetch_add(x, 1);
}

#define a_dec a_dec
static inline void a_dec(volatile int *x)
{
	a_fetch_add(x, -1);
}

#define a_store a_store
static inline void a_store(volatile int *p, int x)
{
	a_swap(p, x);
}

static inline void a_spin()
{
	a_cas(&(int){0}, 0, 0);
}

#define a_crash a_crash
static inline void a_crash()
{
	*(volatile char *)0=0;
}

#define a_and a_and
static inline void a_and(volatile int *p, int v)
{
	int old;
	do old = *p;
	while (a_cas(p, old, old&v) != old);
}

#define a_or a_or
static inline void a_or(volatile int *p, int v)
{
	int old;
	do old = *p;
	while (a_cas(p, old, old|v) != old);
}

#define a_or_l a_or_l
static inline void a_or_l(volatile void *p, long v)
{
	a_or(p, v);
}

#define a_and_64 a_and_64
static inline void a_and_64(volatile uint64_t *p, uint64_t v)
{
    *p = *p & v;
	//union { uint64_t v; uint32_t r[2]; } u = { v };
	//a_and((int *)p, u.r[0]);
	//a_and((int *)p+1, u.r[1]);
}

#define a_or_64 a_or_64
static inline void a_or_64(volatile uint64_t *p, uint64_t v)
{
	union { uint64_t v; uint32_t r[2]; } u = { v };
	a_or((int *)p, u.r[0]);
	a_or((int *)p+1, u.r[1]);
}

#define a_barrier a_barrier
static inline void a_barrier()
{
    __asm__ __volatile__( "" : : : "memory" );
}
#endif
