#include <libguile.h>
#include "lizards.h"

static void inner_main (void *closure, int argc, char **argv)
{
  char line[256];
  /* module initializations would go here */
  world_init();
  spy_init();
  hex_init();
  player_init();
  feature_init();

  /*
  scm_shell (argc, argv);
  */
  gh_eval_str("(load \"aa.scm\")");
  while (fgets(line, 256, stdin))
    {
      gh_eval_str(line);
    }
}

int main (int argc, char **argv)
{
  scm_boot_guile (argc, argv, inner_main, 0);
  return 0; /* never reached */
}
