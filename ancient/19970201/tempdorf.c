#include <conio.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
       overflow_char = NULL;

  unsigned int present_field,
               special_key;

  present_field = start_field;

  while (1)
  {
    show_field (template [present_field], data [present_field]);

    if (data [present_field] != NULL)
      strcpy (string, data [present_field]);
    else
      string [0] = NULL;

    special_key = get_string (string, template [present_field]->length,
      template [present_field]->type, template [present_field]->spaced, 1,
      template [present_field]->overflow_f, &overflow_char);

    if (strlen (string) > 0)
    {
      if (data [present_field] != NULL)
      {
        free (data [present_field]);
        data [present_field] = NULL;
      }

      if ((data [present_field] = malloc (sizeof (char) *
        (strlen (string) + 1))) == NULL)
      {
        printf ("FATAL ERROR: Out of memory for malloc. Press SPACE.\n");

        getch ();

        exit (EXIT_FAILURE);
      }

      strcpy (data [present_field], string);
    } else {
      if (data [present_field] != NULL)
      {
        free (data [present_field]);
        data [present_field] = NULL;
      }
    }

    switch (special_key)
    {
      case SPECIAL_KEY:
        if (special_key_func != NULL)
          special_key_func (&present_field, template, data);

        break;
      case END_KEY:
        if (end_key_func != NULL)
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

        if (field_exit_func != NULL)
          field_exit_func (&present_field, template, data);
    }

    if (template [present_field] == NULL)
    {
      present_field --;
      overflow_char = NULL;
    }
  }
}

void show_template (field_t **template, char **data)
{
  unsigned int loop;

  for (loop = 0; template [loop] != NULL; loop ++)
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

  printf ("\x1B[%d;%dH", field->scr_y, field->scr_x);

  for (loop = 0; loop < field->length; loop ++) printf ("_");

  printf ("\x1B[%d;%dH", field->scr_y, field->scr_x);

  if (data != NULL)
    for (loop = 0; data [loop] != NULL; loop ++)
      if (data [loop] == ' ')
        printf ("_");
      else
        printf ("%c", data [loop]);
}

field_t **load_template (char *template_name, unsigned int *num_fields)
{
  FILE *fptr;

  char template_line [256],
       token [256];

  field_t new_field,
          **fields = NULL;

  if ((fptr = fopen (template_name, "rt")) == NULL)
  {
    printf ("FATAL ERROR: Unable to read %s file. Press SPACE.\n", template_name);

    getch ();

    exit (EXIT_FAILURE);
  }

  *num_fields = 0;

  while (fgets (template_line, 255, fptr) != NULL)
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

    if (fields == NULL)
    {
      if ((fields = malloc (sizeof (field_t *) * ((*num_fields) + 2))) == NULL)
      {
        printf ("FATAL ERROR: Out of memory for malloc. Press SPACE.\n");

        getch ();

        exit (EXIT_FAILURE);
      }
    } else {
      if ((fields = realloc (fields, sizeof (field_t *) * ((*num_fields) + 2))) == NULL)
      {
        printf ("FATAL ERROR: Out of memory for realloc. Press SPACE.\n");

        getch ();

        exit (EXIT_FAILURE);
      }
    }

    if ((fields [*num_fields] = malloc (sizeof (field_t))) == NULL)
    {
      printf ("FATAL ERROR: Out of memory for malloc. Press SPACE.\n");

      getch ();

      exit (EXIT_FAILURE);
    }

    *fields [*num_fields] = new_field;

    (*num_fields) ++;
  }

  fields [*num_fields] = NULL;

  fclose (fptr);

  return (fields);
}

void free_data (field_t **template, char **data)
{
  unsigned int loop;

  if (data == NULL) return;

  for (loop = 0; template [loop] != NULL; loop ++)
  {
    if (data [loop] != NULL)
    {
      free (data [loop]);
      data [loop] = NULL;
    }
  }

  free (data);
  data = NULL;
}

void free_template (field_t **template)
{
  unsigned int loop;

  if (template == NULL) return;

  for (loop = 0; template [loop] != NULL; loop ++)
  {
    free (template [loop]);
    template [loop] = NULL;
  }

  free (template);
  template = NULL;
}

void set_field (field_t **template, char **data, unsigned int field_num,
  char *string)
{
  if (data [field_num] != NULL)
  {
    free (data [field_num]);
    data [field_num] = NULL;
  }

  if (strlen (string) > template [field_num]->length)
  {
    if ((data [field_num] = malloc (sizeof (char) *
      (template [field_num]->length + 1))) == NULL)
    {
      printf ("FATAL ERROR: Out of memory for malloc. Press SPACE.\n");

      getch ();

      exit (EXIT_FAILURE);
    }

    strncpy (data [field_num], string, template [field_num]->length);

    data [field_num][template [field_num]->length] = NULL;
  } else {
    if ((data [field_num] = malloc (sizeof (char) * (strlen (string) + 1))) == NULL)
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
    if (*overflow_char != NULL)
    {
      if (overflow_f)
      {
        character = *overflow_char;
        *overflow_char = NULL;
      } else
        *overflow_char = NULL;
    } else
      character = getch ();

    switch (character)
    {
      case 4:
	if (function_key_f)
	{
	  string [point] = NULL;
	  return (SPECIAL_KEY);
	}
	break;
      case 22:
	if (function_key_f)
	{
	  string [point] = NULL;
	  return (END_KEY);
	}
	break;
      case 5:
	if (function_key_f)
	{
	  string [point] = NULL;
	  return (ERASE_KEY);
	}
	break;
      case 6:
	if (function_key_f)
	{
	  string [point] = NULL;
	  return (FINISH_KEY);
	}
	break;
      case 1:
	if (function_key_f)
	{
	  string [point] = NULL;
	  return (ESCAPE_KEY);
	}
	break;
      case 9:
	string [point] = NULL;
	return (UP_KEY);
      case 10:
	string [point] = NULL;
	return (LEFT_KEY);
      case 12:
	string [point] = NULL;
	return (RIGHT_KEY);
      case 11:
	string [point] = NULL;
	return (DOWN_KEY);
      case '\r':
	end_of_line = 1;
	break;
      case '\b':
	if (point > 0)
	{
	  point --;
	  printf ("\x1B[1D");
	  printf ("_");
	  printf ("\x1B[1D");
	}

	break;
      default:
	character = toupper (character);

	if (point < maxlen)
	{
	  switch (allowed)
	  {
	    case NUMERIC:
	      if (isdigit (character) || (isspace (character) && space_f))
	      {
		string [point ++] = character;
		printf ("%c", character);
	      }

	      break;
	    case ALPHA:
	      if (isalpha (character) || ispunct (character) ||
		(isspace (character) && space_f))
	      {
                string [point ++] = character;

                if (isspace (character))
                  printf ("_");
                else
                  printf ("%c", character);
              }

              break;
            case ALPHANUMERIC:
              if (isalnum (character) || ispunct (character) ||
                (isspace (character) && space_f))
              {
                string [point ++] = character;

                if (isspace (character))
                  printf ("_");
                else
                  printf ("%c", character);
              }

              break;
          }
        } else {        /* character overflows string length */
          if (overflow_f)
          {
            *overflow_char = character;

            string [point] = NULL;

            return (0);
          }
        }

        break;
    }
  }

  string [point] = NULL;

  return (0);
}

void show_error (char *error)
{
  printf ("\x1B[s");
  printf ("\x1B[%d;%dH", 24, 1);
  printf ("%s", error);
  printf ("\x1B[K");
  printf ("\a");
  printf ("\x1B[u");
}

void clear_error (void)
{
  printf ("\x1B[s");
  printf ("\x1B[%d;%dH", 24, 1);
  printf ("\x1B[K");
  printf ("\x1B[u");
}
