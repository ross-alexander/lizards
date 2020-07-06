#include "compat/compat.h"

#define LIZ_TRUE 1
#define LIZ_FALSE 0

#define VER_STRING "Ver 1.1b"

#define COPYRIGHT_STRING "1991-96"

#define MAX_PLAYERS 16

#define MAX_RANDOM_MESSAGES 100

#define MAX_SPYS 1000

#define MAX_LIZARDS_IN_HEX 300

#define PERCENT_HOME_DENS_FOR_VICTORY 0.60

/* various text and binary orders files used by Lizards! */

#define TEXT_ORDS_F                   "LORDS"   /* text order file */
#define BBS_TEXT_ORDS_F               "LEXOR"   /* extra text order file */
#define TEXT_START_ORDS_F             "LSTAR"   /* startup text order file */
#define ORDS_PROC_F                   "LORDB"  /* binary orders */
#define ORDS_SHUF_F                   "LORDR"  /* shuffled binary orders */
#define POSTSCRIPT_F                  "LPOST"  /* postscript turn file */

/* various text output files for players. */

#define PLAYER_MESG_F                 "LP"   /* general player text */
#define PLAYER_LIST_F                 "LL"   /* modem user view list */
#define PLYMAP_MESG_F                 "LM"   /* text maps */
#define PLYPLY_MESG_F                 "LC"   /* text messages */
#define PLYERR_MESG_F		      "LE"   /* error messages */

/* Lizards! binary and text data files. */

#define WORLD_DATA_F                  "LWLDATA."   /* binary hex file */
#define WORLD_STAT_F                  "LWLSTAT."   /* text world statistics */
#define WORLD_TURN_F                  "LWLTURN."   /* lizards turn file */
#define WORLD_SWITCH_F                "LWLSWIT."   /* text game switches */
#define REAL_WORLD_SWITCH_F           "LWLSWITC."
#define HOME_STAT_F                   "LPLYHOM."  /* list of home dens */
#define PLAYER_F                      "LPLAYER."  /* text player list */
#define SPY_F                         "LSPYS."    /* binary spy file */
#define LIZ_MESG_F                    "LMESGS.TXT" /* text random mesg file */

#define LZ_GAMECODELEN 8
#define LZ_MAXPATH 256

#define LZ_FALSE 0
#define LZ_TRUE 1

/* ----------------------------------------------------------------------
--
-- Lots of global variables
--
---------------------------------------------------------------------- */

extern char player_code [MAX_PLAYERS + 1][6];
extern char player_name [MAX_PLAYERS + 1][37];
extern char player_home_den [MAX_PLAYERS + 1];
extern char player_clan_name [MAX_PLAYERS + 1][37];

extern char player_road [MAX_PLAYERS + 1][37];
extern char player_burb [MAX_PLAYERS + 1][37];
extern char player_city [MAX_PLAYERS + 1][37];
extern char player_country [MAX_PLAYERS + 1][37];
extern char player_cred [MAX_PLAYERS + 1][10];
extern char player_output_mode [MAX_PLAYERS + 1];
extern char game_due_date [20];

extern int players_in_game;

extern char dir[7][3];
extern char lizard_colours[6][12];

extern char *data_path;
extern char *game_code;
extern char *disk_data_path;
extern char *email_data_path;
extern int world_x, world_y, number_orders, turn_number;
extern int number_spys;
extern char *strWorldInit, *strWorldFfro, *strWorldFill, *strWorldPop;
extern char *strWorldFix, *strOrderEnt;
