/* Status tags definitions and lookup declarations. TODO: define status catalog. */

#ifndef STATUS_TAGS_H
#define STATUS_TAGS_H

int status_tags_init(void);
const char *status_tags_lookup(int code);

#endif /* STATUS_TAGS_H */
