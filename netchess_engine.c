#include "netchess_engine.h"

#include <math.h>
#include <string.h>

const char* piece_names[] = {NULL, "pawn", "rook", "knight", "bishop", "queen", "king"};

void nce_init(board_t b)
{
	memset(b, 0, sizeof(piece_t) * (8 * 8));

	/* Initialize the team of each place to -1 so that we don't have empty spaces   *
	 * parading around like they're part of the white team 				*/

	int i, j;
	for(i = 0; i < 8; i++)
		for(j = 0; j < 8; j++)
			b[i][j].team = -1;

	for(i = 0; i < 2; i++)
	{
		b[i * 7][0].type = PIECE_ROOK;
		b[i * 7][0].team = (i == 0) ? TEAM_WHITE : TEAM_BLACK;

		b[i * 7][1].type = PIECE_KNIGHT;
		b[i * 7][1].team = (i == 0) ? TEAM_WHITE : TEAM_BLACK;

		b[i * 7][2].type = PIECE_BISHOP;
		b[i * 7][2].team = (i == 0) ? TEAM_WHITE : TEAM_BLACK;

		b[i * 7][3 + i].type = PIECE_QUEEN;
		b[i * 7][3 + i].team = (i == 0) ? TEAM_WHITE : TEAM_BLACK;

		b[i * 7][4 - i].type = PIECE_KING;
		b[i * 7][4 - i].team = (i == 0) ? TEAM_WHITE : TEAM_BLACK;

		b[i * 7][5].type = PIECE_BISHOP;
		b[i * 7][5].team = (i == 0) ? TEAM_WHITE : TEAM_BLACK;

		b[i * 7][6].type = PIECE_KNIGHT;
		b[i * 7][6].team = (i == 0) ? TEAM_WHITE : TEAM_BLACK;

		b[i * 7][7].type = PIECE_ROOK;
		b[i * 7][7].team = (i == 0) ? TEAM_WHITE : TEAM_BLACK;
	}

	for(i = 0; i < 8; i++)
	{
		for(j = 0; j < 2; j++)
		{
			b[j ? 6 : 1][i].type = PIECE_PAWN;
			b[j ? 6 : 1][i].team = j;
		}
	}
}

void nce_print(board_t b)
{
	printf("\n     A    B    C    D    E    F    G    H\n\n");

	int i;
	for(i = 0; i < 8; i++)
	{
		printf("%i  ", (8 - i));

		int j;
		for(j = 0; j < 8; j++)
		{
			char team_char = (b[j][i].team == TEAM_WHITE) ? '+' : '-';

			switch(b[j][i].type)
			{
				case PIECE_ROOK:   printf(" %cR  ", team_char); break;
				case PIECE_KNIGHT: printf(" %cKn ", team_char); break;
				case PIECE_BISHOP: printf(" %cB  ", team_char); break;
				case PIECE_QUEEN:  printf(" %cQ  ", team_char); break;
				case PIECE_KING:   printf(" %cK  ", team_char); break;
				case PIECE_PAWN:   printf(" %cP  ", team_char); break;
				case PIECE_EMPTY:  printf("  .  "); 		break;
			};
		}

		printf("\n\n");
	}
}

int nce_check(board_t b, int team)
{
	// Find the king
	coordinate_t king_coord;
	for(int x = 0; x < 8; x++)
		for(int y = 0; y < 8; y++)
			if(b[x][y].team == team && b[x][y].type == PIECE_KING)
				king_coord[0] = x, king_coord[1] = y;

	printf("%s king found at (%i, %i)\n", team == 0 ? "White" : "Black", king_coord[0], king_coord[1]);

	for(int i = 0; i < 8; i++)
	{
		coordinate_t direction = {0, 0};

		switch(i)
		{
			case 0: direction[0] =  0, direction[1] =  1;	break;
			case 1: direction[0] =  1, direction[1] =  1;	break;
			case 2: direction[0] =  1, direction[1] =  0;	break;
			case 3: direction[0] =  1, direction[1] = -1;	break;
			case 4: direction[0] =  0, direction[1] = -1;	break;
			case 5: direction[0] = -1, direction[1] = -1;	break;
			case 6: direction[0] = -1, direction[1] =  0;	break;
			case 7: direction[0] = -1, direction[1] =  1;	break;
		};

		coordinate_t it = {king_coord[0], king_coord[1]};
		while(it[0] < 8 && it[1] < 8)
		{
			piece_t* p = &b[it[0]][it[1]];

			// If the piece belongs to the enemy, check to see if that piece can attack the king
			if(p->team == ((team ^ 1) == 0))
			{
				switch(i % 1)
				{
					case 0: if(p->type == PIECE_ROOK || p->type == PIECE_QUEEN) return 1; break; // Horizontal
					case 1: if(p->type == PIECE_BISHOP) return 1; break; // Diagonal
				}

				break;
			}

			it[0] += direction[0], it[1] += direction[1];
		}
	}

	return 0;
}

int nce_mate(board_t b, int team)
{
	return 0;
}

/* Evaluate the status of the game, and determine whether a player is in check or checkmate 		*
 * Returns '0' if neither player is in check nor checkmate						*
 *	   '1' if white player is in check								*
 *	   '2' if white player is in checkmate								*
 *	   '3' if black player is in check								*
 *	   '4' if black player is in checkmate								*/
int nce_evaluate(board_t b)
{
	printf("Beginning evaluation.\n");

	for(int team = TEAM_WHITE; team <= TEAM_BLACK; team++)
	{
		if(nce_check(b, team))
		{
			printf("Player is in check!\n");
			if(nce_mate(b, team)) return ((team = 0 ? 0 : 2) + 2);
			return ((team == 0 ? 0 : 2) + 1);
		}
	}

	return 0;
}

int valid_move(board_t b, coordinate_t src, coordinate_t dest)
{
	for(int i = 0; i < 2; i++)
	{
		if(src[i]  < 0 || src[i]  > 8) return 0;
		if(dest[i] < 0 || dest[i] > 8) return 0;
	}

	uint8_t src_type = b[ src[0]][ src[1]].type,
		src_team = b[ src[0]][ src[1]].team;

	uint8_t dest_type = b[dest[0]][dest[1]].type,
		dest_team = b[dest[0]][dest[1]].team;

	// White moves in a positive direction (right), black moves negative (left)
	char direction = (src_team == TEAM_WHITE) ? 1 : -1;

	// This is no time for civil war
	if(dest_team == src_team) return 0;

	switch(src_type)
	{
		case PIECE_ROOK:
			if(clear_path(b, src, dest) == 1) return 1;
		break;

		case PIECE_KNIGHT:
			if(abs(dest[0] - src[0]) == 2 && abs(dest[1] - src[1]) == 1) return 1;
			if(abs(dest[0] - src[0]) == 1 && abs(dest[1] - src[1]) == 2) return 1;
		break;

		case PIECE_BISHOP:
			if(clear_path(b, src, dest) == 2) return 1;
		break;

		case PIECE_QUEEN:
			if(clear_path(b, src, dest) != -1) return 1;
		break;

		case PIECE_KING:
			if(clear_path(b, src, dest) != -1 && abs(dest[0] - src[0]) <= 1 && abs(dest[1] - src[1]) <= 1) return 1;
		break;

		case PIECE_PAWN:
			// Moving a pawn one space forward into an empty space
			if(dest_type == PIECE_EMPTY && dest[1] == src[1] && dest[0] - src[0] == direction)
				return 1;
			// Moving a pawn two spaces forward from the starting position
			if(dest_type == PIECE_EMPTY && src[0] == (src_team == TEAM_WHITE ? 1 : 6) && dest[1] == src[1] && dest[0] - src[0] == (2 * direction))
				return 1;
			// Capture a piece diagonally
			if(dest_type != PIECE_EMPTY && (dest[0] - src[0]) == direction && (dest[1] == src[1] + 1 || dest[1] == src[1] - 1))
				return 1;
		break;

		case PIECE_EMPTY: break;
	};

	printf("DEBUG: Move does not meet criteria for piece selected\n");
	return 0;
}

// Take the source and dest coordinates in chess notation, and move a piece if the move is legal
void nce_move(board_t b, int team, char* src_cn, char* dest_cn)
{
	coordinate_t src_coord, dest_coord;

	cn_to_coord(src_cn, src_coord);
	cn_to_coord(dest_cn, dest_coord);

	if(b[src_coord[0]][src_coord[1]].team == team && valid_move(b, src_coord, dest_coord))
	{
		b[dest_coord[0]][dest_coord[1]].type = b[src_coord[0]][src_coord[1]].type;
		b[dest_coord[0]][dest_coord[1]].team = b[src_coord[0]][src_coord[1]].team;

		b[src_coord[0]][src_coord[1]].type = 0;
		b[src_coord[0]][src_coord[1]].team = -1;
	}
	else
		printf("Invalid move.\n");
}

/* Check to see if pieces are on the same plane horizontally, vertically, or diagonally. 		*
 * If they are, check the path between the two pieces to see if it's obstructed by another piece. 	*
 * Returns '-1' if no path exists,									*
 *	    '0' if path is not clear 									*
 *	    '1' is path is clear horizontally/vertically,						*
 *	    '2' is path is clear diagonally,								*/
int clear_path(board_t b, coordinate_t src, coordinate_t dest)
{
	// X plane
	if(src[1] == dest[1] && src[0] != dest[0])
	{
		int it = (dest[0] - src[0] < 0) ? -1 : 1;
		for(int i = src[0] + it; i != dest[0]; i += it)
			if(b[i][src[1]].type != PIECE_EMPTY) return 0;

		return 1;
	}
	// Y plane
	if(src[0] == dest[0] && src[1] != dest[1])
	{
		int it = (dest[1] - src[1] < 0) ? -1 : 1;
		for(int i = src[1] + it; i != dest[1]; i += it)
			if(b[src[0]][i].type != PIECE_EMPTY) return 0;

		return 1;
	}
	// Diagonal plane
	if(abs(dest[0] - src[0]) == abs(dest[1] - src[1]))
	{
		coordinate_t it = {0, 0};
		it[0] = ((dest[0] - src[0]) < 0) ? -1 : 1;
		it[1] = ((dest[1] - src[1]) < 0) ? -1 : 1;

		coordinate_t i = {0, 0};
		for(i[0] = src[0] + it[0]; i[0] != dest[0]; i[0] += it[0])
			for(i[1] = src[1] + it[1]; i[1] != dest[1]; i[1] += it[1])
				if(b[i[0]][i[1]].type != PIECE_EMPTY) return 0;
		return 2;
	}

	return -1;
}

// Convert chess notation to internal coordinates
void cn_to_coord(char* cn, coordinate_t coord)
{
	change_case(1, cn);

	coord[0] = (cn[0] - 'A');
	coord[1] = 8 - (cn[1] - '0');
}

// Convert internal coordinates to chess notation
void coord_to_cn(coordinate_t coord, char* cn)
{
	memset(cn, 0, 3);

	cn[0] = coord[0] + 'A';
	cn[1] = coord[1] + '0';
}

char* change_case(int new_case, char* str)
{
	int i;
	for(i = 0; i < strlen(str); i++)
		if(str[i] >= (!new_case ? 'A' : 'a') && str[i] <= (!new_case ? 'Z' : 'z'))
			str[i] += (!new_case ? ('a' - 'A') : ('A' - 'a'));

	return str;
}

