#ifndef OPSTACK_H
#define OPSTACK_H

typedef struct {
    char **data;
    int top;
    int capacity;
} OpStack;

void stack_init(OpStack *s);
void stack_push(OpStack *s, const char *op);
char *stack_pop(OpStack *s);
char *stack_peek(OpStack *s);
void stack_free(OpStack *s);

#endif
