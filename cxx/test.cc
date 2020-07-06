#include <cstdio>
#include <vector>

int main()
{
  std::vector<char*> fv;

  fv.push_back("ABC");

  std::vector<char*>::iterator i;

  for (i = fv.begin(); i != fv.end(); i++)
    {
      printf("%s\n", *i);
    }
  fv.erase(fv.begin());
 for (i = fv.begin(); i != fv.end(); i++)
    {
      printf("%s\n", *i);
    }
 }
