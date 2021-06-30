#include "scc.h"


IR *new_ir(int op, int left, int right) {
  IR *ir = malloc(sizeof(IR));
  ir->op = op;
  ir->left = left;
  ir->right = right;
  return ir;
}


static int gen_ir_sub(Vector *v, Node *node) {
  static int regno;
  if (node->type == NODE_NUM) {
    int r = regno++;
    vec_push(v, new_ir(IR_IMM, r, node->val));
    return r;
  }

  assert(node->type == '+' || node->type == '-');

  int left = gen_ir_sub(v, node->left);
  int right = gen_ir_sub(v, node->right);

  vec_push(v, new_ir(node->type, left, right));
  vec_push(v, new_ir(IR_KILL, right, 0));
  return left;
}


Vector *gen_ir(Node *node) {
  Vector *v = new_vec();
  int r = gen_ir_sub(v, node);
  vec_push(v, new_ir(IR_RETURN, r, 0));
  return v;
}