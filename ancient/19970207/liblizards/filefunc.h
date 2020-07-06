unsigned int num_records ( FILE *fptr, unsigned int size_rec );
unsigned int fname_num_records (char *fname, unsigned int size_rec );
void set_rec ( FILE *fptr, unsigned int size_rec, unsigned int rec_num );
void get_rec ( FILE *fptr, unsigned int size_rec, unsigned int rec_num,
	       void *rec );
void put_rec ( FILE *fptr, unsigned int size_rec, unsigned int rec_num,
	       void *rec );
void fname_get_rec ( char *fname, unsigned int size_rec, unsigned int rec_num,
		    void *rec );
void fname_put_rec ( char *fname, unsigned int size_rec, unsigned int rec_num,
		    void *rec );
void zero_file ( char *filename );
void wipe_file ( char *fname, size_t size, unsigned int recs, void *record );
