#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#ifdef HAVE_STDLIB_H
#include <stdlib.h>
#endif

#include "hash.h"


/*
     Update a running Adler-32 checksum with the bytes buf[0..len-1] and
   return the updated checksum. If buf is NULL, this function returns
   the required initial value for the checksum.
   An Adler-32 checksum is almost as reliable as a CRC32 but can be computed
   much faster. Usage example:

     uLong adler = adler32(0L, Z_NULL, 0);

     while (read_buffer(buffer, length) != EOF) {
       adler = adler32(adler, buffer, length);
     }
     if (adler != original_adler) error();
*/

#ifndef FAR
#  define FAR
#endif

#define Z_NULL  0  /* for initializing zalloc, zfree, opaque */

typedef unsigned char  Byte;  /* 8 bits */
typedef unsigned int   uInt;  /* 16 bits or more */
typedef unsigned long  uLong; /* 32 bits or more */

typedef Byte FAR Bytef;
typedef char FAR charf;
typedef int FAR intf;
typedef uInt FAR uIntf;
typedef uLong FAR uLongf;

#ifdef STDC
   typedef void FAR *voidpf;
   typedef void     *voidp;
#else
   typedef Byte FAR *voidpf;
   typedef Byte     *voidp;
#endif

/*+++++*/
/* adler32.c -- compute the Adler-32 checksum of a data stream
 * Copyright (C) 1995 Mark Adler
 * For conditions of distribution and use, see copyright notice in zlib.h
 */

/* From: adler32.c,v 1.6 1995/05/03 17:27:08 jloup Exp */

#define BASE 65521L /* largest prime smaller than 65536 */
#define NMAX 5552
/* NMAX is the largest n such that 255n(n+1)/2 + (n+1)(BASE-1) <= 2^32-1 */

#define DO1(buf)  {s1 += *buf++; s2 += s1;}
#define DO2(buf)  DO1(buf); DO1(buf);
#define DO4(buf)  DO2(buf); DO2(buf);
#define DO8(buf)  DO4(buf); DO4(buf);
#define DO16(buf) DO8(buf); DO8(buf);

/* ========================================================================= */
uLong adler32(adler, buf, len)
    uLong adler;
    Bytef *buf;
    uInt len;
{
    unsigned long s1 = adler & 0xffff;
    unsigned long s2 = (adler >> 16) & 0xffff;
    int k;

    if (buf == Z_NULL) return 1L;

    while (len > 0) {
        k = len < NMAX ? len : NMAX;
        len -= k;
        while (k >= 16) {
            DO16(buf);
            k -= 16;
        }
        if (k != 0) do {
            DO1(buf);
        } while (--k);
        s1 %= BASE;
        s2 %= BASE;
    }
    return (s2 << 16) | s1;
}

unsigned int HashAdler32(int size, int len, char *ptr)
{
  unsigned long hash;
  hash = adler32(adler32(0L, Z_NULL, 0), ptr, len);
  return hash % size;
}

unsigned int HashString(int size, int len, char *ptr)
{
  unsigned int hash;
  for (hash = 0; *ptr; ptr++)
    hash = 31 * hash + *ptr;
  return hash % size;
}

void HashInsert(HashTable *ht, int len, void *key, void *ptr)
{
  unsigned int hash = HashAdler32(ht->size, len, key);
  HashEntry *hx = ht->table[hash];
  while (hx)
    hx = hx->chain;
  hx = (HashEntry*)calloc(sizeof(HashEntry*), 1);
  hx->chain = ht->table[hash];
  hx->key = malloc(len);
  memcpy(hx->key, key, len);
  hx->data = ptr;
  ht->table[hash] = hx;
}

void* HashSearch(HashTable *ht, int len, void *key)
{
  unsigned int hash = HashAdler32(ht->size, len, key);
  HashEntry *hx = ht->table[hash];
  while (hx && memcmp(key, hx->key, len))
    hx = hx->chain;
  return hx ? hx->data : NULL;
}

int HashUpdate(HashTable *ht, int len, void *key, void *ptr)
{
  unsigned int hash = HashAdler32(ht->size, len, key);
  HashEntry *hx = ht->table[hash];
  while (hx && memcmp(key, hx->key, len))
    hx = hx->chain;
  if (hx)
    hx->data = ptr;
  return hx ? 1: 0;
}

int HashDelete(HashTable *ht, int len, void *key)
{
  unsigned int hash = HashAdler32(ht->size, len, key);
  HashEntry *hx = ht->table[hash];
  HashEntry **hxx = &ht->table[hash];
  while (hx && memcmp(key, hx->key, len))
    {
      hxx = &hx->chain;
      hx = hx->chain;
    }
  if (hx)
    {
      *hxx = hx->chain;
      free (hx);
    }
  return hx ? 1 : 0;
}

HashTable *HashCreate(unsigned int size)
{
  HashTable *ht = (HashTable*)calloc(sizeof(HashTable), size);
  if (ht)
    {
      ht->size = size;
      ht->table = (HashEntry**)calloc(sizeof(HashEntry*), size);
    }
  return ht;
}

int HashCount(HashTable *ht)
{
  int count = 0;
  int i;
  HashEntry *he;
  for (i = 0; i < ht->size; i++)
    {
      he = ht->table[i];
      while (he)
	{
	  count++;
	  he = he->chain;
	}
    }
  return count;
}

void** HashLinear(HashTable *ht)
{
  int count = HashCount(ht);
  void **linear = (void**)calloc(sizeof(void*), count + 1);
  int i, j;
  HashEntry *he;
  for (i = j = 0; i < ht->size; i++)
    {
      he = ht->table[i];
      while (he)
	{
	  linear[j++] = he;
	  he = he->chain;
	}
    }
  return linear;
}
