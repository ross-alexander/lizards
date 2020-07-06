#include <typeinfo>
#include <stdio.h>

class test_t { public: virtual ~test_t() {}; };
class test2_t : public test_t {};

int main()
{
  test_t *t1 = new test_t;
  test_t *t2 = new test2_t;

  printf("%s %s\n", typeid(*t1).name(), typeid(*t2).name());
}
