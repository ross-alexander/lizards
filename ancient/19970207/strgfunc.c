#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "strgfunc.h"

/*
   FUNCTION: make_token_table

   Build a NULL terminated malloced table of string tokens from supplied string.
   Each token is part of string seperated by delimiter characters.

   PARAMETERS:

   char **token_table : table pointer, also returned by function. Points to
                        table of string pointers to malloced strings. End of
                        table is marked by a NULL pointer. Table is also
                        returned by function.

   char *string : string consisting of tokens seperated by delimiter characters.

   char *delims : each character in this string is a delimiter used to break up
                  the supplied string into tokens.

   RETURNS:

   Pointer to malloced table of string pointers to malloced strings.

   NOTES:

   Any tables built with this function should be freed (with
   free_malloced_table) when finished with.
*/

char **make_token_table ( char **token_table, char *string, char *delims )
{
  char *token_string;

  unsigned int tokens = 0;

  /* see if there are ANY tokens */

  if (( token_string = strtok ( string, delims )) == NULL )
    return ( NULL ); /* nope, give them nothing back */

  /* while strtok is not returning NULL, build malloc table of tokens */

  do
  {
    /* if table is undefined, malloc it */

    if ( token_table == NULL )
    {
      if (( token_table = malloc ( sizeof ( char * ) * 2)) == NULL )
      {
        printf ("FATAL ERROR: Out of memory for malloc. Press SPACE.\n");

        getch ();

        exit (EXIT_FAILURE);
      }
    } else {
      /* require space for another token entry, increase table length by one */

      if (( token_table = realloc ( token_table, sizeof ( char * ) * ( tokens + 2 )))
	    == NULL )
      {
        printf ("FATAL ERROR: Out of memory for realloc. Press SPACE.\n");

        getch ();

        exit (EXIT_FAILURE);
      }
    }

    /* now that we have space in table for entry, see if we can actually
       malloc entry itself */

    if (( token_table [ tokens ] = malloc ( sizeof ( char ) *
	  ( strlen ( token_string ) + 1 ))) == NULL )
    {
      printf ("FATAL ERROR: Out of memory for malloc. Press SPACE.\n");

      getch ();

      exit (EXIT_FAILURE);
    }

    /* add token to table */

    strcpy ( token_table [ tokens ], token_string );

    tokens ++;
  } while (( token_string = strtok ( NULL, delims )) != NULL );

  /* terminate token table with NULL */

  token_table [ tokens ] = NULL;

  return ( token_table );
}

/*
   FUNCTION: free_malloced_table

   Frees the memory used by malloced table by freeing the table. This destroys
   the table and the information it contains.

   PARAMETERS:

   char **malloced_table : malloced table built using above functions
                           (make_arg_table or make_token_table) that consists
                           of a malloced table of string pointers, with the
                           table end marked by a NULL pointer.

*/

void free_malloced_table ( char **malloced_table )
{
  unsigned int token = 0;

  if ( malloced_table == NULL) return; /* do nothing, table is empty */

  while ( malloced_table [ token ] != NULL ) /* free each string pointer */
    free ( malloced_table [ token ++ ] ); 

  free ( malloced_table ); /* free the table of string pointers */

  malloced_table = NULL; /* mark table as empty */
}

char *flay_spaces (char *string)
{
  char temp_string [80];

  int work,
      work2,
      work3;

  for (work = 0; work < strlen (string); work ++)
    if (string [work] != ' ')
      break;

  work3 = (strlen (string) - work);

  for (work2 = 0; work2 < work3; work2 ++)
  {
    temp_string [work2] = string [work];

    work ++;
  }

  temp_string [work2] = NULL;

  strcpy (string, temp_string);

  return (string);
}

char *strsplt (char *dest, char *source, int start)
{
  int work,
      work2;

  for (work = start, work2 = 0; work < strlen (source); work ++, work2 ++)
    dest [work2] = source [work];

  dest [work2] = NULL;

  return (dest);
}

char *strtoken (char *dest, char *source, char *delim)
{
  unsigned int loop,
           loop_two,
           length;

  length = strlen (source);

  for (loop = 0; loop < length; loop ++)
    if (strchr (delim, (int) source [loop]) != NULL)
      break;
    else
      dest [loop] = source [loop];

  dest [loop] = NULL;

  if (loop == length)
    source [0] = NULL;
  else {
    for (loop ++, loop_two = 0; source [loop] != NULL; loop ++, loop_two ++)
      source [loop_two] = source [loop];

    source [loop_two] = NULL;
  }

  return (dest);
}

char *chop_space (char *string)
{
  int work;

  for (work = strlen (string) - 1; work > -1; work --)
    if (string [work] != ' ')
      return (string);
    else
      string [work] = NULL;

  return (string);
}

/*
   add_to_strlist

   adds the passed entry string to the passed strlist (a dynamically
   allocated list of pointers to dynamically allocated strings) by either
   reallocing an existing list and adding the string, or mallocing a new
   list and adding the string. Passing a strlist of NULL starts a new list.

   eg. list = add_to_strlist (list, "TESTING..."); adds to existing list

   eg. list = add_to_strlist (NULL, "TESTING..."); creates new list

   add_to_strlist returns a pointer to the modified list.
*/

char **add_to_strlist ( char **strlist, char *entry )
{
  int elements;

  if ( strlist == NULL ) /* put entry in NEW table */
  {
    elements = 1;

    if (( strlist = malloc ( sizeof ( char * ) * 2 )) == NULL )
      return ( NULL ); /* failed to malloc new list, give them NULL back */
  } else {
    elements = strlist_len ( strlist ) + 1;

    if (( strlist = realloc ( strlist, sizeof ( char * ) * ( elements + 1))) == NULL )
      return ( NULL ); /* failed to realloc list, give them NULL back */
  }

  if (( strlist [ elements - 1 ] = malloc ( sizeof ( char ) *
    ( strlen ( entry ) + 1 ))) == NULL )
    return ( NULL ); /* unable to malloc space in list for new entry */

  strcpy ( strlist [ elements - 1 ], entry );

  strlist [ elements ] = NULL; /* table terminating NULL */

  return ( strlist ); /* give them back new copy of strlist */
}

/*
   strlist_len

   counts and returns the number of entries in the strlist passed to it.
*/

int strlist_len ( char **strlist )
{
  int element;

  for ( element = 0; strlist [ element ] != NULL; element ++ );

  return ( element );
}

/*
   free_strlist

   frees the memory used to hold each string in the strlist as well as the
   memory used by the strlist itself. Returns NULL so you can easily kill
   lists.

   eg. list = free_strlist (list);
*/

char **free_strlist ( char **strlist )
{
  int element = 0;

  if ( strlist == NULL ) return ( NULL ); /* do nothing as given empty list */

  for ( element = 0; strlist [ element ] != NULL; element ++ )
  {
    free ( strlist [ element ] ); /* free each element */

    strlist [ element ] = NULL;
  }

  free ( strlist ); /* free list itself */

  return ( NULL );
}
