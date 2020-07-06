/*
   DOORBITS.H

   Bits and pieces for doors games.
   By Stuart Nicholson, 1989.
   (c) Copyright Stuart Nicholson, 1989.
   Originally for The Basement BBS.

   RS232 Communication Routines (UTILS.LIB) by Richard Vowles.
*/

int YorN (char *prompt, int default_resp);
int get_user_sfdoors (char *users_name, int *time_left, char *path_to_sfdoors);
void get_door_def (char *bbs_name, char *path_to_sfdoor, char *name_of_sysop,
		   char *path_to_bulletin, char *def_name);
void open_door(char *users_name, char *bbs_name, char *sysops_name,
	int *time_left, char *intro_name, char *def_name);
void time_toint (int *second, int *minute, int *hour);
void date_toint (int *month, int *year, int *weekday, int *yearday);
char *timedate_string (char *string);
char *time_greet (char *greeting);
int minutes_on (int on_min, int on_hour);