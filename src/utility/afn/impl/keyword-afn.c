#include "../header/keyword-afn.h"
#include <string.h>

int is_keyword_char(char c) {
    return (c >= 'a' && c <= 'z');
}

KeywordState transition(KeywordState current, char c) {
    KeywordState next = {0, 0};
    
    switch (current.state) {
        case 0:
            if (c == 'p') next.state = 1;
            else if (c == 'v') next.state = 8;
            else if (c == 'b') next.state = 13;
            else if (c == 'e') next.state = 17;
            else if (c == 't') next.state = 24;
            else if (c == 'i') next.state = 29;
            else if (c == 'w') next.state = 35;
            else if (c == 'd') next.state = 41;
            else if (c == 'f') next.state = 43;
            else if (c == 'c') next.state = 52;
            else if (c == 'r') next.state = 58;
            else if (c == 'a') next.state = 66;
            else if (c == 'o') next.state = 72;
            else if (c == 'u') next.state = 75;
            break;
        case 1:
            if (c == 'r') next.state = 2;
            break;
        case 2:
            if (c == 'o') next.state = 3;
            break;
        case 3:
            if (c == 'g') next.state = 4;
            break;
        case 4:
            if (c == 'r') next.state = 5;
            break;
        case 5:
            if (c == 'a') next.state = 6;
            break;
        case 6:
            if (c == 'm') {
                next.state = 7;
                next.is_final = 1;
            }
            break;
        case 8:
            if (c == 'a') next.state = 9;
            break;
        case 9:
            if (c == 'r') {
                next.state = 10;
                next.is_final = 1;
            }
            break;
        case 13:
            if (c == 'e') next.state = 14;
            break;
        case 14:
            if (c == 'g') next.state = 15;
            break;
        case 15:
            if (c == 'i') next.state = 16;
            break;
        case 16:
            if (c == 'n') {
                next.state = 17;
                next.is_final = 1;
            }
            break;
        case 17:
            if (c == 'n') next.state = 18;
            else if (c == 'l') next.state = 19;
            break;
        case 19:
            if (c == 's') next.state = 20;
            break;
        case 20:
            if (c == 'e') {
                next.state = 21;
                next.is_final = 1;
            }
            break;
        case 24:
            if (c == 'h') next.state = 25;
            else if (c == 'o') next.state = 26;
            else if (c == 'y') {
                next.state = 27;
                next.is_final = 1;
            }
            break;
        case 25:
            if (c == 'e') next.state = 26;
            break;
        case 26:
            if (c == 'n') {
                next.state = 27;
                next.is_final = 1;
            }
            break;
        case 29:
            if (c == 'f') {
                next.state = 30;
                next.is_final = 1;
            }
            break;
        case 35:
            if (c == 'h') next.state = 36;
            break;
        case 36:
            if (c == 'i') next.state = 37;
            break;
        case 37:
            if (c == 'l') next.state = 38;
            break;
        case 38:
            if (c == 'e') {
                next.state = 39;
                next.is_final = 1;
            }
            break;
        case 41:
            if (c == 'o') {
                next.state = 42;
                next.is_final = 1;
            }
            break;
        case 43:
            if (c == 'o') next.state = 44;
            else if (c == 'u') next.state = 45;
            break;
        case 44:
            if (c == 'r') {
                next.state = 45;
                next.is_final = 1;
            }
            break;
        case 52:
            if (c == 'o') next.state = 53;
            break;
        case 53:
            if (c == 'n') next.state = 54;
            break;
        case 54:
            if (c == 's') next.state = 55;
            break;
        case 55:
            if (c == 't') {
                next.state = 56;
                next.is_final = 1;
            }
            break;
        case 58:
            if (c == 'e') next.state = 59;
            break;
        case 59:
            if (c == 'p') next.state = 60;
            else if (c == 'a') next.state = 61;
            else if (c == 'c') next.state = 62;
            break;
        case 60:
            if (c == 'e') next.state = 61;
            break;
        case 61:
            if (c == 'a') next.state = 62;
            break;
        case 62:
            if (c == 't') {
                next.state = 63;
                next.is_final = 1;
            }
            break;
        case 66:
            if (c == 'r') next.state = 67;
            break;
        case 67:
            if (c == 'r') next.state = 68;
            break;
        case 68:
            if (c == 'a') next.state = 69;
            break;
        case 69:
            if (c == 'y') {
                next.state = 70;
                next.is_final = 1;
            }
            break;
        case 72:
            if (c == 'f') {
                next.state = 73;
                next.is_final = 1;
            }
            break;
        case 75:
            if (c == 'n') next.state = 76;
            break;
        case 76:
            if (c == 't') next.state = 77;
            break;
        case 77:
            if (c == 'i') next.state = 78;
            break;
        case 78:
            if (c == 'l') {
                next.state = 79;
                next.is_final = 1;
            }
            break;
    }
    return next;
}

int is_keyword(const char* input) {
    KeywordState current = {0, 0};
    int i = 0;

    while (input[i] != '\0') {
        char c = input[i];
        current = transition(current, c);
        if (current.state == 0) {
            return 0;
        }
        i++;
    }

    return current.is_final;
}
