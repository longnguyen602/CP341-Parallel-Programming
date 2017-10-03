#include <stdio.h>
#include <stdlib.h>
#include <string.h>
struct _LinkedRecord{
    int stops;
    int year;
    struct _LinkedRecord *next;
};

typedef struct _LinkedRecord LinkedRecord_t;

void LinkedRecord_init(LinkedRecord_t* LR){
  LR = malloc(sizeof(LinkedRecord_t));
  LR->year = 1;
}

int parse_line(char* line){
  int year;
  //printf("%s", line);
  char str_year[5] = {line[6], line[7], line[8], line[9], '\0'};
  printf("%s\n", str_year);
  printf("TEST\n");
  year = atoi(str_year);
  printf("%d", year);
  return year;
}

LinkedRecord_t * find_record(LinkedRecord_t * head, int year) {
    LinkedRecord_t * current = head;
    LinkedRecord_t * result;
    while (current != NULL) {
        //printf("%d\n", current->year);
        if (current->year == 1){
          current->year = year;
        }
        if (current->year == year){
          result = current;
        }
        current = current->next;
    }
    return result;
}

void print_list(LinkedRecord_t * head) {
    LinkedRecord_t * current = head;

    while (current != NULL) {
        printf("%d had %d stops\n", (current->year ,current->stops));
        current = current->next;
    }
}

int main(int argc, char** argv) {
  if (argc < 2){
    printf("Usage: %s <traffic data>\nExiting...\n",argv[0]);
  }
  int year;
  LinkedRecord_t LR;
  LinkedRecord_init(&LR);
  FILE* f; // file handle to use for read and write
  int e;
  //The following 10 lines of code are reference from an online source
  // @ https://gsamaras.wordpress.com/code/read-file-line-by-line-in-c-and-c/
  char buf[1024];
  if ((f = fopen(argv[1], "r")) == NULL){ /* Open source file. */
    perror("fopen source-file");
    return 1;
  }

  while (fgets(buf, sizeof(buf), f) != NULL){
    LinkedRecord_t* record;
    buf[strlen(buf) - 1] = '\0'; // eat the newline fgets() stores
    year=parse_line(buf);
    printf("%d", year);
    record = find_record(&LR, year);
    record->stops+=1;
    //printf("%s\n", buf);
  }
  fclose(f);
  print_list(&LR);
  return 0;
  //b = fread(in_buf, sizeof(char), 100, f);
}
