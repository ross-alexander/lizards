typedef struct HashEntry {
  void *key;
  void *data;
  struct HashEntry *chain;
} HashEntry;

typedef struct HashTable {
  int size;
  HashEntry **table;
} HashTable;

unsigned int HashString(int size, int len, char *ptr);
void HashInsert(HashTable *ht, int len, void *key, void *ptr);
void* HashSearch(HashTable *ht, int len, void *key);
int HashUpdate(HashTable *ht, int len, void *key, void *ptr);
int HashDelete(HashTable *ht, int len, void *key);

HashTable *HashCreate(unsigned int size);
void HashDestroy(HashTable *ht);
void** HashLinear(HashTable *ht);
int HashCount(HashTable *ht);
void HashIterate(HashTable *ht, void (*)(void*,void*), void *data);
