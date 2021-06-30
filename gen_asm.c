# include "scc.h"

void gen_x86(Vector *irv) {
  for (int i = 0; i < irv->len; i++) {
    IR *ir = irv->data[i];

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