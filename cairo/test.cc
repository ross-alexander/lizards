#include <cstdlib>

#include <stdio.h>
#include <typeinfo>

class A {
public:
  int v;
  A() { v = 1; };
  virtual int isa_ti(const std::type_info &ti)
  {
    printf("%s %s %s\n", ti.name(), typeid(this).name(), typeid(*this).name());
    return((typeid(this) == ti) || (typeid(*this) == ti));
  }
  static int isa(A* a)
  {
    return(dynamic_cast<A*>(a) != nullptr);
  }
};

class B : public A {
public:
  B() { v = 2; };
  static int isa(A *a)
  {
     return(dynamic_cast<B*>(a) != nullptr);
  }
};

class C : public B {
public:
  C() { v = 3; };
  static int isa(A *a)
  {
     return(dynamic_cast<C*>(a) != nullptr);
  }
};

int main()
{
  A *a = new(A);
  B *b = new(B);
  C *c = new(C);
 
  //  printf("%d\n", a->isa(typeid(B)));
  //  printf("%d\n", b->isa(typeid(B)));
  //  printf("%d\n", c->isa(typeid(B)));

  printf("%d\n", A::isa(a));
  printf("%d\n", B::isa(a));
  printf("%d\n", C::isa(a));

  int x = -4;
  std::div_t d = div(-4, 20);
  printf("%d %d\n", d.quot, d.rem);
}
