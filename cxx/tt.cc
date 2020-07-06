#include <assert.h>
#include <stdio.h>
#include <map>
#include <typeinfo>

class super_t {
public:
  virtual ~super_t() {};
};

class sub_t : public super_t {
public:
  ~sub_t() {};
  int foo(const std::type_info& t)
  {
    printf("%s:%s\n", typeid(this).name(), t.name());
    return ((typeid(this) == t) || (typeid(*this) == t));
  }
};


int main()
{
  std::map<char*, char*> m;
  m["A"] = "b";
  if (m["B"])
    {
      printf("%s %s\n", m["A"], m["B"]);
    }
  super_t *s = 0;
  sub_t* sub = dynamic_cast<sub_t*>(s);
  sub_t* sub2 = new sub_t();
  printf("%d\n", sub2->foo(typeid(sub2)));
  printf("%d\n", sub2->foo(typeid(sub_t)));
}
