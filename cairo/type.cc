#include <typeinfo>
#include <stdio.h>

class foo {
  public:
};

int main()
{
  printf("%s\n", typeid(class foo).name());
}
