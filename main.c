#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "btree.h"

int compareFloat(const void *a, const void *b, void *udata) {
  float fa = *(float *)a;
  float fb = *(float *)b;
  if (fa < fb) return -1;
  if (fa > fb) return 1;
  return 0;
}

int main() {
  FILE *fp = fopen("forest.txt", "r");
  if (!fp) {
    perror("forest.txt");
    return 1;
  }

  struct btree *tr = btree_new(sizeof(float), 0, compareFloat, NULL);

  float value;
  char line[256];
  size_t count = 0;

  clock_t start = clock();
  while (fgets(line, sizeof(line), fp)) {
    char *token = strtok(line, ",");
    if (!token) continue;
    value = atof(token);
    // printf("%.6f\n", value);
    btree_set(tr, &value);
    count++;
  }
  clock_t end = clock();
  fclose(fp);

  double insertTime = (double)(end - start) / CLOCKS_PER_SEC;
  printf("Inserted %zu items in %.4f seconds\n", count, insertTime);

  float range[2] = {0.48f, 0.5f};
  size_t found = 0;
  clock_t qStart = clock();

  struct btree_iter *iter = btree_iter_new(tr);
  if (btree_iter_seek(iter, &range[0])) {
    do {
      float *v = btree_iter_item(iter);
      if (*v > range[1]) break;
      found++;
    } while (btree_iter_next(iter));
  }
  clock_t qEnd = clock();
  double queryTime = (double)(qEnd - qStart) / CLOCKS_PER_SEC;

  printf("Range query [%.2f, %.2f] -> %zu results in %.6f seconds\n",
         range[0], range[1], found, queryTime);

  btree_iter_free(iter);
  btree_free(tr);
  return 0;
}
