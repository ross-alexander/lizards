char **open_help_file (FILE **help_fptr, char *help_file_name,
		       char *index_file_name);
char **load_index_file (char **index, FILE *index_fptr);
long find_index (char **index, char *entry);
void enter_help (FILE *help_fptr, char **index);
void enter_help_page (FILE *help_fptr, char **index, char *page_index);
void display_help_page (FILE *help_fptr, char **index, long offset,
			unsigned int page);
int display_and_link_help_page (FILE *help_fptr, char **index, long offset,
				unsigned int page, hyper_link_t **links,
				unsigned int *pages);
int select_hyper_link (int number_links, hyper_link_t *links,
		       int *present_link, unsigned int present_page,
		       unsigned int pages);
void show_hyper_link (hyper_link_t *links, unsigned int present_link,
		      int hyper_attr, int normal_attr);
