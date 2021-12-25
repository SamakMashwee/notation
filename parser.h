#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#ifndef PDEF
#define PDEF static inline
#endif

#ifndef MAX_COMPLEXITY
#define MAX_COMPLEXITY 128
#endif

#ifndef MAX_LENGTH
#define MAX_LENGTH 256
#endif

// state machine for the text parser.
typedef enum {
    STATE_parentheses,
    STATE_findstart,
    STATE_operation,
    STATE_variable,
    STATE_number,
    STATE_trig,
    STATE_end,
    STATE_error
} p_state;

// current parser data
typedef struct {
    char *input;
    int pos;
    int token_count;
    int token_pos;
    char **tokens;
    p_state state;
} p_data;

// data for the compiled string parser
typedef struct {
    char *c_string;
    int pos;
} c_data;

static char *accepted_inputs = "^+-/*[]{}()1234567890sincotaexlg";
static char *accepted_functions[7] = {"sin", "csc", "cos", "sec", "tan", "cot", "log"};
static char *function_shorthand = "sScCtTl";

// returns whether or not a character exists in a string.
PDEF bool isin(char c, char *s) {
    int i = 0;
    for( ; s[i] != c; i++) { continue; }
    return i < strlen(s);
}

PDEF char *findnum(p_data *data) {
    int length = 0;
    while(isin(data -> input[data -> pos + length], "0123456789"))
        length++;
    
    char *output = (char *)malloc(sizeof(char) * length);
    for(int i = 0; i < length; i++) {
        output[i] = data -> input[data -> pos + i];
    }

    data -> pos += length;
    data -> token_pos++;
    
    return output;
}

PDEF void insert_char(p_data *data, char c) {
    data -> tokens[data -> token_pos] = (char *) malloc(sizeof(char) * 2);
    data -> tokens[data -> token_pos][0] = c;
    data -> tokens[data -> token_pos][1] = '\0';
    return;
}

PDEF void add_ctoken(p_data *data, char c) {
    insert_char(data, c);
    data -> token_pos++;
    data -> pos++;
}

PDEF char encode_trig(char *s) {
    for(int i = 0;i < 7; i++) {
        for(int j = 0; j < 3; j++) {
            if(s[j] == accepted_functions[i][j] && j == 2)
                return function_shorthand[i];
        }
    }
    return '\0';
}

PDEF char *preprocess(p_data *data) {
    int spc_cnt = 0;
    char *p_string = malloc(sizeof(char) * strlen(data -> input));
    for(int i = 0; i < strlen(data -> input); i++) {
        if(data -> input[i] != ' ')
            p_string[i - spc_cnt] = data -> input[i];
        else spc_cnt++;
    }
    return p_string;
}

PDEF void tokenize(p_data *data) {
    data -> state = STATE_findstart;

    char *curstr = data -> input;
    // loops until the parser has either reached the end of the function or has encountered an error.
    while(data -> state != STATE_error || data -> state != STATE_end) { 
        switch (data -> state) {
            case STATE_findstart:

                if(isin(curstr[0], "sincotaelg")) { data -> state = STATE_trig; continue; }
                
                else if(isin(curstr[0], "^+/*-")) { data -> state = STATE_operation; continue; }

                else if(isin(curstr[0], "()[]{}")) { data -> state = STATE_parentheses; continue; }

                else if(curstr[0] == 'x') { data -> state = STATE_variable; continue; }

                else if(curstr[0] >= '0' && curstr[0] <= '9') { data -> state = STATE_number; continue; }

                else if(curstr[0] == '\0' || curstr[0] == '\n') { data -> state = STATE_end; continue; }

                else { data -> state = STATE_error; continue; }
            break;

            case STATE_variable:
                add_ctoken(data, 'x');
                //printf("vars\n");
                data -> state = STATE_findstart;
            break;

            case STATE_number:
                data -> tokens[data -> token_pos] = findnum(data);
                //printf("nums\n");
                data -> state = STATE_findstart;
            break;

            case STATE_trig:
                if(encode_trig(curstr) != '\0') {
                    insert_char(data, encode_trig(curstr));
                    //printf("trig\n");
                    data -> token_pos++;
                    data -> pos += 3;
                    data -> state = STATE_findstart;
                }

                else data -> state = STATE_error;
            break;

            case STATE_operation:
                add_ctoken(data, curstr[0]);
                //printf("oper\n");
                data -> state = STATE_findstart;
            break;

            case STATE_parentheses:
                add_ctoken(data, curstr[0]);
                //printf("parn\n");
                data -> state = STATE_findstart;
            break;

            case STATE_end:
                return;
            break;

            case STATE_error:
                printf("error.\n");
                return;
            break;
        }
        curstr = data -> input + data -> pos;
    }
}

PDEF void addstr(char *init, int pos, char *str) {
    for(int i = 0; i < strlen(str); i++)
        init[i + pos] = str[i];
    return;
}

// a function that converts tokenized input into a makestring
PDEF char *mkstr(p_data *data) {
    char *output = (char *) malloc(sizeof(char) * strlen(data -> input) * 2);
    int pos = 0;
    for(int i = 0; i < data -> token_pos; i++) {
        switch (pos) {
            continue;
        }
    }
    return output;
}

// a function that compiles the input into a makestring
PDEF char *compile(p_data *data) {
    data -> pos = 0;
    data -> token_pos = 0;
    data -> token_count = 0;
    data -> tokens = (char **) malloc(sizeof(char *) * MAX_COMPLEXITY);
    data -> input = preprocess(data);
    tokenize(data);
    mkstr(data);
    return "\0";
}

// a function that prints all of the tokens of a parser dataset object
void print_tokens(p_data *data) {
    for(int i = 0; i < data -> token_pos; i++) {
        printf("%s\n", data -> tokens[i]);
    }
    return;
}

// function that evaluates the value of tokens given an array of tokens as strings.
float run(float x, p_data *data) {
    
} 