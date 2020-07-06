char *show_lizards (int red, int green, int grey, int black, int yellow,
		    char *string);
void kill_lizards (int red, int green, int grey, int black, int yellow,
		   int *new_red, int *new_green, int *new_grey,
		   int *new_black, int *new_yellow, int deaths);
void kill_lizards_xy (hex_t *hex, int deaths);
int lizards_in_hex (hex_t *hex);
void open_player_mesg (int player, FILE **fptr);
void send_player_mesg (int player, char *format, ...);
char *xy_to_string (int x, int y, char *string);
char *loc_to_string (int x, char *string);
int average_combat_points (int lizards1, int cp1, int lizards2, int cp2);
int average_hunger_points (int lizards1, int hp1, int lizards2, int hp2);
int string_to_direction (char *dir_string);
void string_to_loc (char *string, int *x, int *y);
