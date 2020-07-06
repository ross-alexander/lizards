#include "server.h"

int sockfd;

typedef struct TextBuffer {
  struct TextBuffer *next;
  char *text;
  size_t size;
} TextBuffer;

char *Read(int fd)
{
  struct timeval tv;
  fd_set set;
  TextBuffer *last;
  size_t size = 4096;

  last = NULL;

  FD_ZERO(&set);
  FD_SET(fd, &set);

  tv.tv_sec = 10;
  tv.tv_usec = 0;

  FD_SET(fd, &set);
  while(!select(fd + 1, &set, NULL, NULL, &tv))
    FD_SET(fd, &set);

  do
    {
      TextBuffer *tb = (TextBuffer*)alloca(sizeof(TextBuffer));
      tb->size = size;
      tb->text = (char*)alloca(size);
      tb->size = read(fd, tb->text, size - 1);
      tb->text[tb->size] = '\0';
      printf("%d\n", tb->size);
      tb->next = last;
      last = tb;
      FD_SET(fd, &set);
      tv.tv_sec = 0;
      tv.tv_usec = 0;
    }
  while(select(fd + 1, &set, NULL, NULL, &tv) && last->size);

  return last->size ? last->text : NULL;
}

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
/* ----------------------------------------------------------------------
--
-- Serve
--
---------------------------------------------------------------------- */
void Serve(int fd)
{
  char *ptr, **strings;
  int i, params;
  while ((ptr = Read(fd)))
    {
      strings = Split(ptr);
      for (i = 0; strings[i]; i++);
      params = i;
      
      for (i = 0; strings[i]; i++)
	fprintf(stdout, "%s\n", strings[i]);
      fprintf(stdout, "++ %d ++\n", params);

      if (!params)
	write(fd, "Error\r\n", 8);
      else if(!strcmp("quit", strings[0]))
	break;
    }
  close(fd);
}

/* ----------------------------------------------------------------------
--
-- die
--
---------------------------------------------------------------------- */

static void die(int sig)
{
  fprintf(stderr, "Server closed connection\n");
  close(sockfd);
  exit(1);
}

/* ----------------------------------------------------------------------
--
-- main
--
---------------------------------------------------------------------- */
int main(int argc, char *argv[])
{
  int msgsock, length;
  struct hostent *hp;
  struct sockaddr_in sa;
  char *host = "localhost";
  int port = 5000;

#ifdef HAVE_HEADER_STDC
  memset(&sa, 0, sizeof(sa));
#else
  bzero(&sa, sizeof(sa));
#endif

  if ((hp = gethostbyname(host)) == NULL)
    {
      fprintf(stderr, "Cannot get host entry for %s.\n", host);
      exit(1);
    }

  if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
      fprintf(stderr, "Error in socket system call.\n");
      exit(1);
    }

#ifdef HAVE_HEADER_STDC
  memcpy(&(sa.sin_addr), hp->h_addr, hp->h_length);
#else
  bcopy(hp->h_addr, &(sa.sin_addr), hp->h_length);
#endif

#ifdef SIN_LENGTH
  sa.sin_length = sizeof(sa);
#endif

  sa.sin_family = AF_INET;
  sa.sin_port = htons(port);
  sa.sin_addr.s_addr = INADDR_ANY;

  if(bind(sockfd, (struct sockaddr*)&sa, sizeof(sa)) < 0)
    {
      fprintf(stderr, "Could not bind address.\n");
      exit(1);
    }
  (void) signal(SIGTERM, die);
  (void) signal(SIGQUIT, die);
  (void) signal(SIGINT, die);
  (void) signal(SIGHUP, die);

  listen(sockfd, 5);
      
  length = sizeof(sa);
  msgsock = accept(sockfd, &sa, &length);
  if (msgsock < 0)
    {
      perror("accept");
      exit(1);
    }
  else
    Serve(msgsock);
  close(sockfd);
  return 0;
}
