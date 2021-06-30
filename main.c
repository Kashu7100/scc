#include "scc.h"

int main(int argc, char **argv) {
  if (argc != 2) {
    fprintf(stderr, "Usage: scc <code>\n");
    return 1;
  }
  
  Vector *tokens = tokenize(argv[1]);
  Node* node = parse(tokens);

  Vector *irv = gen_ir(node);
  alloc_regs(irv);

  printf(".intel_syntax noprefix\n");
  printf(".global main\n");
  printf("main:\n");
  
  gen_x86(irv);
  return 0;
}
