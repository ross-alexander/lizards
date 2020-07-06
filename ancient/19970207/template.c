#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "compat/cio.h"
#include "template.h"

#include "strgfunc.h"
#include "tempfunc.h"

int enter_template (field_t **template, char **data, unsigned int start_field,
                    unsigned int *field_in, int (*end_key_func) (unsigned int *field,
                    field_t **template, char **data), int (*special_key_func)
                    (unsigned int *field, field_t **template, char **data),
                    int (*field_exit_func) (unsigned int *field,
                    field_t **template, char **data))
{
  char string [80],
       overflow_char = '\0';

  unsigned int present_field,
               special_key;

  present_field = start_field;

  while (1)
  {
    show_field (template [present_field], data [present_field]);

    if (data [present_field] != '\0')
      strcpy (string, data [present_field]);
    else
      string [0] = '\0';

    special_key = get_string (string, template [present_field]->length,
      template [present_field]->type, template [present_field]->spaced, 1,
      template [present_field]->overflow_f, &overflow_char);

    if (strlen (string) > 0)
    {
      if (data [present_field] != '\0')
      {
        free (data [present_field]);
        data [present_field] = '\0';
      }

      if ((data [present_field] = malloc (sizeof (char) *
        (strlen (string) + 1))) == '\0')
      {
        printf ("FATAL ERROR: Out of memory for malloc. Press SPACE.\n");

        getch ();

        exit (EXIT_FAILURE);
      }

      strcpy (data [present_field], string);
    } else {
      if (data [present_field] != '\0')
      {
        free (data [present_field]);
        data [present_field] = '\0';
      }
    }

    switch (special_key)
    {
      case SPECIAL_KEY:
        if (special_key_func != '\0')
          special_key_func (&present_field, template, data);

        break;
      case END_KEY:
        if (end_key_func != '\0')
          if (end_key_func (&present_field, template, data))
            clear_error ();
          else
            special_key = 0;

        break;
      case ERASE_KEY:
        *field_in = present_field;
        return (ERASE_KEY);
      case FINISH_KEY:
        *field_in = present_field;
        return (FINISH_KEY);
      case ESCAPE_KEY:
        *field_in = present_field;
        return (ESCAPE_KEY);
      case UP_KEY:
        if (template [present_field]->jump_up != 0)
          present_field += template [present_field]->jump_up;

        break;
      case DOWN_KEY:
        if (template [present_field]->jump_down != 0)
          present_field += template [present_field]->jump_down;

        break;
      case LEFT_KEY:
        if (template [present_field]->jump_up != 0)
          present_field --;

        break;
      case RIGHT_KEY:
        if (template [present_field]->jump_down != 0)
          present_field ++;

        break;
      case UP_FORM_KEY:
        *field_in = present_field;
        return (UP_FORM_KEY);
      case DOWN_FORM_KEY:
        *field_in = present_field;
        return (DOWN_FORM_KEY);
      default:
        present_field ++;

        if (field_exit_func != '\0')
          field_exit_func (&present_field, template, data);
    }

    if (template [present_field] == '\0')
    {
      present_field --;
      overflow_char = '\0';
    }
  }
}

void show_template (field_t **template, char **data)
{
  unsigned int loop;

  for (loop = 0; template [loop] != '\0'; loop ++)
    show_field (template [loop], data [loop]);
}

void show_field_range (unsigned int field_from, unsigned int field_to,
                       field_t **template, char **data)
{
  for (;field_from != field_to + 1; field_from ++)
    show_field (template [field_from], data [field_from]);
}

void show_field (field_t *field, char *data)
{
  unsigned int loop;

  gotoxy (field->scr_x, field->scr_y);

  for (loop = 0; loop < field->length; loop ++) cprintf ("_");

  gotoxy (field->scr_x, field->scr_y);

  if (data != '\0')
    for (loop = 0; data [loop] != '\0'; loop ++)
      if (data [loop] == ' ')
        cprintf ("_");
      else
        cprintf ("%c", data [loop]);
}

field_t **load_template (char *template_name, unsigned int *num_fields)
{
  FILE *fptr;

  char template_line [256],
       token [256];

  field_t new_field,
          **fields = '\0';

  if ((fptr = fopen (template_name, "rt")) == '\0')
  {
    printf ("FATAL ERROR: Unable to read %s file. Press SPACE.\n", template_name);
    exit (EXIT_FAILURE);
  }

  *num_fields = 0;

  while (fgets (template_line, 255, fptr) != '\0')
  {
    new_field.scr_y = atoi (strtoken (token, template_line, ","));
    new_field.scr_x = atoi (strtoken (token, template_line, ","));
    new_field.type = atoi (strtoken (token, template_line, ","));
    new_field.spaced = atoi (strtoken (token, template_line, ","));
    new_field.length = atoi (strtoken (token, template_line, ","));
    new_field.jump_down = atoi (strtoken (token, template_line, ","));
    new_field.jump_up = atoi (strtoken (token, template_line, ","));
    new_field.start_of_line_f = atoi (strtoken (token, template_line, ","));
    new_field.overflow_f = atoi (strtoken (token, template_line, ","));

    if (fields == '\0')
    {
      if ((fields = malloc (sizeof (field_t *) * ((*num_fields) + 2))) == '\0')
      {
        printf ("FATAL ERROR: Out of memory for malloc. Press SPACE.\n");

        getch ();

        exit (EXIT_FAILURE);
      }
    } else {
      if ((fields = realloc (fields, sizeof (field_t *) * ((*num_fields) + 2))) == '\0')
      {
        printf ("FATAL ERROR: Out of memory for realloc. Press SPACE.\n");

        getch ();

        exit (EXIT_FAILURE);
      }
    }

    if ((fields [*num_fields] = malloc (sizeof (field_t))) == '\0')
    {
      printf ("FATAL ERROR: Out of memory for malloc. Press SPACE.\n");

      getch ();

      exit (EXIT_FAILURE);
    }

    *fields [*num_fields] = new_field;

    (*num_fields) ++;
  }

  fields [*num_fields] = '\0';

  fclose (fptr);

  return (fields);
}

void free_data (field_t **template, char **data)
{
  unsigned int loop;

  if (data == '\0') return;

  for (loop = 0; template [loop] != '\0'; loop ++)
  {
    if (data [loop] != '\0')
    {
      free (data [loop]);
      data [loop] = '\0';
    }
  }

  free (data);
  data = '\0';
}

void free_template (field_t **template)
{
  unsigned int loop;

  if (template == '\0') return;

  for (loop = 0; template [loop] != '\0'; loop ++)
  {
    free (template [loop]);
    template [loop] = '\0';
  }

  free (template);
  template = '\0';
}

void set_field (field_t **template, char **data, unsigned int field_num,
  char *string)
{
  if (data [field_num] != '\0')
  {
    free (data [field_num]);
    data [field_num] = '\0';
  }

  if (strlen (string) > template [field_num]->length)
  {
    if ((data [field_num] = malloc (sizeof (char) *
      (template [field_num]->length + 1))) == '\0')
    {
      printf ("FATAL ERROR: Out of memory for malloc. Press SPACE.\n");

      getch ();

      exit (EXIT_FAILURE);
    }

    strncpy (data [field_num], string, template [field_num]->length);

    data [field_num][template [field_num]->length] = '\0';
  } else {
    if ((data [field_num] = malloc (sizeof (char) * (strlen (string) + 1))) == '\0')
    {
      printf ("FATAL ERROR: Out of memory for malloc. Press SPACE.\n");

      getch ();

      exit (EXIT_FAILURE);
    }

    strcpy (data [field_num], string);
  }
}

int get_string (char *string, unsigned int maxlen, unsigned int allowed,
                unsigned int space_f, unsigned int function_key_f,
                unsigned int overflow_f, char *overflow_char)
{
  int character,
      end_of_line = 0;
  unsigned int point;
  point = strlen (string);
  while (!end_of_line)
    {
      if (*overflow_char != '\0')
	{
	  if (overflow_f)
	    {
	      character = *overflow_char;
	      *overflow_char = '\0';
	  } else
	    *overflow_char = '\0';
      } else
	character = getch ();
      
      switch (character)
	{
	case '\n':
	case '\r':
	case KEY_ENTER:
	  end_of_line = 1;
	  break;
	case KEY_F(3):
	  string [point] = '\0';
	  return (ERASE_KEY);
	case KEY_F(4):
	  string [point] = '\0';
	  return (FINISH_KEY);
	case KEY_F(5):
	  string [point] = '\0';
	  return (ESCAPE_KEY);
	  /*
	     case '\x0':
        character = getch ();

        if (function_key_f)
        {
	switch (character)
	{
            case 59:
              string [point] = '\0';
              return (SPECIAL_KEY);
            case 60:
              string [point] = '\0';
              return (END_KEY);
            case 61:
              string [point] = '\0';
              return (ERASE_KEY);
            case 62:
              string [point] = '\0';
              return (FINISH_KEY);
            case 63:
              string [point] = '\0';
              return (ESCAPE_KEY);
            case 72:

              string [point] = '\0';
              return (UP_KEY);
            case 75:
            case 15:
              string [point] = '\0';
              return (LEFT_KEY);
            case 77:
              string [point] = '\0';
              return (RIGHT_KEY);
            case 80:
              string [point] = '\0';
              return (DOWN_KEY);
            case 73:
              string [point] = '\0';
              return (UP_FORM_KEY);
            case 81:
              string [point] = '\0';
              return (DOWN_FORM_KEY);
          }
	}
        break;
	*/
	case '\x09': /* tab key */
	  string [point] = '\0';
	  return (RIGHT_KEY);
	case 0x0008:
	case 0x007F:
	case KEY_BACKSPACE:
	case KEY_LEFT:
	  if (point > 0)
	    {
	      point --;
	      gotoxy (wherex () - 1, wherey ());
	      cprintf ("_");
	      gotoxy (wherex () - 1, wherey ());
	    }
	  break;
	default:
	  if (point < maxlen)
	    {
	      switch (allowed)
		{
		case NUMERIC:
		  if (isdigit (character) || (isspace (character) && space_f))
		    {
		      string [point ++] = character;
		      cprintf ("%c", character);
		    }

		  break;
		case ALPHA:
		  if (isalpha (character) || ispunct (character) ||
		      (isspace (character) && space_f))
		    {
		      string [point ++] = character;
		      
		      if (isspace (character))
			cprintf ("_");
		      else
			cprintf ("%c", character);
		    }
		  break;
		case ALPHANUMERIC:
		  if (isalnum (character) || ispunct (character) ||
		      (isspace (character) && space_f))
		    {
		      string [point ++] = character;
		      if (isspace (character))
			cprintf ("_");
		      else
			cprintf ("%c", character);
		    }
		  break;
		}
	  }
	  else
	    {        /* character overflows string length */
	      if (overflow_f)
		{
		  *overflow_char = character;
		  string [point] = '\0';
		  return (0);
		}
	    }
	  break;
	}
    }
  string [point] = '\0';
  return 0;
}

void show_error (char *error)
{
  unsigned int x, y;

  x = wherex ();
  y = wherey ();

  gotoxy (1, 25);
  cprintf ("%s", error);
  clreol ();
  printf ("\a");
  gotoxy (x, y);
}

void clear_error (void)
{
  unsigned int x, y;

  x = wherex ();
  y = wherey ();

  gotoxy (1, 25);
  clreol ();
  gotoxy (x, y);
}
