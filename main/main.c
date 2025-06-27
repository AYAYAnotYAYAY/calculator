#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>

#pragma warning(disable:4996)

// Define NAN if not available
#ifndef NAN
#define NAN (0.0/0.0)
#endif

// Structure for elements in the stack/list
struct yuansu {
    char data;
    double num;
    struct yuansu* next;
    struct yuansu* father;
};

// Function to free the entire linked list to prevent memory leaks
void free_list(struct yuansu* node) {
    struct yuansu* tmp;
    while (node != NULL) {
        tmp = node;
        node = node->next;
        free(tmp);
    }
}

// Pop an element from the stack
struct yuansu* pop(struct yuansu* R) {
    if (R == NULL || R->father == NULL) { // Cannot pop the head sentinel
        return R;
    }
    struct yuansu* temp = R->father;
    temp->next = NULL;
    free(R);
    return temp;
}

// Push an element onto the stack
struct yuansu* push(struct yuansu* zz, char a, double i) {
    struct yuansu* temp = (struct yuansu*)malloc(sizeof(struct yuansu));
    if (temp == NULL) {
        printf("Error: Memory allocation failed.\n");
        exit(1);
    }
    if (zz != NULL) {
        zz->next = temp;
    }
    temp->father = zz;
    temp->next = NULL;
    temp->num = i;
    temp->data = a;
    return temp;
}

// Calculate the result from a postfix expression list
double sum(struct yuansu* S) {
    struct yuansu* H = push(NULL, '#', 0); // Use a temporary stack for calculation

    struct yuansu* current = S;
    while (current != NULL) {
        if (current->data == ' ') { // If it's a number
            H = push(H, ' ', current->num);
        }
        else { // If it's an operator
            if (H == NULL || H->father == NULL || H->father->father == NULL) {
                printf("Error: Invalid expression format (not enough operands)!\n");
                free_list(H);
                return NAN;
            }
            double B = H->num; H = pop(H);
            double A = H->num; H = pop(H);

            switch (current->data) {
            case '+': H = push(H, ' ', A + B); break;
            case '-': H = push(H, ' ', A - B); break;
            case '*': H = push(H, ' ', A * B); break;
            case '/':
                if (B == 0) {
                    printf("Error: Division by zero!\n");
                    free_list(H);
                    return NAN;
                }
                H = push(H, ' ', A / B); break;
            case '^': H = push(H, ' ', pow(A, B)); break;
            default:
                printf("Unknown operator: %c\n", current->data);
                free_list(H);
                return NAN;
            }
        }
        current = current->next;
    }

    if (H == NULL || H->father == NULL || H->father->father != NULL) {
        printf("Error: Invalid expression format (too many operands)!\n");
        free_list(H);
        return NAN;
    }

    double result = H->num;
    free_list(H);
    return result;
}

// Check operator precedence
int panduan(struct yuansu* S, char suanshi) {
    char top_op = S->data;

    if (suanshi == '(') return 1; // Always push '('

    if (suanshi == ')') return 2; // Handle ')'

    int p1 = 0, p2 = 0; // Precedence levels

    if (top_op == '+' || top_op == '-') p1 = 1;
    else if (top_op == '*' || top_op == '/') p1 = 2;
    else if (top_op == '^') p1 = 3;
    else if (top_op == '(') p1 = 0; // '(' on stack has low precedence

    if (suanshi == '+' || suanshi == '-') p2 = 1;
    else if (suanshi == '*' || suanshi == '/') p2 = 2;
    else if (suanshi == '^') p2 = 4; // Right-associative for '^

    if (p2 > p1) {
        return 1; // Push if incoming has higher precedence
    }
    else {
        return 0; // Pop otherwise
    }
}

// Convert infix string to postfix linked list
struct yuansu* zhuanhuan(const char* suanshi) {
    struct yuansu* F_head, * F_tail, * S_top;
    char* p = (char*)suanshi;
    char* end;

    F_head = F_tail = push(NULL, '#', 0); // Postfix expression list
    S_top = push(NULL, '#', 0);           // Operator stack

    while (*p != '\0') {
        if (*p >= '0' && *p <= '9' || *p == '.') {
            double num = strtod(p, &end);
            F_tail = push(F_tail, ' ', num);
            p = end;
            continue;
        }
        else if (*p == ' ') {
            p++;
            continue;
        }
        else if (strchr("+-*/^()", *p)) {
            char op = *p;
            if (op == '(') {
                S_top = push(S_top, op, 0);
            } else if (op == ')') {
                while (S_top->data != '#' && S_top->data != '(') {
                    F_tail = push(F_tail, S_top->data, 0);
                    S_top = pop(S_top);
                }
                if (S_top->data == '(') {
                    S_top = pop(S_top); // Pop '('
                } else {
                    printf("Error: Mismatched parentheses!\n");
                    free_list(F_head);
                    free_list(S_top);
                    return NULL;
                }
            } else {
                while (S_top->data != '#' && panduan(S_top, op) == 0) {
                    F_tail = push(F_tail, S_top->data, 0);
                    S_top = pop(S_top);
                }
                S_top = push(S_top, op, 0);
            }
            p++;
        }
        else {
            printf("Unknown character in expression: %c\n", *p);
            free_list(F_head);
            free_list(S_top);
            return NULL;
        }
    }

    while (S_top->data != '#') {
        if (S_top->data == '(') {
            printf("Error: Mismatched parentheses!\n");
            free_list(F_head);
            free_list(S_top);
            return NULL;
        }
        F_tail = push(F_tail, S_top->data, 0);
        S_top = pop(S_top);
    }

    free_list(S_top); // Free the operator stack

    struct yuansu* result_head = F_head->next;
    if (result_head) result_head->father = NULL;
    free(F_head); // Free the sentinel node
    return result_head;
}

int main() {
    printf("Scientific Calculator. Enter expression (or 'exit' to quit):\n");
    char cm[1024];

    while (fgets(cm, sizeof(cm), stdin)) {
        // Remove newline character from fgets
        cm[strcspn(cm, "\n")] = 0;

        if (strcmp(cm, "exit") == 0) {
            break;
        }
        
        if (strlen(cm) == 0) {
            continue;
        }

        struct yuansu* p, * q;
        p = zhuanhuan(cm);
        if (p == NULL) {
            printf("Please enter a valid expression.\n> ");
            continue;
        }

        q = p;
        printf("Postfix: ");
        while (p != NULL) {
            if (p->data == ' ') {
                printf("%.4g ", p->num);
            }
            else {
                printf("%c ", p->data);
            }
            p = p->next;
        }
        printf("\n");

        double result = sum(q);
        if (!isnan(result)) {
            printf("Result: %.4g\n", result);
        }

        free_list(q); // Free the memory for the current expression

        printf("> ");
    }
    printf("Exiting calculator.\n");
    return 0;
}