#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "opstack.h"

void stack_init(OpStack *s)
{
    s->capacity = 10;
    s->top = -1;
    s->data = malloc((size_t)s->capacity * sizeof(char *));
    if (!s->data) {
        fprintf(stderr, "Ошибка: не удалось выделить память для стека\n");
        exit(1);
    }
}

void stack_push(OpStack *s, const char *op)
{
    if (s->top >= s->capacity - 1) {
        s->capacity *= 2;
        char **tmp = realloc(s->data, (size_t)s->capacity * sizeof(char *));
        if (!tmp) {
            fprintf(stderr, "Ошибка: не удалось увеличить стек\n");
            exit(1);
        }
        s->data = tmp;
    }

    s->top++;
    s->data[s->top] = malloc(strlen(op) + 1);
    if (!s->data[s->top]) {
        fprintf(stderr, "Ошибка: не удалось скопировать оператор\n");
        exit(1);
    }
    strcpy(s->data[s->top], op);
}

char *stack_pop(OpStack *s)
{
    if (s->top < 0)
        return NULL;
    return s->data[s->top--];
}

char *stack_peek(OpStack *s)
{
    if (s->top < 0)
        return NULL;
    return s->data[s->top];
}

void stack_free(OpStack *s)
{
    if (!s->data)
        return;
    for (int i = 0; i <= s->top; i++)
        free(s->data[i]);
    free(s->data);
    s->data = NULL;
    s->top = -1;
    s->capacity = 0;
}
