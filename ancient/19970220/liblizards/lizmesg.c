#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "lizard.h"

#include "lizmesg.h"

extern char *message_list [MAX_RANDOM_MESSAGES];

void get_message_file (void)
{
  FILE *fptr;

  char work_string [255],
       filename [255];

  int work;

  for (work = 0; work < MAX_RANDOM_MESSAGES; work ++)
    message_list [work] = NULL;

  sprintf (filename, "%s", LIZ_MESG_F);

  if ((fptr = fopen (filename, "rt")) == NULL)
  {
    printf ("FATAL ERROR: Unable to read %s file. Press SPACE.\n", LIZ_MESG_F);

    getch ();

    exit (EXIT_FAILURE);
  }

  work = 0;

  while (fgets (work_string, 255, fptr) != NULL)
  {
    if (work_string [0] != '/')
    {
      if (work_string [strlen (work_string) - 1] == '\n');
	work_string [strlen (work_string) - 1] = NULL;

      if ((message_list [work] = malloc (strlen (work_string) + 1)) == NULL)
      {
        printf ("FATAL ERROR: Out of memory. Press SPACE.\n");

        getch ();

	exit (EXIT_FAILURE);
      } else
	strcpy (message_list [work], work_string);

      work ++;
    }
  };

  fclose (fptr);
}

char *build_mesg (int mesg, char *hex_str, char *player_str)
{
  char work_string [500];

  int work,
      work2;

  work_string [0] = NULL;

  for (work = 0, work2 = 0; work < strlen (message_list [mesg]); work ++)
    if (message_list [mesg][work] == '%')
    {
      work ++;

      switch (message_list [mesg][work])
      {
	case 'h':
	  strcat (work_string, hex_str);

	  work2 = strlen (work_string);
	  break;
	case 'p':
	  strcat (work_string, player_str);

	  work2 = strlen (work_string);
	  break;
	default:
	  work ++;
	  break;
      }
    } else {
      work_string [work2] = message_list [mesg][work];

      work2 ++;

      work_string [work2] = NULL;
    }

  work_string [work2] = NULL;

  return (work_string);
}
