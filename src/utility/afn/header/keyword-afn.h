#ifndef KEYWORD_AFN_H
#define KEYWORD_AFN_H

typedef struct {
    int state;
    int is_final;
} KeywordState;

int is_keyword_char(char c);
int is_keyword(const char* input);
KeywordState transition(KeywordState current, char c);

#endif
