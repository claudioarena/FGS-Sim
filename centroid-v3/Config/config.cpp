// In config.cpp
#include <string>
#include <stdlib.h>

#define TEXTSIZE 64

namespace config
{
int some_config_int = 123;
std::string some_config_string = "foo";

bool config::load_config_file(char *fileName)
{
    /*FILE *pFile = fopen(fileName, "r");
    char buffer2[TEXTSIZE][TEXTSIZE];
    char *delimiters = " =,\n;";

    if (!pFile)
    {
        return 1;
    }

    size_t linesRead = 0;

    while (fgets(buffer2[linesRead], TEXTSIZE, pFile) != NULL)
    {
        if (buffer2[linesRead][0] == '\n' || buffer2[linesRead][0] == '#')
        {
            continue;
        }
        char *variable;
        variable = strtok(buffer2[linesRead], delimiters);
        if (!variable)
        {
            continue;
        }
        if (strcmp(variable, "server") == 0)
        {
            variable = strtok(NULL, delimiters);
            if (!variable)
            {
                printf("Error in configuration file: no value for server!\n");
                {
                    system("PAUSE");
                    return 2;
                }
            }

            settingsServer = (char *)malloc((strlen(variable) + 1) * sizeof(char));
            memcpy(settingsServer, variable, strlen(variable));
            settingsServer[strlen(variable)] = '\0';
            printf("Server is %s\n", variable);
        }
        else if (strcmp(variable, "network") == 0)
        {
            variable = strtok(NULL, delimiters);
            if (!variable)
            {
                printf("Error in configuration file: no value for network!\n");
                {
                    system("PAUSE");
                    return 2;
                }
            }
            settingsNetwork = atoi(variable);
            printf("Network is %s\n", variable);
        }
        else
        {
            continue;
        }
        linesRead++;
    }

    fclose(pFile);
    return 0;*/
}

} // namespace config