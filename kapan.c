#define _GNU_SOURCE "500"
#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <getopt.h>
#include <assert.h>
#include <string.h>

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
static char const *datemsk = "/home/rganardi/.config/kapan/datemsk";
char	*database = "/home/rganardi/.config/kapan/events";
const char *format = "%F %H:%M";

void die (int status, int errno)
{
	/*
	 * errno 1-7 follows getdate
	 * still have to thing about the others.
	 * */
	switch (errno)
	{
		case 0:
			fprintf(stdout, "goodbye!\n");
			exit(status);

		case 1:
			fprintf(stderr, "getdate: The DATEMSK environment\
					variable is not defined, or its value\
					is an empty string.\n");
			exit(status);

		case 2:
			fprintf(stderr, "getdate: The template file specified\
				       	by DATEMSK cannot be opened for reading.\n");
			exit(status);

		case 3:
			fprintf(stderr, "getdate: Failed to get file status\
				       	information.\n");
			exit(status);

		case 4:
		     	fprintf(stderr, "getdate: The template file is not a\
				       	regular file.\n");
			exit(status);

		case 5:
			fprintf(stderr, "getdate: An error was encountered while\
				       	reading the template file.\n");
			exit(status);

		case 6:
			fprintf(stderr, "getdate: Memory allocation failed (not\
					enough memory available).\n");
			exit(status);

		case 7:
			fprintf(stderr, "getdate: There is no line in the file\
					that matches the input.\n");
			exit(status);

		case 8:
			fprintf(stderr, "getdate: Invalid input specification.\n");
			exit(status);

		case 127:
			fprintf(stderr, "too many argument\n");
			exit(status);

		default:
			fprintf(stderr, "uncaught error, exiting\n");
			exit(status);

	}
}

void usage (int status, int errno)
{
	fprintf(stdout,"\
Usage: %s [options]...\n", PROGRAM_NAME);
		fprintf(stdout,"\
-d, --date STRING	Display what's happening on the period STRING\n\
-f, --file FILE		Use FILE as database\n\
-r, --remove ID		Remove event ID\n\
-a, --add STRING	Add event STRING\n\
-l, --list		List all events\n\
-e, --edit		Edit the database in your favorite editor\n\
-h, --help		Display this help message\n");
		fprintf(stdout,"\
Event STRING should be of the format \"STARTTIME[>ENDTIME]>DESCRIPTION\"\n\
Period STRING should be of the format \"[STARTTIME>]ENDTIME\". If no STARTTIME is provided, current time is assumed.\n");

	die (status, 0);
}

void printcal (char *starttime, char *endtime, char *database, int status, int errno)
{
	/* 
	 * This should take in a range of dates and print events in this period.
	 * input. Exits.
	 * 	time_t?	date
	 * 	int 	status
	 * 	int 	errno
	 * */
	struct	tm *start;
	struct	tm *end;
	time_t	now = time(NULL);

	if (starttime != NULL) {
		start = getdate(starttime);
	} else {
		start = gmtime(&now);
	}
	end = getdate(endtime);
	
	if (!getdate_err) {
		printf("start\n");
		printf("    tm_sec   = %d\n", start->tm_sec);
		printf("    tm_min   = %d\n", start->tm_min);
		printf("    tm_hour  = %d\n", start->tm_hour);
		printf("    tm_mday  = %d\n", start->tm_mday);
		printf("    tm_mon   = %d\n", start->tm_mon);
		printf("    tm_year  = %d\n", start->tm_year);
		printf("    tm_wday  = %d\n", start->tm_wday);
		printf("    tm_yday  = %d\n", start->tm_yday);
		printf("    tm_isdst = %d\n", start->tm_isdst);

		printf("end\n");
		printf("    tm_sec   = %d\n", end->tm_sec);
		printf("    tm_min   = %d\n", end->tm_min);
		printf("    tm_hour  = %d\n", end->tm_hour);
		printf("    tm_mday  = %d\n", end->tm_mday);
		printf("    tm_mon   = %d\n", end->tm_mon);
		printf("    tm_year  = %d\n", end->tm_year);
		printf("    tm_wday  = %d\n", end->tm_wday);
		printf("    tm_yday  = %d\n", end->tm_yday);
		printf("    tm_isdst = %d\n", end->tm_isdst);
	} else {
		fprintf(stderr, "something's wrong\n");
		die(EXIT_FAILURE, 1);
	}


	die(status, errno);
}

void launcheditor (char *database, int status, int errno)
{
	/*
	 * This should just be a wrapper to launch our favorite editor.
	 * Use some systemcall.
	 * */
	char *editor = getenv("EDITOR");

	assert (database != NULL);

	fprintf(stdout, "current favorite editor is %s\n", editor);
	fprintf(stdout, "using database %s\n", database);
	die(status, errno);
}

void removeevent (char *database, int status, int errno)
{
	/*
	 * This function should take in an event id and remove
	 * the event from the database. Exits.
	 * input
	 * 	int	id
	 * 	int	status
	 * 	int	errno
	 * */


	assert (database != NULL);

	die(status, errno);
}

void addevent (char *event, char *database, int status, int errno)
{
	/*
	 * Add event specified in *event into the file *database
	 * */

	char		*starttime = NULL;
	char 		*endtime = NULL;
	char 		*desc = NULL;
	char 		*saveptr = NULL;
	FILE 		*fd;
	struct tm	*start;
	struct tm	*end;
	int		maxsize = 256;
	char		*buffer;

	starttime = strtok_r(event, ">", &saveptr);

	if (strchr(saveptr, '>') == NULL) {	/* check if endtime is specified */
		desc = saveptr;
	} else {
		endtime = strtok_r(NULL, ">", &saveptr);
		desc = strtok_r(NULL, ">", &saveptr);
	}

	start = getdate(starttime);		/* result is valid only
						   until next getdate call
						   */
	if (getdate_err) {
		die(EXIT_FAILURE, getdate_err);
	}

	fd = fopen(database, "a+");

	buffer = (char *) malloc(maxsize);
	strftime(buffer, maxsize, format, start);
	fprintf(fd, "%s\t", buffer);
	free(buffer);

	if (endtime != NULL) {
		end = getdate(endtime);
		if (getdate_err) {
			die(EXIT_FAILURE, getdate_err);
		}
		buffer = (char *) malloc(maxsize);
		strftime(buffer, maxsize, format, end);
		fprintf(fd, "%s\t", buffer);
		free(buffer);
	}
	
	fprintf(fd, "%s\n", desc);
	fclose(fd);

	die(status, errno);
}

int main (int argc, char **argv)
{
	char 	*date = NULL;
	char	*event = NULL;
	char	*rmid = NULL;
	int	next_option = -1;
	int	edit_flag = 0;
	int	remove_flag = 0;
	int	print_flag = 0;
	int	add_flag = 0;
	char	*starttime = NULL;
	char 	*endtime = NULL;
	char 	*saveptr = NULL;

	setenv("DATEMSK", datemsk, 1);	/* ensure we're using the right
					   format file for getdate()
					   */


	fprintf(stdout, "read %i args\n", argc);
	if (argc < 2) {
		usage(EXIT_SUCCESS, 0);
	}
	int i = 0;
	for (i = 0; i < argc; i++) {
		fprintf(stdout, "argument %i is %s\n", i, argv[i]);
	}

	do {
		next_option = getopt_long(argc, argv, short_options,\
				long_options, NULL);

		switch(next_option)
		{
			case 'd':
				date = optarg;
				print_flag += 1;
				break;
				
			case 'f':
				database = optarg;
				break;
				
			case 'r':
				rmid = optarg;
				remove_flag += 1;
				break;

			case 'l':
				print_flag += 1;
				break;
				
			case 'e':
				edit_flag += 1;
				break;

			case 'a':
				event = optarg;
				add_flag += 1;
				break;

			case 'h':
				usage(EXIT_SUCCESS, 0);
				break;

			case '?':
				die(EXIT_FAILURE, 3);
				break;
		}
	}
	while (next_option != -1);

	if (optind != argc) {
		die(EXIT_FAILURE, 127);
	}

	if (edit_flag) {
		launcheditor(database, EXIT_SUCCESS, 2);
	}

	if (add_flag) {
		addevent(event, database, EXIT_SUCCESS, 0);
	}

	if (remove_flag) {
		removeevent(database, EXIT_SUCCESS, 3);
	}

	if (print_flag) {

		if (print_flag > 1) {		/* -l and -d are set */
			fprintf(stderr, "make up your mind, do you want -l or -d?\n");
			die(EXIT_FAILURE, 127);
		}

		if (strchr(date, '>') != NULL) {	/* check if starttime is specified */
			starttime = strtok_r(date, (const char *) ">", &saveptr);
			endtime = strtok_r(NULL, (const char *) ">", &saveptr);
		} else {
			endtime = date;
		}

		printcal(starttime, endtime, database, EXIT_SUCCESS, 1);
	}

	die(EXIT_SUCCESS, 0);	/* should not be reachable */
}
