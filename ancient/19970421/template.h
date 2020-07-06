#define NUMERIC 0
#define ALPHA 1
#define ALPHANUMERIC 2

#define SPECIAL_KEY 1
#define END_KEY 2
#define ERASE_KEY 3
#define FINISH_KEY 4
#define ESCAPE_KEY 5
#define UP_KEY 6
#define DOWN_KEY 7
#define LEFT_KEY 8
#define RIGHT_KEY 9
#define UP_FORM_KEY 10
#define DOWN_FORM_KEY 11
#define CHECK_KEY 12

typedef struct
{
  unsigned int scr_x : 7,
               scr_y : 7,
               length : 7,
               type : 4,
               spaced : 1,      /* allowed whitespace */
               overflow_f : 1;  /* allowed to flow into next field */

  char jump_down,
       jump_up,
       start_of_line_f;
} field_t;
