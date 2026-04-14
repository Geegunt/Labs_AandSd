#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "opstack.h"
#include "parser.h"

static int precedence(char oper)
{
    if (oper == '~')
        return 4;
    if (oper == '^')
        return 3;
    if (oper == '*' || oper == '/')
        return 2;
    if (oper == '+' || oper == '-')
        return 1;
    return 0;
}

static void parser_abort(char **tokens, int count, OpStack *s, char ***postfix_arr)
{
    for (int i = 0; i < count; i++)
        free(tokens[i]);
    free(tokens);
    stack_free(s);
    *postfix_arr = NULL;
}

static bool is_binary_op_token(const char *t)
{
    return t[0] != '\0' && t[1] == '\0' && strchr("+-*/^", t[0]) != NULL;
}

static bool is_unary_op_token(const char *t)
{
    return strcmp(t, "~") == 0;
}

static char *slice_token(const char *infix, int start, int end)
{
    const size_t n = (size_t)(end - start);
    char *t = malloc(n + 1);
    if (!t) {
        fprintf(stderr, "Ошибка: не хватило памяти\n");
        exit(1);
    }
    memcpy(t, infix + start, n);
    t[n] = '\0';
    return t;
}

bool postfix_valid(char **tok, int n)
{
    if (n <= 0)
        return false;

    int depth = 0;
    for (int i = 0; i < n; i++) {
        if (is_unary_op_token(tok[i])) {
            if (depth < 1)
                return false;
        } else if (is_binary_op_token(tok[i])) {
            if (depth < 2)
                return false;
            depth--;
        } else {
            depth++;
        }
    }
    return depth == 1;
}

int infix_to_postfix(const char *infix, char ***postfix_arr)
{
    OpStack op_stack;
    stack_init(&op_stack);

    int capacity = 16;
    int count = 0;
    char **tokens = malloc((size_t)capacity * sizeof(char *));
    if (!tokens) {
        fprintf(stderr, "Ошибка: не хватило памяти\n");
        exit(1);
    }

    const int len = (int)strlen(infix);
    int i = 0;

    while (i < len) {
        if (isspace((unsigned char)infix[i])) {
            i++;
            continue;
        }

        if (isdigit((unsigned char)infix[i]) ||
            (infix[i] == '-' && i + 1 < len && isdigit((unsigned char)infix[i + 1]) &&
             (i == 0 || infix[i - 1] == '(' || infix[i - 1] == '+' || infix[i - 1] == '-' ||
              infix[i - 1] == '*' || infix[i - 1] == '/' || infix[i - 1] == '^'))) {
            if (count >= capacity) {
                capacity *= 2;
                char **tmp = realloc(tokens, (size_t)capacity * sizeof(char *));
                if (!tmp) {
                    fprintf(stderr, "Ошибка: не хватило памяти\n");
                    exit(1);
                }
                tokens = tmp;
            }
            const int start = i;
            if (infix[i] == '-')
                i++;
            while (i < len && isdigit((unsigned char)infix[i]))
                i++;
            tokens[count++] = slice_token(infix, start, i);
            continue;
        }

        if (isalpha((unsigned char)infix[i])) {
            if (count >= capacity) {
                capacity *= 2;
                char **tmp = realloc(tokens, (size_t)capacity * sizeof(char *));
                if (!tmp) {
                    fprintf(stderr, "Ошибка: не хватило памяти\n");
                    exit(1);
                }
                tokens = tmp;
            }
            const int start = i;
            while (i < len && isalnum((unsigned char)infix[i]))
                i++;
            tokens[count++] = slice_token(infix, start, i);
            continue;
        }

        if (infix[i] == '(') {
            stack_push(&op_stack, "(");
            i++;
            continue;
        }

        if (infix[i] == ')') {
            while (op_stack.top >= 0 && strcmp(stack_peek(&op_stack), "(") != 0) {
                if (count >= capacity) {
                    capacity *= 2;
                    char **tmp = realloc(tokens, (size_t)capacity * sizeof(char *));
                    if (!tmp) {
                        fprintf(stderr, "Ошибка: не хватило памяти\n");
                        exit(1);
                    }
                    tokens = tmp;
                }
                tokens[count++] = stack_pop(&op_stack);
            }
            if (op_stack.top < 0 || strcmp(stack_peek(&op_stack), "(") != 0) {
                fprintf(stderr, "Ошибка: лишняя закрывающая скобка или нет пары для ')'\n");
                parser_abort(tokens, count, &op_stack, postfix_arr);
                return -1;
            }
            free(stack_pop(&op_stack));
            i++;
            continue;
        }

        if (strchr("+-*/^", infix[i]) != NULL) {
            if (infix[i] == '+') {
                const bool is_unary_plus =
                    (i == 0 || infix[i - 1] == '(' || infix[i - 1] == '+' ||
                     infix[i - 1] == '-' || infix[i - 1] == '*' || infix[i - 1] == '/' ||
                     infix[i - 1] == '^');
                if (is_unary_plus) {
                    i++;
                    continue;
                }
            }

            if (infix[i] == '-') {
                const bool is_unary_minus =
                    (i == 0 || infix[i - 1] == '(' || infix[i - 1] == '+' ||
                     infix[i - 1] == '-' || infix[i - 1] == '*' || infix[i - 1] == '/' ||
                     infix[i - 1] == '^');
                if (is_unary_minus) {
                    while (op_stack.top >= 0 &&
                           precedence(stack_peek(&op_stack)[0]) >= precedence('~')) {
                        if (count >= capacity) {
                            capacity *= 2;
                            char **tmp = realloc(tokens, (size_t)capacity * sizeof(char *));
                            if (!tmp) {
                                fprintf(stderr, "Ошибка: не хватило памяти\n");
                                exit(1);
                            }
                            tokens = tmp;
                        }
                        tokens[count++] = stack_pop(&op_stack);
                    }
                    stack_push(&op_stack, "~");
                    i++;
                    continue;
                }
            }

            const char incoming = infix[i];
            const int inc_prec = precedence(incoming);
            while (op_stack.top >= 0) {
                const char *peek_str = stack_peek(&op_stack);
                if (strcmp(peek_str, "(") == 0)
                    break;
                const int top_prec = precedence(peek_str[0]);
                const bool pop = (incoming == '^') ? (top_prec > inc_prec) : (top_prec >= inc_prec);
                if (!pop)
                    break;
                if (count >= capacity) {
                    capacity *= 2;
                    char **tmp = realloc(tokens, (size_t)capacity * sizeof(char *));
                    if (!tmp) {
                        fprintf(stderr, "Ошибка: не хватило памяти\n");
                        exit(1);
                    }
                    tokens = tmp;
                }
                tokens[count++] = stack_pop(&op_stack);
            }

            char op[2] = {infix[i], '\0'};
            stack_push(&op_stack, op);
            i++;
            continue;
        }

        fprintf(stderr, "Ошибка: недопустимый символ '%c' в позиции %d\n", infix[i], i);
        parser_abort(tokens, count, &op_stack, postfix_arr);
        return -1;
    }

    while (op_stack.top >= 0) {
        if (count >= capacity) {
            capacity *= 2;
            char **tmp = realloc(tokens, (size_t)capacity * sizeof(char *));
            if (!tmp) {
                fprintf(stderr, "Ошибка: не хватило памяти\n");
                exit(1);
            }
            tokens = tmp;
        }
        char *op = stack_pop(&op_stack);
        if (strcmp(op, "(") == 0) {
            fprintf(stderr, "Ошибка: не хватает закрывающей скобки\n");
            free(op);
            parser_abort(tokens, count, &op_stack, postfix_arr);
            return -1;
        }
        tokens[count++] = op;
    }
    stack_free(&op_stack);

    if (!postfix_valid(tokens, count)) {
        fprintf(stderr, "Ошибка: некорректное выражение (операнды и операторы не согласованы)\n");
        for (int j = 0; j < count; j++)
            free(tokens[j]);
        free(tokens);
        *postfix_arr = NULL;
        return -1;
    }

    *postfix_arr = tokens;
    return count;
}
