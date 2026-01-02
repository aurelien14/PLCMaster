/* Tag table interface for registering and retrieving tags. TODO: complete definitions. */

#ifndef TAG_TABLE_H
#define TAG_TABLE_H

int tag_table_init(void);
void *tag_table_find(const char *name);

#endif /* TAG_TABLE_H */
