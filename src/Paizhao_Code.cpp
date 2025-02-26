#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include <HardwareSerial.h>

#define MAX_SIZE 100

char* replaceString(char* str, int n, int y, int r) {
    char* newStr = (char*) malloc(sizeof(char) * MAX_SIZE);
    if (newStr == NULL) {
        printf("Memory allocation failed\n");
        exit(1);
    }

    int i, j = 0;
    for (i = 0; str[i] != '\0'; i++) {
        if (str[i] == 'n') {
            newStr[j++] = (char) (n + '0');
        } else if (str[i] == 'y') {
            newStr[j++] = (char) (y + '0');
        } else if (str[i] == 'r') {
            newStr[j++] = (char) (r + '0');
        } else {
            newStr[j++] = str[i];
        }
    }
    newStr[j] = '\0';

    return newStr;
}

long intPow(long base, long exp) {
    long result = 1;
    while (exp > 0) {
        if (exp % 2 == 1) {
            result *= base;
        }
        base *= base;
        exp /= 2;
    }
    return result;
}

int precedence(char op) {
    if (op == '+' || op == '-') {
        return 1;
    } else if (op == '*' || op == '/') {
        return 2;
    } else if (op == '%' || op == '^') {
        return 3;
    } else {
        return 0;
    }
}


int toInt(const char* str, int l) {
    char inc[l + 1];
    for (int i = 0; i < l; ++i) {
        *(inc + i) = *(str + i);
    }
    *(inc + l) = '\0';
    return atoi(inc);
}

void infixToPostfix(char* infix, char* postfix) {
    char c[MAX_SIZE];
    int l = 0;

    int i, k = 0;

    for (i = 0; infix[i] != '\0'; i++) {
        if (isalnum(infix[i])) {
            int j = i;
            for (; infix[j] != '\0'; j++) {
                if (!isalnum(infix[j])) {
                    break;
                }
                postfix[k++] = infix[j];
            }
            postfix[k++] = ' ';
            i = j - 1;
            continue;
        }
        if (infix[i] == '(') {
            c[l++] = infix[i];
            continue;
        }
        if (infix[i] == ')') {
            while (l >= 0 && c[l - 1] != '(') {
                postfix[k++] = c[--l];
            }
            if (l >= 0 && c[l - 1] == '(') {
                l--;
                // Discard '('
            }
            continue;
        }   // Operator
        while (l >= 0 && precedence(c[l - 1]) >= precedence(infix[i])) {
            postfix[k++] = c[--l];
        }
        c[l++] = infix[i];

    }

    while (l > 0) {
        postfix[k++] = c[--l];

    }

    postfix[k] = '\0';

}

int calculate(const char* str) {
    int c[MAX_SIZE];
    int l = 0;

    for (int i = 0; str[i] != '\0'; i++) {
        if (str[i] == ' ') {
            continue;
        }
        if (isalnum(str[i])) {
            const char* s = str + i;
            for (int j = i; str[j] != '\0'; j++) {
                if (isalnum(str[j])) {
                    continue;
                }
                c[l++] = toInt(s, j - i);
                i = j - 1;
                break;
            }
            continue;
        }
        int b = c[--l];
        int a = c[--l];
        switch (str[i]) {
            case '+':;
                c[l] = a + b;
                break;
            case '-':
                c[l] = a - b;
                break;
            case '*':
                c[l] = a * b;
                break;
            case '/':
                c[l] = a / b;
                break;
            case '^':
                c[l] = intPow(a, b);
                break;
            case '%':
                c[l] = a % b;
                break;
        }
        l++;
    }
    return c[0];
}

int postfix(char infix[MAX_SIZE]) {
    char postfix[MAX_SIZE];
    infixToPostfix(infix, postfix);
    Serial.print("Postfix expression: ");
    Serial.println(postfix);
    int end = calculate(postfix);
    Serial.print("Calculation results: ");
    Serial.println(end);
    return end;
}