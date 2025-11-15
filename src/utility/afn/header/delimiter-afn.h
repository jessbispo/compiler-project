#ifndef DELIMITER_H
#define DELIMITER_H

typedef enum {
    DEL_Q1, DEL_Q2, DEL_Q3, DEL_Q4, DEL_Q5, DEL_Q6, DEL_Q7, DEL_Q8, DEL_Q9, DEL_Q10, DEL_Q11
} DelimiterState;

DelimiterState delimiter_next_state(DelimiterState current_state, char character);
int delimiter_afn(const char *input);

#endif /* DELIMITER_H */
