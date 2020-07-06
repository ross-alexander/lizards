#include <stdarg.h>
#include <curses.h>

#define screen_attr                   attributes[0]
#define shadow_attr                   attributes[1]
#define dialog_attr                   attributes[2]
#define title_attr                    attributes[3]
#define border_attr                   attributes[4]
#define button_active_attr            attributes[5]
#define button_inactive_attr          attributes[6]
#define button_key_active_attr        attributes[7]
#define button_key_inactive_attr      attributes[8]
#define button_label_active_attr      attributes[9]
#define button_label_inactive_attr    attributes[10]
#define inputbox_attr                 attributes[11]
#define inputbox_border_attr          attributes[12]
#define searchbox_attr                attributes[13]
#define searchbox_title_attr          attributes[14]
#define searchbox_border_attr         attributes[15]
#define position_indicator_attr       attributes[16]
#define menubox_attr                  attributes[17]
#define menubox_border_attr           attributes[18]
#define item_attr                     attributes[19]
#define item_selected_attr            attributes[20]
#define tag_attr                      attributes[21]
#define tag_selected_attr             attributes[22]
#define tag_key_attr                  attributes[23]
#define tag_key_selected_attr         attributes[24]
#define check_attr                    attributes[25]
#define check_selected_attr           attributes[26]
#define uarrow_attr                   attributes[27]
#define darrow_attr                   attributes[28]

/* number of attributes */
#define ATTRIBUTE_COUNT               29

extern chtype attributes[];

#define SCREEN_FG                    COLOR_CYAN
#define SCREEN_BG                    COLOR_BLUE
#define SCREEN_HL                    TRUE

#define SHADOW_FG                    COLOR_BLACK
#define SHADOW_BG                    COLOR_BLACK
#define SHADOW_HL                    TRUE

#define DIALOG_FG                    COLOR_BLACK
#define DIALOG_BG                    COLOR_WHITE
#define DIALOG_HL                    FALSE

#define TITLE_FG                     COLOR_YELLOW
#define TITLE_BG                     COLOR_WHITE
#define TITLE_HL                     TRUE

#define BORDER_FG                    COLOR_WHITE
#define BORDER_BG                    COLOR_WHITE
#define BORDER_HL                    TRUE

#define BUTTON_ACTIVE_FG             COLOR_WHITE
#define BUTTON_ACTIVE_BG             COLOR_BLUE
#define BUTTON_ACTIVE_HL             TRUE

#define BUTTON_INACTIVE_FG           COLOR_BLACK
#define BUTTON_INACTIVE_BG           COLOR_WHITE
#define BUTTON_INACTIVE_HL           FALSE

#define BUTTON_KEY_ACTIVE_FG         COLOR_WHITE
#define BUTTON_KEY_ACTIVE_BG         COLOR_BLUE
#define BUTTON_KEY_ACTIVE_HL         TRUE

#define BUTTON_KEY_INACTIVE_FG       COLOR_RED
#define BUTTON_KEY_INACTIVE_BG       COLOR_WHITE
#define BUTTON_KEY_INACTIVE_HL       FALSE

#define BUTTON_LABEL_ACTIVE_FG       COLOR_YELLOW
#define BUTTON_LABEL_ACTIVE_BG       COLOR_BLUE
#define BUTTON_LABEL_ACTIVE_HL       TRUE

#define BUTTON_LABEL_INACTIVE_FG     COLOR_BLACK
#define BUTTON_LABEL_INACTIVE_BG     COLOR_WHITE
#define BUTTON_LABEL_INACTIVE_HL     TRUE

#define INPUTBOX_FG                  COLOR_BLACK
#define INPUTBOX_BG                  COLOR_WHITE
#define INPUTBOX_HL                  FALSE

#define INPUTBOX_BORDER_FG           COLOR_BLACK
#define INPUTBOX_BORDER_BG           COLOR_WHITE
#define INPUTBOX_BORDER_HL           FALSE

#define SEARCHBOX_FG                 COLOR_BLACK
#define SEARCHBOX_BG                 COLOR_WHITE
#define SEARCHBOX_HL                 FALSE

#define SEARCHBOX_TITLE_FG           COLOR_YELLOW
#define SEARCHBOX_TITLE_BG           COLOR_WHITE
#define SEARCHBOX_TITLE_HL           TRUE

#define SEARCHBOX_BORDER_FG          COLOR_WHITE
#define SEARCHBOX_BORDER_BG          COLOR_WHITE
#define SEARCHBOX_BORDER_HL          TRUE

#define POSITION_INDICATOR_FG        COLOR_YELLOW
#define POSITION_INDICATOR_BG        COLOR_WHITE
#define POSITION_INDICATOR_HL        TRUE

#define MENUBOX_FG                   COLOR_BLACK
#define MENUBOX_BG                   COLOR_WHITE
#define MENUBOX_HL                   FALSE

#define MENUBOX_BORDER_FG            COLOR_WHITE
#define MENUBOX_BORDER_BG            COLOR_WHITE
#define MENUBOX_BORDER_HL            TRUE

#define ITEM_FG                      COLOR_BLACK
#define ITEM_BG                      COLOR_WHITE
#define ITEM_HL                      FALSE

#define ITEM_SELECTED_FG             COLOR_WHITE
#define ITEM_SELECTED_BG             COLOR_BLUE
#define ITEM_SELECTED_HL             TRUE

#define TAG_FG                       COLOR_YELLOW
#define TAG_BG                       COLOR_WHITE
#define TAG_HL                       TRUE

#define TAG_SELECTED_FG              COLOR_YELLOW
#define TAG_SELECTED_BG              COLOR_BLUE
#define TAG_SELECTED_HL              TRUE

#define TAG_KEY_FG                   COLOR_YELLOW
#define TAG_KEY_BG                   COLOR_WHITE
#define TAG_KEY_HL                   TRUE

#define TAG_KEY_SELECTED_FG          COLOR_YELLOW
#define TAG_KEY_SELECTED_BG          COLOR_BLUE
#define TAG_KEY_SELECTED_HL          TRUE

#define CHECK_FG                     COLOR_BLACK
#define CHECK_BG                     COLOR_WHITE
#define CHECK_HL                     FALSE

#define CHECK_SELECTED_FG            COLOR_WHITE
#define CHECK_SELECTED_BG            COLOR_BLUE
#define CHECK_SELECTED_HL            TRUE

#define UARROW_FG                    COLOR_GREEN
#define UARROW_BG                    COLOR_WHITE
#define UARROW_HL                    TRUE

#define DARROW_FG                    COLOR_GREEN
#define DARROW_BG                    COLOR_WHITE
#define DARROW_HL                    TRUE

/* End of default color definitions */

#define C_ATTR(x,y)                  ((x ? A_BOLD : 0) | COLOR_PAIR((y)))
#define COLOR_NAME_LEN               10
#define COLOR_COUNT                  8

extern int color_table[][3];

#define ESC 27
#define TAB 9

#define NO_BORDER 0
#define SINGLE_BORDER 1

#define getche() NcursesGetCh()

typedef struct {
    char name[COLOR_NAME_LEN];
    int value;
} color_names_st;

extern color_names_st color_names[];
extern int color_table[][3];
extern int cursesInited;

extern WINDOW *cWindow;

void attr_clear (WINDOW * win, int height, int width, chtype attr);

void dialog_clear (void);
void draw_box (WINDOW *, int, int, int, int, chtype, chtype);
void clrscr(void);
void gotoxy(int x, int y);
int NcursesGetCh(void);
void draw_shadow (WINDOW * win, int y, int x, int height, int width);
void color_setup (void);
void NcursesInit(int useClr);
void NcursesExit(void);
WINDOW* NcursesOpenWindow(int, int, int, int, int, int, int);
void NcursesCloseWindow(WINDOW *);
void NcursesReopenWindow(WINDOW *);
void cprintf(char *text, ...);
int wherex(void);
int wherey(void);
