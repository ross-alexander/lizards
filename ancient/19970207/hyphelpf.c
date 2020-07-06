#include <conio.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "hyplink.h"

#include "hyphelpf.h"
#include "strgfunc.h"

char **open_help_file (FILE **help_fptr, char *help_file_name,
		       char *index_file_name)
{
  FILE *index_fptr;

  char **index = NULL;

  if ((index_fptr = fopen (index_file_name, "rt")) == NULL)
  {
    printf ("FATAL ERROR: Unable to read %s file. Press SPACE.\n", index_file_name);

    getch ();

    exit (EXIT_FAILURE);
  }

  index = load_index_file (index, index_fptr);

  if (((*help_fptr) = fopen (help_file_name, "rt")) == NULL)
  {
    printf ("FATAL ERROR: Unable to read %s file. Press SPACE.\n",
      help_file_name);

    getch ();

    exit (EXIT_FAILURE);
  }

  return (index);
}

char **load_index_file (char **index, FILE *index_fptr)
{
  char string [256];

  while (fgets (string, 255, index_fptr) != NULL)
  {
    if (string [strlen (string) - 1] == '\n')
      string [strlen (string) - 1] = NULL;

    index = add_to_strlist (index, string);
  }

  return (index);
}

long find_index (char **index, char *entry)
{
  char index_entry [256],
       entry_copy [256],
       *char_ptr = NULL;

  unsigned int loop;

  long offset;

  for (loop = 0; index [loop] != NULL; loop ++)
  {
    strcpy (index_entry, index [loop]);
    char_ptr = strchr (index_entry, '@');
    char_ptr [0] = NULL;

    strcpy (entry_copy, entry);

    if (strcmp (index_entry, strupr (entry_copy)) == 0)
    {
      char_ptr ++;
      offset = atol (char_ptr);

      return (offset);
    }
  }

  return (-1);
}

void enter_help (FILE *help_fptr, char **index)
{
  enter_help_page (help_fptr, index, "START");
}

void enter_help_page (FILE *help_fptr, char **index, char *page_index)
{
  int number_links = 0,
      present_link = 0,
      result = 0;

  unsigned int pages = 0,
	       present_page = 0;

  long offset = -1;

  hyper_link_t *hyper_links = NULL;

  offset = find_index (index, page_index);

  if (offset == -1) return;

  number_links = display_and_link_help_page (help_fptr, index, offset,
    present_page, &hyper_links, &pages);

  while (1)
  {
    result = select_hyper_link (number_links, hyper_links, &present_link,
      present_page, pages);

    switch (result)
    {
      case -1:
	free (hyper_links);
	hyper_links = NULL;

	return;
      case -2:
	if (present_page < pages)
	{
	  present_page ++;
	  display_help_page (help_fptr, index, offset, present_page);
	}

	break;
      case -3:
	if (present_page > 0)
	{
	  present_page --;
	  display_help_page (help_fptr, index, offset, present_page);
	}

	break;
      default:
	enter_help_page (help_fptr, index, hyper_links [present_link].text);
	display_help_page (help_fptr, index, offset, present_page);
	break;
    }
  }
}

void display_help_page (FILE *help_fptr, char **index, long offset,
			unsigned int page)
{
  char string [256];

  int loop,
      help_end_f = 0,
      link_f = 0,
      text_lines = 1,
      present_page = 0;

  if (offset == -1) return;

  clrscr ();

  fseek (help_fptr, offset, SEEK_SET);

  help_end_f = 0;

  if (page != 0)
  {
    cprintf ("--- MORE ABOVE (PAGE UP) ---\n\r");
  } else
    gotoxy (1, 2);

  while (fgets (string, 255, help_fptr) != NULL && !help_end_f)
  {
    switch (string [0])
    {
      case '!':
	help_end_f = 1;

	break;
      default:
	for (loop = 0; string [loop] != NULL; loop ++)
	  switch (string [loop])
	  {
	    case '\n':
	      if (present_page == page)
		cprintf ("\n\r");

	      text_lines ++;

	      if (text_lines > 23)
	      {
		if (present_page == page)
		  cprintf ("--- MORE BELOW (PAGE DOWN) ---");

		text_lines = 1;
		present_page ++;
	      }

	      break;
	    case '@':
	      if (!link_f)
	      {
		link_f = 1;
		textattr (WHITE + (BLACK << 4));
	      } else {
		link_f = 0;
		textattr (LIGHTGRAY + (BLACK << 4));
	      }

	      break;
	    default:
	      if (present_page == page)
		cprintf ("%c", string [loop]);
	  }
    }
  }
}

int display_and_link_help_page (FILE *help_fptr, char **index, long offset,
				unsigned int page, hyper_link_t **links,
				unsigned int *pages)
{
  char string [256];

  int loop,
      link_loop = 0,
      link_f = 0,
      number_links = 0,
      help_end_f = 0,
      text_lines = 1,
      text_column = 0;

  hyper_link_t work_link;

  if (offset == -1) return -1;

  clrscr ();

  *pages = 0;

  fseek (help_fptr, offset, SEEK_SET);

  help_end_f = 0;

  if (page != 0)
  {
    cprintf ("--- MORE ABOVE (PAGE UP) ---\n\r");
  } else
    gotoxy (1, 2);

  while (fgets (string, 255, help_fptr) != NULL && !help_end_f)
  {
    switch (string [0])
    {
      case '!':
	help_end_f = 1;

	break;
      default:
	for (loop = 0; string [loop] != NULL; loop ++)
	  switch (string [loop])
	  {
	    case '\n':
	      if (*pages == page)
		cprintf ("\n\r");

	      text_lines ++;
	      text_column = 0;

	      if (text_lines > 23)
	      {
		if (*pages == page)
		  cprintf ("--- MORE BELOW (PAGE DOWN) ---");

		text_lines = 1;
		(*pages) ++;
	      }

	      break;
	    case '@':
	      if (!link_f)
	      {
		link_f = 1;
		textattr (WHITE + (BLACK << 4));
		work_link.page = *pages;
		work_link.column = text_column + 1;
		work_link.row = text_lines + 1;
		link_loop = 0;
	      } else {
		link_f = 0;
		textattr (LIGHTGRAY + (BLACK << 4));
		work_link.text [link_loop] = NULL;
		work_link.length = text_column + 1 - work_link.column;

		if ((*links) == NULL)
		{
		  if (((*links) = malloc (sizeof (hyper_link_t))) == NULL)
		  {
		    printf ("FATAL ERROR: Out of memory for malloc. Press SPACE.\n");

		    getch ();

		    exit (EXIT_FAILURE);
		  }
		} else {
		  if (((*links) = realloc (*links, sizeof (hyper_link_t) *
		    (number_links + 1))) == NULL)
		  {
		    printf ("FATAL ERROR: Out of memory for realloc. Press SPACE.\n");

		    getch ();

		    exit (EXIT_FAILURE);
		  }
		}

		(*links) [number_links] = work_link;
		number_links ++;
	      }

	      break;
	    default:
	      if (*pages == page)
		cprintf ("%c", string [loop]);

	      text_column ++;

	      if (link_f)
	      {
		work_link.text [link_loop] = string [loop];
		link_loop ++;
	      }
	  }
    }
  }

  return number_links;
}

int select_hyper_link (int number_links, hyper_link_t *links,
		       int *present_link, unsigned int present_page,
		       unsigned int pages)
{
  int option = NULL;

  /* now should have a list of active links the user can tab around in */

  *present_link = 0;

  if (number_links > 0)
  {
    /* find first link on present page */

    while (links [*present_link].page != present_page &&
      *present_link < number_links)
      (*present_link) ++;

    if (*present_link < number_links)
      show_hyper_link (links, *present_link, BLACK + (LIGHTGRAY << 4),
	LIGHTGRAY + (BLACK << 4));
  }

  while (option != '\x1B')
  {
    option = getch ();

    switch (option)
    {
      case 0:
	option = getch ();

	switch (option)
	{
	  case 81: /* page down */
	    if (pages > 0)
	      return (-2);
	  case 73: /* page up */
	    if (present_page != 0)
	      return (-3);
	  case 15: /* shift-tab */
	    if (number_links > 0 && *present_link < number_links)
	    {
	      show_hyper_link (links, *present_link, WHITE + (BLACK << 4),
		LIGHTGRAY + (BLACK << 4));

	      do {
		(*present_link) --;

		if (*present_link < 0 ) *present_link = number_links - 1;
	      } while (links [*present_link].page != present_page);

	      show_hyper_link (links, *present_link, BLACK + (LIGHTGRAY << 4),
		LIGHTGRAY + (BLACK << 4));
	    }

	    break;
	}

	break;
      case '\x09':
	if (number_links > 0 && *present_link < number_links)
	{
	  show_hyper_link (links, *present_link, WHITE + (BLACK << 4),
	    LIGHTGRAY + (BLACK << 4));

	  do {
	    (*present_link) ++;

	    if (*present_link >= number_links) *present_link = 0;
	  } while (links [*present_link].page != present_page);

	  show_hyper_link (links, *present_link, BLACK + (LIGHTGRAY << 4),
	    LIGHTGRAY + (BLACK << 4));
	}

	break;
      case '\r':
	if (*present_link < number_links)
	  return (0);
    }
  }

  return (-1);
}

void show_hyper_link (hyper_link_t *links, unsigned int present_link,
		      int hyper_attr, int normal_attr)
{
  gotoxy (links [present_link].column, links [present_link].row);

  textattr (hyper_attr);

  cprintf ("%s", links [present_link].text);

  textattr (normal_attr);
}