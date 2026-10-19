#include "kernel/types.h"
#include "kernel/param.h"
#include "user/user.h"

#define MAX_THREAD 4
#define STACK_SIZE 4096

struct context { uint64 ra, sp, s0, s1, s2, s3, s4, s5, s6, s7, s8, s9, s10, s11; };
struct thread { char *stack; int state; struct context context; };
struct thread all_thread[MAX_THREAD];
struct thread *current_thread;
enum thread_state { FREE, RUNNING, RUNNABLE };
extern void thread_switch(uint64, uint64);

void thread_schedule(void);
void thread_entry(void) { void (*f)(void) = (void (*)(void))current_thread->context.s0; f(); current_thread->state = FREE; thread_schedule(); exit(0); }

void thread_init(void) { current_thread = &all_thread[0]; current_thread->state = RUNNING; }

void thread_create(void (*func)()) {
  for (int i = 0; i < MAX_THREAD; i++) if (all_thread[i].state == FREE) {
    all_thread[i].state = RUNNABLE;
    all_thread[i].stack = malloc(STACK_SIZE);
    all_thread[i].context.sp = (uint64)(all_thread[i].stack + STACK_SIZE);
    all_thread[i].context.ra = (uint64)thread_entry;
    all_thread[i].context.s0 = (uint64)func;
    return;
  }
}

void thread_schedule(void) {
  struct thread *t, *next = 0;
  for (int i = 1; i <= MAX_THREAD; i++) {
    t = &all_thread[(current_thread - all_thread + i) % MAX_THREAD];
    if (t->state == RUNNABLE) { next = t; break; }
  }
  if (!next) { printf("thread_schedule: no runnable threads\n"); exit(0); }
  if (current_thread != next) {
    t = current_thread; current_thread = next; next->state = RUNNING;
    thread_switch((uint64)&t->context, (uint64)&next->context);
  }
}

void thread_a(void) { for (int i = 0; i < 100; i++) { printf("thread_a %d\n", i); thread_schedule(); } printf("thread_a: exit after 100\n"); }
void thread_b(void) { for (int i = 0; i < 100; i++) { printf("thread_b %d\n", i); thread_schedule(); } printf("thread_b: exit after 100\n"); }
void thread_c(void) { for (int i = 0; i < 100; i++) { printf("thread_c %d\n", i); thread_schedule(); } printf("thread_c: exit after 100\n"); }

int main(int argc, char *argv[]) { thread_init(); thread_create(thread_a); thread_create(thread_b); thread_create(thread_c); thread_schedule(); exit(0); }
