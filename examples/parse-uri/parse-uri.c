/*

str_t demo to extract scheme (http/ftp/etc..), host, user, path and port from the following URI formats:
The function which does this is less than 50 lines, requires no buffers, and does not modify the given uri.

scheme:
scheme:path
scheme:path/path
scheme://host
scheme://host/path
scheme://host/path/path
scheme://user@host
scheme://user@host/path
scheme://user@host/path/path
scheme://host:<port>
scheme://host:<port>/path
scheme://host:<port>/path/path
scheme://user@host:<port>
scheme://user@host:<port>/path
scheme://user@host:<port>/path/path
host
host/path
host/path/path
user@host
user@host/path
user@host/path/path
host:<port>
host:<port>/path
host:<port>/path/path
user@host:<port>
user@host:<port>/path
user@host:<port>/path/path

*/

	#include <stdio.h>
	#include <limits.h>
	#include <stdint.h>
	#include <string.h>
	#include <ctype.h>

	#include "../../str.h"

//********************************************************************************************************
// Configurable defines
//********************************************************************************************************

//********************************************************************************************************
// Local defines
//********************************************************************************************************

	struct uri_struct
	{
		str_t scheme;
		str_t host;
		str_t path;
		str_t user;
		str_t port;
	};

//********************************************************************************************************
// Public variables
//********************************************************************************************************

//********************************************************************************************************
// Private variables
//********************************************************************************************************
	#define NUMBER_OF_DEMO_STRINGS	27

	const char* demo_string[NUMBER_OF_DEMO_STRINGS] =
	{
		"http:",
		"http:home",
		"http:home/fred",
		"http://github.com",
		"http://github.com/path",
		"http://github.com/home/fred",
		"http://mickjc750@github.com",
		"http://mickjc750@github.com/path",
		"http://mickjc750@github.com/home/fred",
		"http://github.com:80",
		"http://github.com:80/path",
		"http://github.com:80/home/fred",
		"http://mickjc750@github.com:80",
		"http://mickjc750@github.com:80/path",
		"http://mickjc750@github.com:80/home/fred",
		"github.com",
		"github.com/path",
		"github.com/home/fred",
		"mickjc750@github.com",
		"mickjc750@github.com/path",
		"mickjc750@github.com/home/fred",
		"github.com:80",
		"github.com:80/path",
		"github.com:80/home/fred",
		"mickjc750@github.com:80",
		"mickjc750@github.com:80/path",
		"mickjc750@github.com:80/home/fred"
	};

//********************************************************************************************************
// Private prototypes
//********************************************************************************************************

	static struct uri_struct parse_uri(str_t uri);
	static void show_uri(struct uri_struct uri);

//********************************************************************************************************
// Public functions
//********************************************************************************************************

int main(int argc, const char* argv[])
{
	int i = 0;

	if(argc > 1)
		show_uri(parse_uri(cstr(argv[1])));
	else
	{
		while(i != NUMBER_OF_DEMO_STRINGS)
		{
			printf("[%s]\n", demo_string[i]);
			show_uri(parse_uri(cstr(demo_string[i])));
			i++;
		};
		printf("To run with a custom string use %s <string>\n", argv[0]);
	};
	return 0;
}

//********************************************************************************************************
// Private functions
//********************************************************************************************************

static struct uri_struct parse_uri(str_t uri)
{
	struct uri_struct result = {0};
	str_t tmp;

	if(str_is_valid(str_find_first(uri, cstr("://"))))
	{
		result.scheme = str_pop_first_split(&uri, cstr(":"));
		uri = str_sub(uri, 2, INT_MAX);
	};

	result.user = str_pop_first_split(&uri, cstr("@"));
	if(!str_is_valid(uri))
		str_swap(&result.user, &uri);

	if(!str_is_valid(str_find_first(uri, cstr(":"))))
	{
		tmp = str_pop_first_split(&uri, cstr("/"));
		if(str_is_valid(uri))
		{
			result.path = uri;
			result.host = tmp;
		}
		else
			result.host = tmp;
	}
	else
	{
		tmp = str_pop_first_split(&uri, cstr(":"));
		if(!str_is_valid(uri))
			str_swap(&tmp, &uri);
		if(uri.size && isdigit(uri.data[0]))
		{
			result.host = tmp;

			tmp = str_pop_first_split(&uri, cstr("/"));
			result.port = tmp;
			if(str_is_valid(uri))
				result.path = uri;
		}
		else
		{
			result.scheme = tmp;
			if(uri.size)
				result.path = uri;
		};
	};
	return result;
}

static void show_uri(struct uri_struct uri)
{
	if(str_is_valid(uri.scheme))
		printf("scheme = \"%"PRIstr"\"\n", PRIstrarg(uri.scheme));
	if(str_is_valid(uri.host))
		printf("host   = \"%"PRIstr"\"\n", PRIstrarg(uri.host));
	if(str_is_valid(uri.path))
		printf("path   = \"%"PRIstr"\"\n", PRIstrarg(uri.path));
	if(str_is_valid(uri.user))
		printf("user   = \"%"PRIstr"\"\n", PRIstrarg(uri.user));
	if(str_is_valid(uri.port))
		printf("port   = \"%"PRIstr"\"\n", PRIstrarg(uri.port));
	printf("\n");
}
