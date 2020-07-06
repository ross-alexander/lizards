void move_lizards (int old_x, int old_y, int new_x, int new_y, int red,
		   int green, int grey, int black, int yellow);
int combat (int red, int green, int grey, int black, int yellow,
	    int *new_red, int *new_green, int *new_grey, int *new_black,
	    int *new_yellow, int attack_x, int attack_y, int defend_x,
	    int defend_y, int flee_flag);
void capture_lizards (hex_t *hex, int no_captured, int *red, int *green, int *grey,
		      int *black, int *yellow, int *red_captured, int *green_captured,
		      int *grey_captured, int *black_captured, int *yellow_captured);
void combat_sea_monster (int red, int green, int grey, int black, int yellow,
			int *new_red, int *new_green, int *new_grey,
			int *new_black, int *new_yellow, int attack_x,
			int attack_y, int *sea_monster);
int terrain_adjacent (int x, int y, int terrain);
void move_coords (int direction, int x, int y, int *new_x, int *new_y);
void in_sight (int x, int y, int *players);
void players_here (int x, int y, int *players);
void get_hex (int x, int y, hex_t *hex);
void put_hex (int x, int y, hex_t *hex);
void whirlpool_kill (int x, int y);
void bridge_collapse_kill (int x, int y);
int player_spy_here (int x, int y, int player);
int moveable_player_spy_here (int x, int y, int player);
void inc_combat_points (hex_t *hex);
int spy_lizards_in_hex (int x, int y, int player);
