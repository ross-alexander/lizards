#include <stdio.h>

class a {
public:
  int v;
  a() { v = 10; }
  virtual a* clone() { return new a(*this); };
  virtual void out() { printf("v = %d\n", v); };
};

class b : public a {
public:
  b() { v = 20; }
  void out() { printf("**v = %d\n", v); }
  b* clone() { return new b(*this); }
};

class c {
public:
  c() { printf("Constuct c\n"); };
};
  

int main()
{
  b x;
  a* y = x.clone();
  x.out();
  y->out();
  c *z = new c[10];
}
