#include "netchess_interface.h"

const char* commands[] = {"q", "help", "board", "move", "turn", 0};

const char* team_names[] = {"White(+)", "Black(-)"};

void nci_help()
{
        printf( "NetChess follows standard chess rules."
                "\nThe game starts with the white team making the first move."

                "\n\nCOMMAND            EFFECT"
                "\nq            Quit"

                "\n\nhelp       \tPrint documentation"

                "\n\nboard      \tPrint the game board"

                "\n\nmove       \tTakes two chess coordinates as arguments,"
                "\n             and moves a piece if legal"

                "\n\nturn       \tEnds the current player's turn, and"
                "\n             finalizes their moves"

                "\n\n" );
}

void nci_shell(board_t board)
{
        int i, run = 1;
        char* tokens[MAX_TOKENS];
        char  str[MAX_INPUT_LENGTH];

        uint8_t team = 0, turn_change = 0;

        board_t temp_board;
        memcpy(temp_board, board, sizeof(piece_t) * (8 * 8));

        for(i = 0; i < MAX_TOKENS; i++) tokens[i] = malloc(TOKEN_LENGTH + 1);

        printf("Game starts with the %s team.\n", team_names[team]);

        while(run)
        {
                if(turn_change)
                {
                        printf("%s team moves\n: ", team_names[team]);
                        turn_change = 0;
                }
                         else printf(": ");

                for(i = 0; i < MAX_TOKENS; i++) memset(tokens[i], 0, TOKEN_LENGTH);

                fgets(str, MAX_INPUT_LENGTH, stdin);
                tokenize(str, (char**)tokens);

                switch(cmd_to_idx(change_case(0, tokens[0])))
                {
                        case -0x1:
                                printf("\n\"%s\" is not a valid command. Type \"help\" for instructions.\n", tokens[0]);
                        break;
                        case  0x0:
                                run = 0;
                        break;
                        case  0x1:
                                nci_help();
                        break;
                        case  0x2:
                                nce_print(temp_board);
                        break;
                        case  0x3:
                                memcpy(temp_board, board, sizeof(piece_t) * (8 * 8));
                                nce_move(temp_board, team, tokens[1], tokens[2]);
                        break;
                        case  0x4:
                                memcpy(board, temp_board, sizeof(piece_t) * (8 * 8));
                                team ^= 1, turn_change = 1;
                        break;
                };
        }

        for(i = 0; i < MAX_TOKENS; i++) free(tokens[i]);
}

void tokenize(char* str, char** tokens)
{
        int i = 0;
        char* token = strtok(str, "     \n");

        for(i = 0; token && (i < MAX_TOKENS); i++)
        {
                strcpy(tokens[i], token);
                token = strtok(NULL, "  \n");
        }
}

// Convert a text command to a corresponding instruction index
int cmd_to_idx(char* cmd)
{
        int i;
        for(i = 0; commands[i]; i++)
                if(strcmp(cmd, commands[i]) == 0) return i;

        return -1;
}
