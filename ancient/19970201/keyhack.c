#include <conio.h>
#include <stdio.h>

main ()
{
  int option;

  do
  {
    option = getch ();

    printf ("%d\n", option);
  } while (option != 'Q');
}