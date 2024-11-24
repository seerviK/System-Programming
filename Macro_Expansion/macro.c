#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// Structure for Macro Name Table (MNT)
typedef struct
{
    char macroName[50];
    int mdtIndex;
    int numParams;
} MNTEntry;

// Structure for Macro Definition Table (MDT)
typedef struct
{
    int index;
    char definition[100];
} MDTEntry;

// Actual Parameter Table (APTAB) as an array
char APTAB[10][50];

// Global tables
MNTEntry MNT[10];
MDTEntry MDT[100];
int mntCount = 0;
int mdtCount = 0;

// Function to initialize MNT and MDT from the input macro definitions file
void initializeTables()
{
    FILE *macroDefFile = fopen("D:\\Collage_Codes\\System-Programming-Sem5\\Assignment5\\macro_definitions.txt", "r");
    if (macroDefFile == NULL)
    {
        printf("Error: Cannot open macro_definitions.txt\n");
        exit(1);
    }

    char line[100];
    int mdtIndex = 0;

    // Read macro definitions and populate MDT and MNT
    while (fgets(line, sizeof(line), macroDefFile))
    {
        if (strstr(line, "MACRO") != NULL)
        {
            // Process macro definition
            char macroName[50], param[10];
            sscanf(line, "MACRO %s %s", macroName, param);

            // Add entry to MNT
            MNT[mntCount].mdtIndex = mdtIndex;
            MNT[mntCount].numParams = param[0] ? 1 : 0; // assuming only one param
            strcpy(MNT[mntCount].macroName, macroName);
            mntCount++;

            // Add macro definition to MDT
            strcpy(MDT[mdtCount].definition, line);
            MDT[mdtCount].index = mdtCount;
            mdtCount++;

            // Read lines until MEND
            while (fgets(line, sizeof(line), macroDefFile) && strstr(line, "MEND") == NULL)
            {
                strcpy(MDT[mdtCount].definition, line);
                MDT[mdtCount].index = mdtCount;
                mdtCount++;
            }
            strcpy(MDT[mdtCount].definition, "MEND");
            MDT[mdtCount].index = mdtCount;
            mdtCount++;
        }
    }
    fclose(macroDefFile);
}

// Function to display MNT, MDT, and APTAB
void displayTables(FILE *outputFile)
{
    fprintf(outputFile, "\nMacro Name Table (MNT):\n");
    fprintf(outputFile, "Macro Name\tMDT Index\tNumber of Parameters\n");
    for (int i = 0; i < mntCount; i++)
    {
        fprintf(outputFile, "%s\t\t%d\t\t%d\n", MNT[i].macroName, MNT[i].mdtIndex, MNT[i].numParams);
    }

    fprintf(outputFile, "\nMacro Definition Table (MDT):\n");
    fprintf(outputFile, "Index\tDefinition\n");
    for (int i = 0; i < mdtCount; i++)
    {
        fprintf(outputFile, "%d\t%s", MDT[i].index, MDT[i].definition);
    }
}

// Function to expand a macro call
void expandMacro(const char *macroName, const char *actualParam, FILE *outputFile)
{
    // Find the macro in MNT
    int found = 0;
    int mntIndex;
    for (int i = 0; i < mntCount; i++)
    {
        if (strcmp(MNT[i].macroName, macroName) == 0)
        {
            found = 1;
            mntIndex = i;
            break;
        }
    }

    if (!found)
    {
        fprintf(outputFile, "Error: Macro %s not found!\n", macroName);
        return;
    }

    // Load actual parameter into APTAB
    strcpy(APTAB[0], actualParam);

    // Expand the macro using MDT
    fprintf(outputFile, "\nExpanded Code for %s:\n", macroName);
    for (int i = MNT[mntIndex].mdtIndex + 1; i < mdtCount; i++)
    {
        if (strstr(MDT[i].definition, "MEND") != NULL)
            break;

        // Replace formal parameters with actual parameters
        char expandedLine[100];
        strcpy(expandedLine, MDT[i].definition);
        char *paramPosition = strstr(expandedLine, "&ARG1");
        if (paramPosition != NULL)
        {
            strncpy(paramPosition, APTAB[0], strlen(APTAB[0]));
            paramPosition[strlen(APTAB[0])] = '\0';
        }
        fprintf(outputFile, "%s", expandedLine);
    }

    // Display APTAB for the current macro call
    fprintf(outputFile, "\nActual Parameter Table (APTAB):\n");
    fprintf(outputFile, "Parameter\tValue\n");
    fprintf(outputFile, "&ARG1\t\t%s\n", APTAB[0]);
}

// Function to process macro calls from the input file
void processMacroCalls()
{
    FILE *macroCallsFile = fopen("D:\\Collage_Codes\\System-Programming-Sem5\\Assignment5\\marco_calls.txt", "r");
    FILE *outputFile = fopen("D:\\Collage_Codes\\System-Programming-Sem5\\Assignment5\\output.txt", "w");
    if (macroCallsFile == NULL || outputFile == NULL)
    {
        printf("Error: Cannot open input/output files.\n");
        exit(1);
    }

    char line[100];
    // Read each line containing a macro call
    while (fgets(line, sizeof(line), macroCallsFile))
    {
        char macroName[50], actualParam[50];
        sscanf(line, "%s %s", macroName, actualParam);

        // Expand the macro
        expandMacro(macroName, actualParam, outputFile);
    }

    // Display tables in the output file
    displayTables(outputFile);

    fclose(macroCallsFile);
    fclose(outputFile);
}

int main()
{
    // Initialize tables with data from the macro definitions file
    initializeTables();

    // Process and expand macro calls from the macro calls file
    processMacroCalls();

    printf("Macro expansion completed. Check output.txt for the result.\n");
    return 0;
}
