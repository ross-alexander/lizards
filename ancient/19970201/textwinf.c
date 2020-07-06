#include <conio.h>
#include <stdlib.h>
#include <stdio.h>

#include "textwind.h"
#include "textwinf.h"

void open_text_window (win_t *wind, int left, int top, int right, int bottom,
		       int fore_colour, int back_colour, int bordered,
		       int topl_b, int topr_b, int edge_b, int side_b,
		       int botl_b, int botr_b)
{
  register int loop;

  window (1, 1, 80, 25);

  textcolor (fore_colour);
  textbackground (back_colour);

  wind->fore_colour = fore_colour;
  wind->back_colour = back_colour;

  if (bordered)
  {
    gotoxy (left, top);
    putch (topl_b);

    for (loop = left + 1; loop < right; loop ++)
      putch (edge_b);

    putch (topr_b);

    for (loop = top + 1; loop < bottom; loop ++)
    {
      gotoxy (left, loop);
      putch (side_b);

      gotoxy (right, loop);
      putch (side_b);
    }

    gotoxy (left, bottom);

    putch (botl_b);

    for (loop = left + 1; loop < right; loop ++)
      putch (edge_b);

    putch (botr_b);

    wind->bordered = 1;

    window (left + 1, top + 1, right - 1, bottom - 1);
  } else {
    wind->bordered = 0;

    window (left, top, right, bottom);
  }

  clrscr ();

  wind->left = left;
  wind->top = top;
  wind->right = right;
  wind->bottom = bottom;
}

void reopen_text_window (win_t *wind)
{
  textcolor (wind->fore_colour);
  textbackground (wind->back_colour);

  if (wind->bordered)
    window (wind->left + 1, wind->top + 1, wind->right - 1, wind->bottom - 1);
  else
    window (wind->left, wind->top, wind->right, wind->bottom);
}

void close_text_window (win_t *wind)
{
  window (wind->left, wind->top, wind->right, wind->bottom);

  textcolor (LIGHTGRAY);
  textbackground (BLACK);

  clrscr ();

  window (1, 1, 80, 25);
}
