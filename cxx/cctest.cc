#include <stdio.h>
#include <stdlib.h>

class base_t {
public:
  base_t() { printf("new base_t\n"); };
  virtual void name() { printf("base_t\n"); };
};

class super_t : public base_t {
public:
  super_t() { printf("new super_t\n"); };
  void name();
};

void super_t::name() { printf("super_t\n"); }

class zuper_t : public base_t {
public:
  zuper_t() {};
};

class dir_t {
private:
  int d;
public:
  dir_t(int x) { d = x; };
  operator char *() { char *s = (char*)calloc(128, 1); sprintf(s, "%d", d); return s;};
};

int main()
{
  printf("Here\n");
  super_t *t = new super_t;
  zuper_t *z = new zuper_t;
  base_t *b = t;
  t->name();
  b->name();
  z->name();
  dir_t d(333);
  printf("%s\n", (char*)d);
}
