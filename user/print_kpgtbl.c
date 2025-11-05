#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int
main(int argc, char *argv[])
{
  // Call the system call that prints kernel page table
  kpgtbl(); // this syscall is provided by the lab
  exit(0);
}
