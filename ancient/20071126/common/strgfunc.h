char **make_token_table ( char **token_table, char *string, char *delims );
void free_malloced_table ( char **malloced_table );
char *flay_spaces (char *string);
char *strsplt (char *dest, char *source, int start);
char *strtoken (char *dest, char *source, char *delim);
char *chop_space (char *string);
char **add_to_strlist ( char **strlist, char *entry );
char **free_strlist ( char **strlist );
int strlist_len ( char **strlist );
