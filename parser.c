#include "parser.h"

int main(void) {
    p_data *data = malloc(sizeof(p_data));

    data -> input = (char *) malloc(sizeof(char) * MAX_LENGTH);
    printf("function: ");
    fgets(data -> input, MAX_LENGTH, stdin);

    compile(data);

    print_tokens(data);

    printf("%s\n", data -> mkstr); 
    printf("%f\n", evaluate(data, 2, 0, data -> token_cnt + 1));
} 

// (x + 32)x(23 - xsinxcosx^x+32) /cosx-23+2x^x  
// need to evaluate the tokenized mkstr according to PEMDAS    