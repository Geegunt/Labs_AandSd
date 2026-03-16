#ifndef SORT_H
#define SORT_H

int read_table_from_file(const char *filename, int **keys, char ***values, int *n);
void print_table(const int *keys, char *const *values, int n, const char *title);
void free_table(int *keys, char **values, int n);

void copy_table(const int *src_keys, char *const *src_values,
                int n, int *dst_keys, char **dst_values);

void shaker_sort(int *keys, char **values, int n);
int binary_search(const int *keys, int n, int target);

void make_sorted_case(const int *src_keys, char *const *src_values,
                      int n, int *dst_keys, char **dst_values);

void make_reverse_case(const int *src_keys, char *const *src_values,
                       int n, int *dst_keys, char **dst_values);

void make_random_case(const int *src_keys, char *const *src_values,
                      int n, int *dst_keys, char **dst_values);

#endif
