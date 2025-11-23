/*
*/

	#include <stdlib.h>
	#include <stdio.h>
	#include <assert.h>
	#include <limits.h>
	#include <stdint.h>
	#include <math.h>

	#include "strbuf.h"
	#include "strview.h"
	#include "strnum.h"

//********************************************************************************************************
// Configurable defines
//********************************************************************************************************

//********************************************************************************************************
// Local defines
//********************************************************************************************************

	#define DBG(_fmtarg, ...) print("%s:%.4i - "_fmtarg"\n" , __FILE__, __LINE__ ,##__VA_ARGS__)

	typedef struct lexbracket_t
	{
		const char *bracket_pairs;
		int pair_count;
		int depth;
		char opening_char;
		char closing_char;
	} lexbracket_t;

//********************************************************************************************************
// Public variables
//********************************************************************************************************


//********************************************************************************************************
// Private variables
//********************************************************************************************************

//********************************************************************************************************
// Private prototypes
//********************************************************************************************************

	static void lexbracket_init(lexbracket_t *ctx, const char *bracket_pairs);
	static bool lexbracket_is_inside(lexbracket_t *ctx, const char c);

	static void test_number_conversion(strview_t view, int options);

//********************************************************************************************************
// Public functions
//********************************************************************************************************

int main(int argc, const char* argv[])
{
	(void)argc;
	(void)argv;
	char* txt=NULL;
	size_t txt_size = 0;
	char* brackets=NULL;
	size_t brackets_size = 0;
	lexbracket_t lb;
	char *ptr;

	while(true)
	{
		printf("Enter brackets string ie. {}()[]''\"\"\n");
		getline(&brackets, &brackets_size, stdin);
		brackets[strlen(brackets)-1] = 0;
		printf("Enter text\n");
		getline(&txt, &txt_size, stdin);
		txt[strlen(txt)-1] = 0;
		lexbracket_init(&lb, brackets);
		ptr = txt;
		while(*ptr)
			printf("%c", lexbracket_is_inside(&lb, *ptr++) ? 'I':'O');
		printf("\nI=inside quotes or brackets (including opening bracket/quote), O=outside quotes or brackets (not including closing bracket/quote)\n\n");
	};

	return 0;
}

//********************************************************************************************************
// Private functions
//********************************************************************************************************


static void lexbracket_init(lexbracket_t *ctx, const char *bracket_pairs)
{
	int i = 0;

	if(bracket_pairs)
		i = strlen(bracket_pairs);

	if(i & 1)
		i = 0;

	if(i)
	{
		ctx->pair_count = i/2;
		ctx->bracket_pairs = bracket_pairs;
	}
	else
	{
		ctx->pair_count = 0;
		ctx->bracket_pairs = NULL;
	};

	ctx->depth = 0;
	ctx->closing_char = 0;
	ctx->opening_char = 0;
}

static bool lexbracket_is_inside(lexbracket_t *ctx, const char c)
{
	int i = 0;

	if(ctx->depth && c == ctx->opening_char && c != ctx->closing_char)
		ctx->depth++;
	else if(ctx->depth && c == ctx->closing_char)
		ctx->depth--;
	else while(ctx->depth == 0 && i != ctx->pair_count)
	{
		if(ctx->bracket_pairs[i * 2] == c)
		{
			ctx->depth = 1;
			ctx->opening_char = c;
			ctx->closing_char = ctx->bracket_pairs[i * 2 + 1];
		};
		i++;
	};
	return (ctx->depth != 0);
}
