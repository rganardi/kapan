#include <stdio.h>
#include <time.h>
#include <getopt.h>
#include <stdlib.h>
#include <assert.h>

#define PROGRAM_NAME "kapan"

/*
 * This is a homemade calendar program
 *
 * */

static char const short_options[] = "d:f:r:a:leh";

static struct option const long_options[] = 
{
	{"date",	required_argument,	NULL, 'd'},
	{"file",	required_argument, 	NULL, 'f'},
	{"remove",	required_argument, 	NULL, 'r'},
	{"add",		required_argument, 	NULL, 'a'},
	{"list",	no_argument,		NULL, 'l'},
	{"edit",	no_argument, 		NULL, 'e'},
	{"help",	no_argument,		NULL, 'h'},
};

void die (int status, int errno)
{
	switch (errno)
	{
		case 0:
			fprintf(stdout, "goodbye!\n");
			exit(status);
		case 1:
			fprintf(stdout, "events are printed\n");
			exit(status);
		case 2:
			fprintf(stdout, "editor is launched\n");
			exit(status);
		case 3:
			fprintf(stdout, "event is removed\n");
			exit(status);
		default:
			fprintf(stderr, "uncaught error, exiting\n");
			exit(status);
	}
}

void usage (int status, int errno)
{
	if (status != EXIT_SUCCESS) {
		/* go and die */
		die (status, errno);
	}
	else
	{
		fprintf(stdout,"\
Usage: %s [options]...\n", PROGRAM_NAME);
		fprintf(stdout,"\
-d, --date STRING	Display what's happening on that date\n\
-f, --file FILE		Use FILE as database\n\
-r, --remove ID		Remove event ID\n\
-a, --add STRING	Add event\n\
-l, --list		List all events\n\
-e, --edit		Edit the database in your favorite editor\n\
-h, --help		Display this help message\n");
		fprintf(stdout,"\
Event STRING should of of the format \"STARTTIME[:ENDTIME]:DESCRIPTION\"\n");
	}
	die (status, 0);
}

void printcal ()
{
	die(EXIT_SUCCESS, 1);
}

void launcheditor ()
{
	die(EXIT_SUCCESS, 2);
}

void removeevent ()
{
	die(EXIT_SUCCESS, 3);
}


int main (int argc, char **argv)
{
	char 	*date = NULL;
	char	*configfile = "~/.config/kapan/events";
	char	*event = NULL;
	char	*rmid = NULL;
	int	next_option = -1;

	fprintf(stdout, "read %i args\n", argc);
	if (argc < 2) {
		usage(EXIT_SUCCESS, 0);
	}

	do {
		next_option = getopt_long(argc, argv, short_options,\
				long_options, NULL);

		switch(next_option)
		{
			case 'd':
				date = optarg;
				printcal();
				break;
				
			case 'f':
				configfile = optarg;
				break;
				
			case 'r':
				rmid = optarg;
				removeevent();
				break;

			case 'l':
				printcal();
				break;
				
			case 'e':
				launcheditor();
				break;

			case 'a':
				event = optarg;
				break;

			case 'h':
				usage(EXIT_SUCCESS, 0);

			case '?':
				die(EXIT_FAILURE, 3);
		}
	}
	while (next_option != -1);
	
	fprintf(stdout, "please go here\n");
	fprintf(stdout, "configfile = %s\n", configfile);
	fprintf(stdout, "event = %s\n", event);
	fprintf(stdout, "rmid = %s\n", rmid);

	die(EXIT_SUCCESS, 0);
}
