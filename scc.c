#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdarg.h>
#include <assert.h>


enum{
  TOKEN_NUM = 256,
  TOKEN_EOF,
};


typedef struct {
  int type;
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
      tokens[i].type = *p;
      tokens[i].input = p;
      i++;
      p++;
      continue;
    }
    
    if(isdigit(*p)){
      tokens[i].type = TOKEN_NUM;
      tokens[i].input = p;
      tokens[i].val = strtol(p, &p, 10);
      i++;
      continue;
    }

    fprintf(stderr, "[*] unexpected token: %s", p);
    exit(1);
  }

  tokens[i].type = TOKEN_EOF;
}


void error(char *fmt, ...){
  va_list ap;
  va_start(ap, fmt);
  vfprintf(stderr, fmt, ap);
  fprintf(stderr, "\n");
  exit(1);
}


int pos = 0;

enum {
  NODE_NUM = 256,
};


typedef struct node{
  int type;
  struct node *left;
  struct node *right;
  int val;
} Node;


Node *new_node(int op, Node *left, Node *right){
  Node *node = malloc(sizeof(Node));
  node->type = op;
  node->left = left;
  node->right = right;
  return node;
}


Node *new_node_num(int val){
  Node *node = malloc(sizeof(Node));
  node->type = NODE_NUM;
  node->val = val;
  return node;
}


Node *number(){
  if (tokens[pos].type == TOKEN_NUM)
    return new_node_num(tokens[pos++].val);
  error("[*] number expected, but got %s", tokens[pos].input);
}


Node *expr(){
  Node *left = number();
  for (;;){
    int op = tokens[pos].type;
    if (op != '+' && op != '-')
      break;
    pos++;
    left = new_node(op, left, number());
  }

  if (tokens[pos].type != TOKEN_EOF)
    error("[*] stray token: %s", tokens[pos].input);
  return left;
}


char *regs[] = {
  "rdi", "rsi", "r10", "r11", "r12", "r13", "r14", "r15", NULL
};
int cur=0;


char *gen(Node *node) {
  if (node->type == NODE_NUM) {
    char *reg = regs[cur++];
    if (!reg)
      error("[*] register exhausted");
    printf("  mov %s, %d\n", reg, node->val);
    return reg;
  }

  char *dst = gen(node->left);
  char *src = gen(node->right);

  switch (node->type) {
  case '+':
    printf("  add %s, %s\n", dst, src);
    return dst;
  case '-':
    printf("  sub %s, %s\n", dst, src);
    return dst;
  default:
    assert(0 && "[*] unknown operator");
  }
}


int main(int argc, char **argv) {
  if (argc != 2) {
    fprintf(stderr, "Usage: 9cc <code>\n");
    return 1;
  }
  
  tokenize(argv[1]);
  Node* node = expr();

  printf(".intel_syntax noprefix\n");
  printf(".global main\n");
  printf("main:\n");
  printf("  mov rax, %s\n", gen(node));
  printf("  ret\n");
  return 0;
}
