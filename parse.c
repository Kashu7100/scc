#include "scc.h"


static Vector *tokens;
static int pos;


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
  Token *t = tokens->data[pos];
  if (t->type != TOKEN_NUM)
    error("[*] number expected, but got %s", t->input);
  pos++;
  return new_node_num(t->val);
}


Node *expr(){
  Node *left = number();
  for (;;){
    Token *t = tokens->data[pos];
    int op = t->type;
    if (op != '+' && op != '-')
      break;
    pos++;
    left = new_node(op, left, number());
  }
  Token *t = tokens->data[pos];
  if (t->type != TOKEN_EOF)
    error("[*] stray token: %s", t->input);
  return left;
}


Node *parse(Vector *v) {
  tokens = v;
  pos = 0;
  return expr();
}
