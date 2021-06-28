#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>
#include <stdarg.h>
#include <assert.h>
#include <stdnoreturn.h>


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


noreturn void error(char *fmt, ...){
  va_list ap;
  va_start(ap, fmt);
  vfprintf(stderr, fmt, ap);
  fprintf(stderr, "\n");
  exit(1);
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


IR *new_ir(int op, int left, int right) {
  IR *ir = malloc(sizeof(IR));
  ir->op = op;
  ir->left = left;
  ir->right = right;
  return ir;
}


IR *ins[1000];
int inp;
int regno;


int gen_ir_sub(Node *node) {
  if (node->type == NODE_NUM) {
    int r = regno++;
    ins[inp++] = new_ir(IR_IMM, r, node->val);
    return r;
  }

  assert(node->type == '+' || node->type == '-');

  int left = gen_ir_sub(node->left);
  int right = gen_ir_sub(node->right);

  ins[inp++] = new_ir(node->type, left, right);
  ins[inp++] = new_ir(IR_KILL, right, 0);
  return left;
}


void gen_ir(Node *node) {
  int r = gen_ir_sub(node);
  ins[inp++] = new_ir(IR_RETURN, r, 0);
}


char *regs[] = {"rdi", "rsi", "r10", "r11", "r12", "r13", "r14", "r15"};
bool used[8];
int reg_map[1000];


int alloc(int ir_reg){
  if (reg_map[ir_reg] != -1){
    int r = reg_map[ir_reg];
    assert(used[r]);
    return r;
  }

  for (int i = 0; i < sizeof(regs) / sizeof(*regs); i++) {
    if (used[i])
      continue;
    used[i] = true;
    reg_map[ir_reg] = i;
    return i;
  }
  error("[*] register exhausted");
}


void kill(int r) {
  assert(used[r]);
  used[r] = false;
}


void alloc_regs() {
  for (int i = 0; i < inp; i++){
    IR *ir = ins[i];

    int op = ir->op;
    if (op == IR_IMM)
      ir->left = alloc(ir->left);
    else if (op == IR_MOV || op == '+' || op == '-'){
      ir->left = alloc(ir->left);
      ir->right = alloc(ir->right);
    } 
    else if (op == IR_RETURN)
      kill(reg_map[ir->left]);
    else if (op == IR_KILL){
      kill(reg_map[ir->left]);
      ir->op = IR_NOP;
    } else 
      assert(0 && "[*] unknown operator");   
  }
}


void gen_x86() {
  for (int i = 0; i < inp; i++) {
    IR *ir = ins[i];

    int op = ir->op;
    if (op == IR_IMM)
      printf("  mov %s, %d\n", regs[ir->left], ir->right);
    else if (op == IR_MOV)
      printf("  mov %s, %s\n", regs[ir->left], regs[ir->right]);
    else if (op == IR_RETURN){
      printf("  mov rax, %s\n", regs[ir->left]);
      printf("  ret\n");
    } 
    else if (op == '+')
      printf("  add %s, %s\n", regs[ir->left], regs[ir->right]);
    else if (op == '-')
      printf("  sub %s, %s\n", regs[ir->left], regs[ir->right]);
    else if (op != IR_NOP)
      assert(0 && "[*] unknown operator");
  }
}

int main(int argc, char **argv) {
  if (argc != 2) {
    fprintf(stderr, "Usage: scc <code>\n");
    return 1;
  }

  // init reg_map
  for (int i = 0; i < sizeof(reg_map) / sizeof(*reg_map); i++)
    reg_map[i] = -1;
  
  tokenize(argv[1]);
  Node* node = expr();

  gen_ir(node);
  alloc_regs();

  printf(".intel_syntax noprefix\n");
  printf(".global main\n");
  printf("main:\n");
  
  gen_x86();
  return 0;
}
