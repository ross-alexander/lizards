#include <string>
#include <map>

#include <cairomm/context.h>

#include "lizards.h"

int main()
{
  serial_map_t s;
  s.add("abc", serial_t(10));
  serial_array_t a;
  s.add("x", a);
  a.add(serial_t(0.34));
  printf("%s\n", s.to_string());
}
