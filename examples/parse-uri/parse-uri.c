/*

strview_t demo to extract scheme (http/ftp/etc..), host, user, path and port from the following URI formats:

The function which does this:
	* Is less than 50 lines.
	* Requires no buffers.
	* Doesn't modify the given uri.
	* Doesn't even copy any data from the given uri. (we already HAVE the data, we don't need to copy it).

Possible combinations of URI components:

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

	#include "strview.h"

//********************************************************************************************************
// Configurable defines
//********************************************************************************************************

//********************************************************************************************************
// Local defines
//********************************************************************************************************

	struct uri_struct
	{
		strview_t scheme;
		strview_t host;
		strview_t path;
		strview_t user;
		strview_t port;
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

	static struct uri_struct parse_uri(strview_t uri);
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

static struct uri_struct parse_uri(strview_t uri)
{
	struct uri_struct result = {0};
	strview_t tmp;

	if(strview_is_valid(strview_find_first(uri, cstr("://"))))
	{
		result.scheme = strview_split_first_delim(&uri, cstr(":"));
		uri = strview_sub(uri, 2, INT_MAX);
	};

	result.user = strview_split_first_delim(&uri, cstr("@"));
	if(!strview_is_valid(uri))
		strview_swap(&result.user, &uri);

	if(!strview_is_valid(strview_find_first(uri, cstr(":"))))
	{
		tmp = strview_split_first_delim(&uri, cstr("/"));
		if(strview_is_valid(uri))
		{
			result.path = uri;
			result.host = tmp;
		}
		else
			result.host = tmp;
	}
	else
	{
		tmp = strview_split_first_delim(&uri, cstr(":"));
		if(!strview_is_valid(uri))
			strview_swap(&tmp, &uri);
		if(uri.size && isdigit(uri.data[0]))
		{
			result.host = tmp;

			tmp = strview_split_first_delim(&uri, cstr("/"));
			result.port = tmp;
			if(strview_is_valid(uri))
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
	if(strview_is_valid(uri.scheme))
		printf("scheme = \"%"PRIstr"\"\n", PRIstrarg(uri.scheme));
	if(strview_is_valid(uri.host))
		printf("host   = \"%"PRIstr"\"\n", PRIstrarg(uri.host));
	if(strview_is_valid(uri.path))
		printf("path   = \"%"PRIstr"\"\n", PRIstrarg(uri.path));
	if(strview_is_valid(uri.user))
		printf("user   = \"%"PRIstr"\"\n", PRIstrarg(uri.user));
	if(strview_is_valid(uri.port))
		printf("port   = \"%"PRIstr"\"\n", PRIstrarg(uri.port));
	printf("\n");
}
