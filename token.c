#include "scc.h"


Token *add_token(Vector *v, int type, char *input) {
  Token *t = malloc(sizeof(Token));
  t->type = type;
  t->input = input;
  vec_push(v, t);
  return t;
}


Vector *tokenize(char *p){
  Vector *v = new_vec();
  int i = 0;
  while(*p){
    if(isspace(*p)){
      p++;
      continue;
    }
    
    if(*p == '+' || *p == '-'){
      add_token(v, *p, p);
      i++;
      p++;
      continue;
    }
    
    if(isdigit(*p)){
      Token *t = add_token(v, TOKEN_NUM, p);
      t->val = strtol(p, &p, 10);
      i++;
      continue;
    }

    fprintf(stderr, "[*] unexpected token: %s", p);
    exit(1);
  }

  add_token(v, TOKEN_EOF, p);
  return v;
}