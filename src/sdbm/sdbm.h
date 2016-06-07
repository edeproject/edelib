#ifndef __EDELIB_SDBM_H__
#define __EDELIB_SDBM_H__

#ifdef __cplusplus
extern "C" {
#endif

/*
 * sdbm - ndbm work-alike hashed database library
 * based on Per-Ake Larson's Dynamic Hashing algorithms. BIT 18 (1978).
 * author: oz@nexus.yorku.ca
 * status: public domain. 
 */
#define DBLKSIZ 4096
#define PBLKSIZ 1024
#define PAIRMAX 1008			/* arbitrary on PBLKSIZ-N */
#define SPLTMAX	10			/* maximum allowed splits */
					/* for a single insertion */
#define DIRFEXT	".dir"
#define PAGFEXT	".pag"

typedef struct {
	int dirf;		       /* directory file descriptor */
	int pagf;		       /* page file descriptor */
	int flags;		       /* status/error flags, see below */
	long maxbno;		       /* size of dirfile in bits */
	long curbit;		       /* current bit number */
	long hmask;		       /* current hash mask */
	long blkptr;		       /* current block for nextkey */
	int keyptr;		       /* current key for nextkey */
	long blkno;		       /* current page to read/write */
	long pagbno;		       /* current page in pagbuf */
	char pagbuf[PBLKSIZ];	       /* page file block buffer */
	long dirbno;		       /* current block in dirbuf */
	char dirbuf[DBLKSIZ];	       /* directory file block buffer */
} SDBM;

#define SDBM_RDONLY	0x1	       /* data base open read-only */
#define SDBM_IOERR	0x2	       /* data base I/O error */

/*
 * utility macros
 */
#define sdbm_rdonly(db)		((db)->flags & SDBM_RDONLY)
#define sdbm_error(db)		((db)->flags & SDBM_IOERR)

#define sdbm_clearerr(db)	((db)->flags &= ~SDBM_IOERR)  /* ouch */

#define sdbm_dirfno(db)	((db)->dirf)
#define sdbm_pagfno(db)	((db)->pagf)

typedef struct {
	char *dptr;
	int dsize;
} datum;

extern datum nullitem;

/*
 * flags to sdbm_store
 */
#define SDBM_INSERT	0
#define SDBM_REPLACE	1

/*
 * ndbm interface
 */
extern SDBM *sdbm_open (char *, int, int);
extern void sdbm_close (SDBM *);
extern datum sdbm_fetch (SDBM *, datum);
extern int sdbm_delete (SDBM *, datum);
extern int sdbm_store (SDBM *, datum, datum, int);
extern datum sdbm_firstkey (SDBM *);
extern datum sdbm_nextkey (SDBM *);

/*
 * other
 */
extern SDBM *sdbm_prep (char *, char *, int, int);
extern long sdbm_hash (char *, int);

#ifdef __cplusplus
}
#endif

#endif
