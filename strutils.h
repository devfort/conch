#ifndef _DUTCH_PATISSERIE_DELICACIES_H
#define _DUTCH_PATISSERIE_DELICACIES_H

char *strclone(const char *c);
char *strcopycat(const char *c, const char *d);
char *stralleycat(int count, char **strs);
char *strcopytrunc(char const *const src, unsigned int width);

char *expand_home(const char *path);

int count_lines_and_find_length_of_longest(const char *string,
                                           int *out_longest_line);
char **wrap_lines(char *text, int max_line_length, unsigned int *nout);
void wrap_lines_free(char **wrapped_lines, unsigned int nlines);

#endif /* _DUTCH_PATISSERIE_DELICACIES_H */
