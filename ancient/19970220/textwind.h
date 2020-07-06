/* Contains data structure for 'dumb' text windows (they're not true
   windows). */

#define NO_BORDER FALSE, 0, 0, 0, 0, 0, 0
#define SINGLE_BORDER TRUE, 218, 191, 196, 179, 192, 217
#define DOUBLE_BORDER TRUE, 201, 187, 205, 186, 200, 188

typedef struct
{
  int left,
      top,
      right,
      bottom,
      bordered,
      fore_colour,
      back_colour;
} win_t;
