#ifndef _DUTCH_PATISSERIE_DELICACIES_H
#define _DUTCH_PATISSERIE_DELICACIES_H

char *strclone(char *c);
char *strcopycat(char *c, char *d);
char* stralleycat(int count, char **strs);

int count_lines_and_find_length_of_longest(const char *string,
                                           int *out_longest_line);
char **wrap_lines(char *text, int max_line_length);
void wrap_lines_free(char **wrapped_lines);

#endif /* _DUTCH_PATISSERIE_DELICACIES_H */
