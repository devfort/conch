#ifndef _DUTCH_PATISSERIE_DELICACIES_H
#define _DUTCH_PATISSERIE_DELICACIES_H

char *strclone(char *c);
char *strcopycat(char *c, char *d);
int count_lines_and_find_length_of_longest(const char *string,
                                           int *out_longest_line);
char **conch_blast_lines(char *blast, int available_width);
void conch_blast_lines_free(char **blast_lines);

#endif /* _DUTCH_PATISSERIE_DELICACIES_H */
