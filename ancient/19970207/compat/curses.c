#include "lzcurses.h"
#include <alloca.h>

chtype attributes[] =
{
    A_NORMAL,			/* screen_attr */
    A_NORMAL,			/* shadow_attr */
    A_NORMAL,			/* dialog_attr */
    A_BOLD,			/* title_attr */
    A_NORMAL,			/* border_attr */
    A_REVERSE,			/* button_active_attr */
    A_DIM,			/* button_inactive_attr */
    A_REVERSE,			/* button_key_active_attr */
    A_BOLD,			/* button_key_inactive_attr */
    A_REVERSE,			/* button_label_active_attr */
    A_NORMAL,			/* button_label_inactive_attr */
    A_NORMAL,			/* inputbox_attr */
    A_NORMAL,			/* inputbox_border_attr */
    A_NORMAL,			/* searchbox_attr */
    A_BOLD,			/* searchbox_title_attr */
    A_NORMAL,			/* searchbox_border_attr */
    A_BOLD,			/* position_indicator_attr */
    A_NORMAL,			/* menubox_attr */
    A_NORMAL,			/* menubox_border_attr */
    A_NORMAL,			/* item_attr */
    A_REVERSE,			/* item_selected_attr */
    A_BOLD,			/* tag_attr */
    A_REVERSE,			/* tag_selected_attr */
    A_BOLD,			/* tag_key_attr */
    A_REVERSE,			/* tag_key_selected_attr */
    A_BOLD,			/* check_attr */
    A_REVERSE,			/* check_selected_attr */
    A_BOLD,			/* uarrow_attr */
    A_BOLD			/* darrow_attr */
};

int color_table[][3] =
{
    {SCREEN_FG, SCREEN_BG, SCREEN_HL},
    {SHADOW_FG, SHADOW_BG, SHADOW_HL},
    {DIALOG_FG, DIALOG_BG, DIALOG_HL},
    {TITLE_FG, TITLE_BG, TITLE_HL},
    {BORDER_FG, BORDER_BG, BORDER_HL},
    {BUTTON_ACTIVE_FG, BUTTON_ACTIVE_BG, BUTTON_ACTIVE_HL},
    {BUTTON_INACTIVE_FG, BUTTON_INACTIVE_BG, BUTTON_INACTIVE_HL},
    {BUTTON_KEY_ACTIVE_FG, BUTTON_KEY_ACTIVE_BG, BUTTON_KEY_ACTIVE_HL},
    {BUTTON_KEY_INACTIVE_FG, BUTTON_KEY_INACTIVE_BG, BUTTON_KEY_INACTIVE_HL},
    {BUTTON_LABEL_ACTIVE_FG, BUTTON_LABEL_ACTIVE_BG, BUTTON_LABEL_ACTIVE_HL},
    {BUTTON_LABEL_INACTIVE_FG, BUTTON_LABEL_INACTIVE_BG,
     BUTTON_LABEL_INACTIVE_HL},
    {INPUTBOX_FG, INPUTBOX_BG, INPUTBOX_HL},
    {INPUTBOX_BORDER_FG, INPUTBOX_BORDER_BG, INPUTBOX_BORDER_HL},
    {SEARCHBOX_FG, SEARCHBOX_BG, SEARCHBOX_HL},
    {SEARCHBOX_TITLE_FG, SEARCHBOX_TITLE_BG, SEARCHBOX_TITLE_HL},
    {SEARCHBOX_BORDER_FG, SEARCHBOX_BORDER_BG, SEARCHBOX_BORDER_HL},
    {POSITION_INDICATOR_FG, POSITION_INDICATOR_BG, POSITION_INDICATOR_HL},
    {MENUBOX_FG, MENUBOX_BG, MENUBOX_HL},
    {MENUBOX_BORDER_FG, MENUBOX_BORDER_BG, MENUBOX_BORDER_HL},
    {ITEM_FG, ITEM_BG, ITEM_HL},
    {ITEM_SELECTED_FG, ITEM_SELECTED_BG, ITEM_SELECTED_HL},
    {TAG_FG, TAG_BG, TAG_HL},
    {TAG_SELECTED_FG, TAG_SELECTED_BG, TAG_SELECTED_HL},
    {TAG_KEY_FG, TAG_KEY_BG, TAG_KEY_HL},
    {TAG_KEY_SELECTED_FG, TAG_KEY_SELECTED_BG, TAG_KEY_SELECTED_HL},
    {CHECK_FG, CHECK_BG, CHECK_HL},
    {CHECK_SELECTED_FG, CHECK_SELECTED_BG, CHECK_SELECTED_HL},
    {UARROW_FG, UARROW_BG, UARROW_HL},
    {DARROW_FG, DARROW_BG, DARROW_HL},
};				/* color_table */

WINDOW *cWindow;
int cursesInited = 0;

void
attr_clear (WINDOW * win, int height, int width, chtype attr)
{
    int i, j;

    wattrset (win, attr);
    for (i = 0; i < height; i++) {
	wmove (win, i, 0);
	for (j = 0; j < width; j++)
	    waddch (win, ' ');
    }
    touchwin (win);
}

void dialog_clear (void)
{
  attr_clear (stdscr, LINES, COLS, screen_attr);
  wnoutrefresh (stdscr);
}

/*
 * Draw a rectangular box with line drawing characters
 */
void
draw_box (WINDOW * win, int y, int x, int height, int width,
	  chtype box, chtype border)
{
    int i, j;

    wattrset (win, 0);
    for (i = 0; i < height; i++) {
	wmove (win, y + i, x);
	for (j = 0; j < width; j++)
	    if (!i && !j)
		waddch (win, border | ACS_ULCORNER);
	    else if (i == height - 1 && !j)
		waddch (win, border | ACS_LLCORNER);
	    else if (!i && j == width - 1)
		waddch (win, box | ACS_URCORNER);
	    else if (i == height - 1 && j == width - 1)
		waddch (win, box | ACS_LRCORNER);
	    else if (!i)
		waddch (win, border | ACS_HLINE);
	    else if (i == height - 1)
		waddch (win, box | ACS_HLINE);
	    else if (!j)
		waddch (win, border | ACS_VLINE);
	    else if (j == width - 1)
		waddch (win, box | ACS_VLINE);
	    else
		waddch (win, box | ' ');
    }
}

void clrscr(void)
{
  if (!cursesInited)
    NcursesInit(1);
  werase(cWindow);
  wmove(cWindow, 0,0);
}

int wherex(void)
{
  int x,y;
  getyx(cWindow, y, x);
  return x;
}

int wherey(void)
{
  int x,y;
  getyx(cWindow, y, x);
  return y;
}

void gotoxy(int x, int y)
{
  wmove(cWindow, y, x);
}

int NcursesGetCh(void)
{
  return wgetch(cWindow);
}

#define getche() NcursesGetCh()

void draw_shadow (WINDOW * win, int y, int x, int height, int width)
{
  int i;
  
  if (has_colors ()) {	/* Whether terminal supports color? */
    wattrset (win, shadow_attr);
    wmove (win, y + height, x + 2);
    for (i = 0; i < width; i++)
      waddch (win, winch (win) & A_CHARTEXT);
    for (i = y + 1; i < y + height + 1; i++) {
      wmove (win, i, x + width);
      waddch (win, winch (win) & A_CHARTEXT);
      waddch (win, winch (win) & A_CHARTEXT);
    }
    wnoutrefresh (win);
  }
}

/*
 * Setup for color display
 */
void color_setup (void)
{
    int i;

    if (has_colors ()) {	/* Terminal supports color? */
	start_color ();

	/* Initialize color pairs */
	for (i = 0; i < ATTRIBUTE_COUNT; i++)
	    init_pair (i + 1, color_table[i][0], color_table[i][1]);

	/* Setup color attributes */
	for (i = 0; i < ATTRIBUTE_COUNT; i++)
	    attributes[i] = C_ATTR (color_table[i][2], i + 1);
    }
}

/*
 * End using dialog functions.
 */

void NcursesInit(int useClr)
{
  int useColors = useClr;

  cWindow = initscr ();
  keypad (stdscr, TRUE);
  cbreak ();
  noecho ();
  if (useColors)
    color_setup ();
  dialog_clear ();
  cursesInited = 1;
}

void NcursesExit(void)
{
  endwin ();
}

WINDOW *NcursesOpenWindow(int left, int top, int right, int bottom,
		       int fore_colour, int back_colour, int bordered)
{
  WINDOW *wind;
  int x = left;
  int y = top;
  int height = bottom - top + 1;
  int width = right - left + 1;
  wind = newwin (height, width, y, x);
  keypad (wind, TRUE);
  wmove(wind, 1, 1);
  return wind;
}

void NcursesReopenWindow(WINDOW *wind)
{
  wrefresh(wind);
}

void NcursesCloseWindow(WINDOW *wind)
{
  endwin();
}

void cprintf(char *text, ...)
{
  va_list ap;
  float ftmp;
  char *buf, *optbuf;
  int bufptr = 0;
  WINDOW *w = cWindow;
  int x, y, i, slen, optstart, optstop, optlen;
  getyx(w, y, x);
  slen = strlen(text);
  buf = (char*)alloca(4096);
  va_start(ap, text);
  for (i = 0; i < slen; i++)
    if (text[i] == '%') {
      optstart = i;
      optstop = 0;
      while (!optstop) {
	switch(text[++i]) {
	case 'c':
	  optbuf = (char*)alloca(i - optstart + 2);
	  strncpy(optbuf, &text[optstart], i - optstart + 1);
	  optbuf[i - optstart + 1] = '\0';
	  sprintf(&buf[bufptr], optbuf, va_arg(ap, char));
	  optstop = 1;
	  break;
	case 'u':
	  optbuf = (char*)alloca(i - optstart + 2);
	  strncpy(optbuf, &text[optstart], i - optstart + 1);
	  optbuf[i - optstart + 1] = '\0';
	  sprintf(&buf[bufptr], optbuf, va_arg(ap, unsigned int));
	  optstop = 1;
	  break;
	case 'd':
	  optlen = i - optstart + 1;
	  optbuf = (char*)alloca(optlen + 1);
	  strncpy(optbuf, &text[optstart], optlen);
	  optbuf[optlen] = '\0';
	  sprintf(&buf[bufptr], optbuf, va_arg(ap, int));
	  optstop = 1;
	  break;
	case 's':
	  optlen = i - optstart + 1;
	  optbuf = (char*)alloca(optlen + 1);
	  strncpy(optbuf, &text[optstart], optlen);
	  optbuf[optlen] = '\0';
	  sprintf(&buf[bufptr], optbuf, va_arg(ap, char*));
	  optstop = 1;
	  break;
	case '%':
	  sprintf(&buf[bufptr], "%%");
	  optstop = 1;
	  break;
	case 'f':
	  optlen = i - optstart + 1;
	  optbuf = (char*)alloca(optlen + 1);
	  strncpy(optbuf, &text[optstart], optlen);
	  optbuf[optlen] = '\0';
	  ftmp = va_arg(ap, double);
	  sprintf(&buf[bufptr], optbuf, ftmp);
	  optstop = 1;
	  break;
	}
      }
      bufptr += strlen(&buf[bufptr]);
    }
  else
    {
      buf[bufptr++] = text[i];
    };
  buf[bufptr] = '\0';
  slen = bufptr;
  va_end(ap);
  for (i = 0; i < slen; i++)
    switch(buf[i]) {
    case '\n':
      getyx(w, y, x);
      y += 1;
      wmove(w, y, x);
      break;
    case '\r':
      getyx(w, y, x);
      x = 0;
      wmove(w, y, x);
      break;
    default:
      waddch(w, buf[i]);
    }
  wrefresh(w);
}
