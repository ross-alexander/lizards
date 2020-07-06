#include <stdio.h>

class fish {
public:
  void virtual show();
  fish& operator=(const fish&);
};

void fish::show()
{
  printf("Fish\n");
}

fish& fish::operator=(const fish& f)
{
  printf("= operator for fish.\n");
  return *this;
}

class shark : public fish {
public:
  void show();
};

void shark::show() {
  printf("Shark!\n");
};

int main()
{
  fish f1;
  fish f2;
  f1 = f2;
  fish *f = new fish;
  shark *s = new shark;
  f->show();
  s->show();
  f = s;
  *f = f2;
  f->show();
}
