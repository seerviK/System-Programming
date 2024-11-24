#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_SYMBOLS 100
#define MAX_CODE_LINES 100
#define MAX_LITERALS 100
#define MAX_LINE_LENGTH 100

typedef struct
{
    char *name;
    char *type;
    int opcode;
} MOTEntry;

typedef struct
{
    char *name;
    char *name1;
    int code;
} Register;

typedef struct
{
    char *name;
    int address;
} Symbol;

typedef struct
{
    char *name;
    int address;
} Literal;

typedef struct
{
    char *operation;
    int code;
    int reg;
    char *operandType;
    int operandValue;
    int lc;
} IntermediateCode;

MOTEntry mot[] = {
    {"ADD", "IS", 1},
    {"SUB", "IS", 2},
    {"MUL", "IS", 3},
    {"MOVER", "IS", 4},
    {"MOV", "IS", 5},
    {"MOVEM", "IS", 6},
    {"BC", "IS", 7},
    {"DIV", "IS", 8},
    {"START", "AD", 1},
    {"END", "AD", 2},
    {"ORIGIN", "AD", 3},
    {"EQU", "AD", 4},
    {"LTORG", "AD", 5},
    {"DS", "DL", 1},
    {"DC", "DL", 2}};

Register registers[] = {
    {"AREG", "R", 1},
    {"BREG", "R", 2},
    {"CREG", "R", 3},
    {"DREG", "R", 4}};

Symbol symbol_table[MAX_SYMBOLS];
int symbol_count = 0;

Literal literal_table[MAX_LITERALS];
int literal_count = 0;

IntermediateCode intermediate_code[MAX_CODE_LINES];
int code_line_count = 0;

int location_counter = 0;

void process_line(char *line)
{
    char *label = NULL, *opcode = NULL, *operand1 = NULL, *operand2 = NULL;
    char *token = strtok(line, " ,\t");

    if (token == NULL)
        return;

    // Check if first token is a label
    if (token[strlen(token) - 1] == ':')
    {
        label = token;
        label[strlen(label) - 1] = '\0'; // Remove ':'
        token = strtok(NULL, " ,\t");
    }

    if (token != NULL)
        opcode = token;

    token = strtok(NULL, " ,\t");
    if (token != NULL)
        operand1 = token;

    token = strtok(NULL, " ,\t");
    if (token != NULL)
        operand2 = token;

    // Process label if exists
    if (label != NULL)
    {
        symbol_table[symbol_count].name = strdup(label);
        symbol_table[symbol_count].address = location_counter;
        symbol_count++;
    }

    // Process opcode and operands
    if (opcode != NULL)
    {
        int found = 0;
        for (int i = 0; i < sizeof(mot) / sizeof(mot[0]); i++)
        {
            if (strcmp(opcode, mot[i].name) == 0)
            {
                found = 1;
                if (strcmp(mot[i].type, "AD") == 0)
                {
                    if (strcmp(opcode, "START") == 0)
                    {
                        location_counter = atoi(operand1);
                        intermediate_code[code_line_count].operation = "AD";
                        intermediate_code[code_line_count].code = mot[i].opcode;
                        intermediate_code[code_line_count].operandType = "C";
                        intermediate_code[code_line_count].operandValue = location_counter;
                        intermediate_code[code_line_count].lc = location_counter;
                        code_line_count++;
                    }
                    else if (strcmp(opcode, "END") == 0)
                    {
                        intermediate_code[code_line_count].operation = "AD";
                        intermediate_code[code_line_count].code = mot[i].opcode;
                        intermediate_code[code_line_count].lc = location_counter;
                        code_line_count++;
                    }
                }
                else if (strcmp(mot[i].type, "IS") == 0)
                {
                    int reg_code = -1;
                    if (operand1 != NULL)
                    {
                        for (int j = 0; j < sizeof(registers) / sizeof(registers[0]); j++)
                        {
                            if (strcmp(operand1, registers[j].name) == 0)
                            {
                                reg_code = registers[j].code;
                                break;
                            }
                        }
                        if (reg_code == -1)
                        {
                            printf("Error: Invalid register %s\n", operand1);
                            return;
                        }
                    }
                    intermediate_code[code_line_count].operation = "IS";
                    intermediate_code[code_line_count].code = mot[i].opcode;
                    intermediate_code[code_line_count].reg = reg_code;
                    if (operand2 != NULL && operand2[0] == '=')
                    {
                        literal_table[literal_count].name = strdup(operand2);
                        literal_table[literal_count].address = -1; // To be filled in Pass-2
                        literal_count++;
                        intermediate_code[code_line_count].operandType = "L";
                        intermediate_code[code_line_count].operandValue = literal_count - 1;
                    }
                    else if (operand2 != NULL)
                    {
                        int sym_index = -1;
                        for (int j = 0; j < symbol_count; j++)
                        {
                            if (strcmp(operand2, symbol_table[j].name) == 0)
                            {
                                sym_index = j;
                                break;
                            }
                        }
                        if (sym_index == -1)
                        {
                            symbol_table[symbol_count].name = strdup(operand2);
                            symbol_table[symbol_count].address = location_counter; // Address to be filled in Pass-2
                            sym_index = symbol_count;
                            symbol_count++;
                            location_counter++;
                        }
                        intermediate_code[code_line_count].operandType = "S";
                        intermediate_code[code_line_count].operandValue = sym_index;
                    }
                    intermediate_code[code_line_count].lc = location_counter;
                    code_line_count++;
                    location_counter++;
                }
                else if (strcmp(mot[i].type, "DL") == 0)
                {
                    if (strcmp(opcode, "DS") == 0)
                    {
                        int size = atoi(operand1);
                        symbol_table[symbol_count - 1].address = location_counter;
                        intermediate_code[code_line_count].operation = "DL";
                        intermediate_code[code_line_count].code = mot[i].opcode;
                        intermediate_code[code_line_count].operandType = "C";
                        intermediate_code[code_line_count].operandValue = size;
                        intermediate_code[code_line_count].lc = location_counter;
                        code_line_count++;
                        location_counter += size;
                    }
                    else if (strcmp(opcode, "DC") == 0)
                    {
                        int value = atoi(operand1);
                        symbol_table[symbol_count - 1].address = location_counter;
                        intermediate_code[code_line_count].operation = "DL";
                        intermediate_code[code_line_count].code = mot[i].opcode;
                        intermediate_code[code_line_count].operandType = "C";
                        intermediate_code[code_line_count].operandValue = value;
                        intermediate_code[code_line_count].lc = location_counter;
                        code_line_count++;
                        location_counter++;
                    }
                }
                break;
            }
        }
        if (!found)
        {
            printf("Error: Invalid instruction %s\n", opcode);
        }
    }
}

void read_file_and_process_lines(const char *filename)
{
    FILE *file = fopen(filename, "r");
    if (!file)
    {
        printf("Error: Unable to open file %s\n", filename);
        return;
    }

    char line[MAX_LINE_LENGTH];
    while (fgets(line, sizeof(line), file))
    {
        line[strcspn(line, "\n")] = '\0';
        process_line(line);
    }

    fclose(file);
}

void fill_literal_addresses()
{
    for (int i = 0; i < literal_count; i++)
    {
        if (literal_table[i].address == -1)
        {
            literal_table[i].address = location_counter;
            location_counter++;
        }
    }
}

void write_output_to_file(const char *filename)
{
    FILE *file = fopen(filename, "w");
    if (!file)
    {
        printf("Error: Unable to open file %s\n", filename);
        return;
    }

    fprintf(file, "Symbol Table:\n");
    for (int i = 0; i < symbol_count; i++)
    {
        fprintf(file, "%d    %s    %d\n", i, symbol_table[i].name, symbol_table[i].address);
    }

    fprintf(file, "\nIntermediate Code with LC:\n");
    fprintf(file, "LC   \n");
    for (int i = 0; i < code_line_count; i++)
    {
        fprintf(file, "%d   (%s, %d)   ", intermediate_code[i].lc, intermediate_code[i].operation, intermediate_code[i].code);
        if (intermediate_code[i].reg != -1)
        {
            for (int j = 0; j < sizeof(registers) / sizeof(registers[0]); j++)
            {
                if (registers[j].code == intermediate_code[i].reg)
                {
                    fprintf(file, "(%s, %d)   ", registers[j].name1, registers[j].code);
                    break;
                }
            }
        }
        if (intermediate_code[i].operandType != NULL)
        {
            fprintf(file, "(%s, %d)", intermediate_code[i].operandType, intermediate_code[i].operandValue);
        }
        fprintf(file, "\n");
    }

    fclose(file);
}

int main()
{

    printf("Output generated in output_pass1.txt\n");
    const char *input_filename = "input_pass1.txt";
    const char *output_filename = "output_pass1.txt"; // Specify the output file path here

    read_file_and_process_lines(input_filename);
    fill_literal_addresses();
    write_output_to_file(output_filename);

    return 0;
}
