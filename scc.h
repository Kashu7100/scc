#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>
#include <stdarg.h>
#include <assert.h>
#include <stdnoreturn.h>


noreturn void error(char *fmt, ...);


// Vector
typedef struct {
  void **data;
  int capacity;
  int len;
} Vector;


Vector *new_vec();
void vec_push(Vector *v, void *elem);


// tokens
enum{
  TOKEN_NUM = 256,
  TOKEN_EOF,
};


typedef struct {
  int type;
  int val;
  char *input; // token string
} Token;


Vector *tokenize(char *p);


// nodes
enum {
  NODE_NUM = 256,
};


typedef struct node{
  int type;
  struct node *left;
  struct node *right;
  int val;
} Node;


Node *number();
Node *expr();
Node *parse(Vector *tokens);


// Intermediate representation
enum {
  IR_IMM,
  IR_MOV,
  IR_RETURN,
  IR_KILL,
  IR_NOP,
};


typedef struct {
  int op;
  int left;
  int right;
} IR;


Vector *gen_ir(Node *node);


/// regalloc.c
extern char *regs[];
void alloc_regs(Vector *irv);


// code generator
void gen_x86(Vector *irv);