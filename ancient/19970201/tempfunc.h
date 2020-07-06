int enter_template (field_t **template, char **data, unsigned int start_field,
                    unsigned int *field_in, int (*end_key_func) (unsigned int *field,
                    field_t **template, char **data), int (*special_key_func)
                    (unsigned int *field, field_t **template, char **data),
                    int (*field_exit_func) (unsigned int *field,
                    field_t **template, char **data));
void show_template (field_t **template, char **data);
void show_field_range (unsigned int field_from, unsigned int field_to,
                       field_t **template, char **data);
void show_field (field_t *field, char *data);
field_t **load_template (char *template_name, unsigned int *num_fields);
void free_data (field_t **template, char **data);
void free_template (field_t **template);
void set_field (field_t **template, char **data, unsigned int field_num,
  char *string);
int get_string (char *string, unsigned int maxlen, unsigned int allowed,
                unsigned int space_f, unsigned int function_key_f,
                unsigned int overflow_f, char *overflow_char);
void show_error (char *error);
void clear_error (void);
