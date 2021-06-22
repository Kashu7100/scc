#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>


enum{
  TOKEN_NUM = 256,
  TOKEN_EOF,
};


typedef struct {
  int key;
  int val;
  char *input; // token string
} Token;


// store tokenized input to this array
Token tokens[100];


void tokenize(char *p){
  int i = 0;
  while(*p){
    if(isspace(*p)){
      p++;
      continue;
    }
    
    if(*p == '+' || *p == '-'){
      tokens[i].key = *p;
      tokens[i].input = p;
      i++;
      p++;
      continue;
    }
    
    if(isdigit(*p)){
      tokens[i].key = TOKEN_NUM;
      tokens[i].input = p;
      tokens[i].val = strtol(p, &p, 10);
      i++;
      continue;
    }

    fprintf(stderr, "[*] unexpected token: %s", p);
    exit(1);
  }

  tokens[i].key = TOKEN_EOF;
}


void error(int i){
  fprintf(stderr, "[*] unexpected token: %s\n", tokens[i].input);
  exit(1);
}


int main(int argc, char **argv) {
  if (argc != 2) {
    fprintf(stderr, "Usage: 9cc <code>\n");
    return 1;
  }
  
  tokenize(argv[1]);

  printf(".intel_syntax noprefix\n");
  printf(".global main\n");
  printf("main:\n");
  
  if(tokens[0].key != TOKEN_NUM)
    error(0);
  printf("  mov rax, %d\n", tokens[0].val);
 
  int i = 1;
  while(tokens[i].key != TOKEN_EOF){
    if(tokens[i].key == '+'){
      i++;
      if(tokens[i].key != TOKEN_NUM)
        error(i);
      printf("  add rax, %d\n", tokens[i].val);
      i++;
      continue;
    }
    if(tokens[i].key == '-'){
      i++;
      if(tokens[i].key != TOKEN_NUM)
        error(i);
      printf("  sub rax, %d\n", tokens[i].val);
      i++;
      continue;
    }
    error(i);
  }
  printf("  ret\n");
  return 0;
}
