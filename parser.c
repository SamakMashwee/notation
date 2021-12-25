#include "parser.h"

int main(void) {
    p_data *data = malloc(sizeof(p_data));

    data -> input = (char *) malloc(sizeof(char) * MAX_LENGTH);
    printf("function: ");
    fgets(data -> input, MAX_LENGTH, stdin);

    compile(data);

    print_tokens(data);
}