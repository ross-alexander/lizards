#define BLACKB 0
#define REDB 1
#define GREENB 2
#define YELLOWB 3
#define BLUEB 4
#define MAGENTAB 5
#define CYANB 6
#define WHITEB 7

#define BLACKF 0
#define REDF (1<<4)
#define GREENF (2<<4)
#define YELLOWF (3<<4)
#define BLUEF (4<<4)
#define MAGENTAF (5<<4)
#define CYANF (6<<4)
#define WHITEF (7<<4)
#define FLASH 128
#define BOLD 128

#define BBLACK "40"
#define BRED "41"
#define BGREEN "42"
#define BYELLOW "43"
#define BBLUE "44"
#define BMAGENTA "45"
#define BCYAN "46"
#define BWHITE "47"

#define FBLACK "30"
#define FRED "31"
#define FGREEN "32"
#define FYELLOW "33"
#define FBLUE "34"
#define FMAGENTA "35"
#define FCYAN "36"
#define FWHITE "37"

#define FBOLD "1"
#define FREVERSE "7"
#define FUNDERLINE "4"

#define FALSE 0
#define TRUE 1
#define CRLF "\x0D\x0A"
#define NORMAL (7<<4)

void writeln (char *format, ...);
int NeedChar (void);
int YorN (char *prompt, int default_yes);
void show_file (char *file);
void input_line (char *input, int length);
void tap_key (char *prompt);
int input_value (char *prompt, int low, int high, int *return_pressed_f);