/**
 ** FILEFUNC.C. A set of pesimistic binary file record manipulation routines.
 **/

#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>

#include "filefunc.h"

/* ----------------------------------------------------------------------
--
-- filelength
--
---------------------------------------------------------------------- */
off_t filelength(int fd)
{
  struct stat sb;

  if (fstat(fd, &sb) == -1)
    {
      fprintf(stderr, "Error trying to stat file\n");
      exit(1);
    }
  return sb.st_size;
}

/**
 ** Return the number of records of length size_rec in open file.
 **/

unsigned int num_records ( FILE *fptr, unsigned int size_rec )
{
  return ( (unsigned int) ( filelength ( fileno ( fptr )) /
    ( long ) size_rec ));
}

/**
 ** Return to number of records of length size_rec in closed file of
 ** fname. Opens the file for binary read and then closes afterwards.
 **/

unsigned int fname_num_records ( char *fname, unsigned int size_rec )
{
  FILE *fptr;

  unsigned int records;

  if (( fptr = fopen ( fname, "rb" )) == NULL )
    return ( 0 ); /* unopenable file */

  records = num_records ( fptr, size_rec );

  fclose ( fptr );

  return ( records );
}

/**
 ** Set file pointer in open file fptr to the beginning of record rec_num
 ** in a file of records of length size_rec.
 **/

void set_rec ( FILE *fptr, unsigned int size_rec, unsigned int rec_num )
{
  fseek ( fptr, (( long ) size_rec * ( rec_num )), SEEK_SET );
}

/**
 ** Retrieve record rec_num from an open file of records of length size_rec.
 **/

void get_rec ( FILE *fptr, unsigned int size_rec, unsigned int rec_num,
	       void *rec )
{
  set_rec ( fptr, size_rec, rec_num ); /* set to beginning of record */

  fread ( rec, size_rec, 1, fptr );
}

/**
 ** Save record rec_num to an open file of records of length size_rec.
 **/

void put_rec ( FILE *fptr, unsigned int size_rec, unsigned int rec_num,
	       void *rec )
{
  set_rec ( fptr, size_rec, rec_num ); /* set to beginning of record */

  fwrite ( rec, size_rec, 1, fptr );
}

/**
 ** Retrieve record rec_num from a closed file of fname, containing records
 ** of length size_rec.
 **/

void fname_get_rec ( char *fname, unsigned int size_rec, unsigned int rec_num,
		    void *rec )
{
  FILE *fptr;

  if (( fptr = fopen ( fname, "rb" )) == NULL )
  {
    printf ( "FATAL ERROR: Unable to read %s file.\n", fname );
    exit (EXIT_FAILURE);
  }

  get_rec ( fptr, size_rec, rec_num, rec );

  fclose ( fptr );
}

/**
 ** Save record rec_num to a closed file of fname, containing records
 ** of length size_rec.
 **/

void fname_put_rec ( char *fname, unsigned int size_rec, unsigned int rec_num,
		    void *rec )
{
  FILE *fptr;

  if (( fptr = fopen ( fname, "r+b" )) == NULL )
  {
    printf ( "FATAL ERROR: Unable to read %s file. Press SPACE.\n", fname );
    exit (EXIT_FAILURE);
  }

  put_rec ( fptr, size_rec, rec_num, rec );

  fclose ( fptr );
}

/**
 ** Erase a closed file of fname by opening the file in write mode.
 **/

void zero_file ( char *fname )
{
  FILE *fptr;

  if (( fptr = fopen ( fname, "wb" )) == NULL )
  {
    printf ( "FATAL ERROR: Unable to write to %s file. Press SPACE.\n", fname );
    exit (EXIT_FAILURE);
  }

  fclose ( fptr );
}

/**
 ** Fill a closed file of fname with recs records of size length.
 **/

void fill_file ( char *fname, size_t size, unsigned int recs,
		 void *record )
{
  FILE *fptr;

  unsigned int loop;

  if (( fptr = fopen ( fname, "wb" )) == NULL )
  {
    printf ( "FATAL ERROR: Unable to write %s file. Press SPACE.\n", fname );
    exit (EXIT_FAILURE);
  }

  for ( loop = 0; loop < recs; loop ++ )
    put_rec ( fptr, size, loop, record );

  fclose ( fptr );
}
