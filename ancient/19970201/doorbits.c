/*
   DOORBITS.C

   Bits and pieces for doors games.
   By Stuart Nicholson, 1989.
   (c) Copyright Stuart Nicholson, 1989.
   Originally for The Basement BBS.

   RS232 Communication Routines (UTILS.LIB) by Richard Vowles.
*/

#include <dir.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <conio.h>

#include "io_lib.h"
#include "doorbits.h"

extern int ANSI;

/*
   YORN

   Prompts for Yes or No responses. Default is the default response
   ( 0 = No (FALSE), 1 = Yes (TRUE) ), returns 0 for No, 1 for Yes.
*/

int YorN (char *prompt, int default_resp)
{
  unsigned char response = '\0';

  while (response != 'Y' && response != 'N' && response != '\x0D')
  {
    writeln ("%s", prompt);

    response = toupper (NeedChar ());

    writeln ("%n");
  }

  switch (response)
  {
    case 'Y':
      return (1);
    case 'N':
      return (0);
    default:
      if (default_resp) return (1); else return (0);
  }
}

/*
   TIME_TOINT

   Returns present system time as seconds, minutes, hours.
*/

void time_toint (int *second, int *minute, int *hour)
{
  time_t tnow;
  struct tm *worktime;

  time (&tnow);

  worktime = localtime (&tnow);

  *second = worktime->tm_sec;
  *minute = worktime->tm_min;
  *hour = worktime->tm_hour;
}

/*
   DATE_TOINT

   Returns present date as month, year, weekday (0-6), yearday (0-364).
*/

void date_toint (int *month, int *year, int *weekday, int *yearday)
{
  time_t tnow;
  struct tm *worktime;

  time (&tnow);

  worktime = localtime (&tnow);

  *month = worktime->tm_mon;
  *year = worktime->tm_year;
  *weekday = worktime->tm_wday;
  *yearday = worktime->tm_yday;
}

/*
   TIMEDATE_STRING

   Returns the system time/date string.
*/

char *timedate_string (char *text)
{
  time_t tnow;

  time (&tnow);

  localtime (&tnow);

  strcpy (text, ctime(&tnow));

  return (text);
}

/*
   MINUTES_PASSED

   Returns the number of minutes passed since 'logon' (ie since the
   login_clock variable was set using clock () function).
*/

int minutes_passed (void)
{
  return ((((long) clock () / CLK_TCK) / 60));
}

/*
   TIME_GREET

   Returns 'greeting' depending on time.
*/

char *time_greet (char *greeting)
{
  int work1,
      work2,
      work3;

  time_toint (&work1, &work2, &work3);

  if (work3 < 12)
    strcpy (greeting, "morning");
  else
    if (work3 < 18)
      strcpy (greeting, "afternoon");
    else
      strcpy (greeting, "evening");

  return (greeting);
}

/*
   GET_USER_SFDOORS

   Loads SFDOORS.DAT and returns appropriate data, also initialises
   RS232IO stuff using info from file.
*/

int get_user_sfdoors (char *users_name, int *time_left, char *path_to_sfdoors)
{
  FILE *SFDOORS;
  char file_path[64], rubbish[100], COMPort[6];
  int monochrome = FALSE, COMBase, ANSI;

  strcpy (file_path, path_to_sfdoors);
  file_path[strlen(file_path)-1] = 0;

  if ((SFDOORS = fopen(file_path, "rt")) == NULL)
  {
    printf ("FATAL ERROR: Unable to open SFDOORS.DAT file!\n");
    exit (1);
  }

  fgets (rubbish, 100, SFDOORS); /* user number, unreliable */

  fgets (users_name, 30, SFDOORS);
  users_name[strlen(users_name)-1]=0;

  fgets (rubbish, 100, SFDOORS); /* password, not needed */
  fgets (rubbish, 100, SFDOORS); /* First name, not needed */
  fgets (rubbish, 100, SFDOORS); /* baud rate, presumed still set! */

  fgets (COMPort, 4, SFDOORS);
  if (strlen (COMPort) != 0) COMPort[strlen(COMPort)-1]=0;
  COMBase = atoi(COMPort);

  if (COMBase == 0) COMBase = 1;

  rubbish[strlen(rubbish)-1]=0;

  if ((strcmp("LOCAL", rubbish) != 0) && (strcmp("0", rubbish) != 0) )
  {
    InitializeRS232 (COMBase);
    COMBase = atoi (rubbish);
    set_baud_rate (COMBase);
  }

  fgets(rubbish, 10, SFDOORS);
  *time_left = atoi (rubbish);

  fgets (rubbish, 100, SFDOORS);
  fgets (rubbish, 100, SFDOORS); /* path to SPITFIRE.EXE file*/
  fgets (rubbish, 10, SFDOORS); /* ANSI ON/OFF */

  InitializeIO (FALSE, FALSE, FALSE);

  if (strcmp("TRUE\n", rubbish) == 0)
  {
    ANSI=TRUE;
    monochrome=FALSE;
  } else ANSI=FALSE;

  InitializeIO(ANSI, monochrome, FALSE);

  fgets (rubbish, 10, SFDOORS); /* security level */

  return (atoi (rubbish));
}

/*
   GET_DOOR_DEF

   Loads the Doors definition file, which contains BBS Name etc.
*/

void get_door_def (char *bbs_name, char *path_to_sfdoor, char *name_of_sysop,
		   char *path_to_bulletin, char *def_name)
{
  FILE *doordef;

  if ((doordef = fopen(def_name, "r")) == NULL)
  {
    printf ("FATAL ERROR: Unable to open door definition file!\n");
    printf ("Please refer to doors documentation.\n");
    exit(1);
  }else{
    fgets(bbs_name, 100, doordef);
    fgets(name_of_sysop, 40, doordef);
    fgets(path_to_sfdoor, 64, doordef);
    fgets(path_to_bulletin, 64, doordef);

    if (strlen(path_to_bulletin)>0) path_to_bulletin[strlen(path_to_bulletin)-1] = 0;

    fclose(doordef);
  }
}

/*
   OPEN_DOOR

   Calls other modules in DOOROPEN.C to open the door game,
   displays 'welcome' header, also displays bulletin file if
   one exists.
*/

void open_door(char *users_name, char *bbs_name, char *sysops_name,
	int *time_left, char *intro_name, char *def_name)
{
  char path_to_sfdoor[65], path_to_bulletin[65], work_string [13];

  get_door_def (bbs_name, path_to_sfdoor, sysops_name, path_to_bulletin,
		def_name);

  get_user_sfdoors (users_name, time_left, path_to_sfdoor);

  if (*bbs_name != NULL) bbs_name[strlen(bbs_name)-1] = '\0';
  if (*sysops_name != NULL) sysops_name[strlen(sysops_name)-1] = '\0';

  writeln ("%b");

  window (1, 1, 80, 15);

  writeln ("%r%s and %s present,%n%n", BLACKF+WHITEB,
	    bbs_name, sysops_name);

  if (ANSI)
  {
    fnmerge (work_string,"","",intro_name, ".CLR");
    show_file (work_string);
  }else{
    fnmerge (work_string, "", "", intro_name, ".BBS");
    show_file (work_string);
  }

  if (*path_to_bulletin != NULL)
  {
    show_file (path_to_bulletin);
    tap_key ("Press any key to continue.");
    writeln ("%n");
  }
}
