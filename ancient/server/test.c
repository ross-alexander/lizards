/* ----------------------------------------------------------------------
--
-- Split
--
---------------------------------------------------------------------- */
char **Split(char *ptr)
{
  int i, count;
  const char *special = "(,)";
  char *copy, *copyref, *tmp, **strings;

  copy = alloca(strlen(ptr) + 1);
  strcpy(copy, ptr);
  copyref = copy;

  count = 0;
  do {
    tmp = strsep(&copyref, special);
    count++;
  } while (tmp);

  strings = (char**)malloc(sizeof(char*) * count);
  strings[--count] = NULL;

  copyref = copy;
  for (i = 0; i < count; i++)
    {
      int length = strlen(copyref) + 1;
      strings[i] = (char*)malloc(length);
      strcpy(strings[i], copyref);
      copyref += length;
    };
  return strings;
}

int main()
{
  char **
