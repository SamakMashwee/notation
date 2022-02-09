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
    STATE_par,
    STATE_str,
    STATE_opr,
    STATE_var,
    STATE_num,
    STATE_trg,
    STATE_end,
    STATE_err
} p_state;

typedef struct astnode {
    char *contents;
    int operation;
    astnode *left;
    astnode *right;
} astnode;

// current parser data
typedef struct {
    char *input;
    int pos;
    int token_pos;
    int token_cnt;
    char **tokens;
    p_state state;
    char *mkstr;
} p_data;

static char *function_shorthand = "sScCtTl"; 
static char *accepted_inputs = "^+-/*[]{}()1234567890sincotaexlg";
static char *accepted_functions[7] = {"sin", "csc", "cos", "sec", "tan", "cot", "log"};

// returns whether or not a character exists in a string.
PDEF bool isin(char c, char *s) {
    int i = 0;
    for( ; s[i] != c; i++) { continue; }
    return i < strlen(s);
}

// returns the string corresponding to consecutive integers in the input.
PDEF char *findnum(p_data *data) {
    int length = 0;
    while(isin(data -> mkstr[data -> pos + length], "0123456789"))
        length++;
    
    char *output = (char *) malloc(sizeof(char) * length);
    for(int i = 0; i < length; i++) {
        output[i] = data -> mkstr[data -> pos + i];
    }

    data -> pos += length;
    data -> token_pos++;
    
    return output;
}

// inserts a character as a string token into the token array.
PDEF void add_ctoken(p_data *data, char c) {
    data -> tokens[data -> token_pos] = (char *) malloc(sizeof(char) * 2);
    data -> tokens[data -> token_pos][0] = c;
    data -> tokens[data -> token_pos][1] = '\0';
    data -> token_pos++;
    data -> pos++;
    return;
}

// simpifies trig functions to a single character corresponding to that function.
PDEF char encode_trig(char *s) {
    for(int i = 0;i < 7; i++)
        for(int j = 0; j < 3; j++)
            if(s[j] == accepted_functions[i][j] && j == 2) return function_shorthand[i];
    return '\0';
}

// preprocessing done to input in order to produce a makestring.
PDEF void preprocess(p_data *data) {
    int length = strlen(data -> input);
    data -> mkstr = (char *) malloc(sizeof(char) * length * 2);

    // eats whitespace.
    int spc_cnt = 0;
    char *a_string = (char *) malloc(sizeof(char) * length);
    for(int i = 0; i < length; i++) {
        if(data -> input[i] != ' ') a_string[i - spc_cnt] = data -> input[i];
        else spc_cnt++;
    }

    // encodes trig functions.
    char *b_string = (char *) malloc(sizeof(char) * length);
    for(int i = 0, j = 0; j < strlen(a_string); i++, j++) {
        if(isin(a_string[j], "sctl")) {
            if(encode_trig(a_string + j) != '\0') {
                b_string[i] = encode_trig(a_string + j);
                j+=2;
            } else data -> state = STATE_err;
        } else b_string[i] = a_string[j];
    } free(a_string);

    // insert multiplication symbol where implied by mathematical notation: 2x, xsinx, 3(2-1), etc...
    for(int i = 0, j = 0; i < strlen(b_string); i++, j++) {
        if((isin(b_string[i+1], "sScCtTl") && !isin(b_string[i], "sScCtTl({[/+-*^")  )
        || (isin(b_string[i+1], "([{")     && !isin(b_string[i], "([{sScCtTl")       )
        || (isin(b_string[i], ")}]0123456789") && isin(b_string[i+1], "([{xsScCtTl") )
        || (b_string[i] == 'x' && isin(b_string[i+1], "0123456789sScCtTl([{x")       )) {
            data -> mkstr[j] = b_string[i]; j++; 
            data -> mkstr[j] = '*';
        } else { data -> mkstr[j] = b_string[i]; }
    } free(b_string);
}

// returns the current state of the parser based on the type of the character that is being parsed.
PDEF p_state identify(char c) {
    if(isin(c, function_shorthand)) return STATE_trg;
    else if(isin(c, "^+/*-"))       return STATE_opr;
    else if(isin(c, "()[]{}"))      return STATE_par;
    else if(c == 'x')               return STATE_var;
    else if(c >= '0' && c <= '9')   return STATE_num;
    else if(c == '\0' || c == '\n') return STATE_end;
    return STATE_err;
}

// converts string input into tokenized input.
PDEF void tokenize(p_data *data) {
    
    if(data -> state == STATE_err) {
        printf("error.\n");
        return;
    } data -> state = STATE_str;

    // state machine loop until end or error encountered.
    char *curstr = data -> mkstr;
    while(data -> state != STATE_err || data -> state != STATE_end) { 
        
        switch(data -> state) {
            case STATE_str: 
                data -> state = identify(curstr[0]); break;

            case STATE_num:
                data -> tokens[data -> token_pos] = findnum(data);
                data -> state = STATE_str; break;
                
            case STATE_end: return; break;

            default:
                add_ctoken(data, curstr[0]);
                data -> state = STATE_str; break;
        }
        curstr = data -> mkstr + data -> pos;
    }
}

// compiles input data into a makestring and tokenizes the makestring
PDEF void compile(p_data *data) {
    preprocess(data);

    data -> tokens = (char **) malloc(sizeof(char *) * MAX_COMPLEXITY);
    data -> pos = 0;
    data -> token_pos = 0;

    tokenize(data);

    data -> token_cnt = data -> token_pos;
    data -> pos = 0;
    data -> token_pos = 0; 
}

// a function that prints all of the tokens of a parser dataset object
void print_tokens(p_data *data) {
    for(int i = 0; i < data -> token_cnt; i++)
        printf("%s\n", data -> tokens[i]);
    
    return;
}

PDEF float evaluate(p_data *data) {
    
}