  int loop,
      loop_two;

  for (loop = 0; loop < strlen (source); loop ++)
    if (strchr (delim, (int) source [loop]) != NULL)
      break;
    else
      dest [loop] = source [loop];

  dest [loop] = NULL;

  for (loop ++, loop_two = 0; source [loop] != NULL; loop ++, loop_two ++)
    source [loop_two] = source [loop];

  source [loop_two] = NULL;

  return (dest);
