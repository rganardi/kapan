#define _GNU_SOURCE "500"
#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <getopt.h>
#include <assert.h>
#include <string.h>

#define PROGRAM_NAME "kapan"

/*
 * This is a homemade calendar program
 *
 * */

#define DATEMSK "/etc/datemsk"
#define DELIM "|"

#define EDIT_FLAG	(1 << 0)
#define REMOVE_FLAG	(1 << 1)
#define PRINT_FLAG	(1 << 2)
#define ADD_FLAG	(1 << 3)

#ifndef	KAPANDB
#define KAPANDB HOME"/.kapandb"
#endif

#define BUFFERSIZE  1024
#define ONBOLD "\033[1m"
#define OFFBOLD "\33[22m"
#define FORMAT "%FT%T%z"	/* use ISO-8601 format */

/* Define the version string based on the git repo tags, */
/* or manually, if no git repository is present */
#ifndef VERSION_STRING
#define VERSION_STRING "0.0.0"
#endif



static char const short_options[] = "d:f:r:a:leh";

static struct option const long_options[] =
{
	{"date",	required_argument,	NULL, 'd'},
	{"file",	required_argument,	NULL, 'f'},
	{"remove",	required_argument,	NULL, 'r'},
	{"add",		required_argument,	NULL, 'a'},
	{"list",	no_argument,		NULL, 'l'},
	{"edit",	no_argument,		NULL, 'e'},
	{"help",	no_argument,		NULL, 'h'},
};

char	*database = KAPANDB;

void die (int status, int errno)
{
	/*
	 * errno 1-7 follows getdate
	 * */
	switch (errno)
	{
		case 0:
			exit(status);

		case 1:
			fprintf(stderr, "\
getdate: The DATEMSK environment variable is not defined,\
or its value is an empty string.\n");
			exit(status);

		case 2:
			fprintf(stderr, "\
getdate: The template file specified by DATEMSK cannot be\
opened for reading.\n");
			exit(status);

		case 3:
			fprintf(stderr, "\
getdate: Failed to get file status information.\n");
			exit(status);

		case 4:
			fprintf(stderr, "\
getdate: The template file is not a regular file.\n");
			exit(status);

		case 5:
			fprintf(stderr, "\
getdate: An error was encountered while reading the template file.\n");
			exit(status);

		case 6:
			fprintf(stderr, "\
getdate: Memory allocation failed (not enough memory available).\n");
			exit(status);

		case 7:
			fprintf(stderr, "\
Date format is not recognized\n");
			exit(status);

		case 8:
			fprintf(stderr, "\
getdate: Invalid input specification.\n");
			exit(status);

		case 9:
			fprintf(stderr, "\
Database doesn't exist.\n");
			exit(status);

		case 10:
			fprintf(stderr, "\
Something is wrong with the database.\n");
			exit(status);

		case 11:
			fprintf(stderr, "\
Unable to open database file.\n");
			exit(status);

		case 12:
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
	fprintf(stdout,"%s %s -- a simple calendar.\n", PROGRAM_NAME, VERSION_STRING);
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
Event STRING should be of the format \"STARTTIME["DELIM"ENDTIME]"DELIM"DESCRIPTION\"\n\
Period STRING should be of the format \"[STARTTIME"DELIM"]ENDTIME\". If no STARTTIME is provided, current time is assumed.\n");

	die (status, errno);
}

void printcal (char *starttime, char *endtime, char *database, int status, int errno)
{
	/*
	 * This should take in a range of dates and print events in this period.
	 * input. Exits.
	 *	time_t?	date
	 *	int	status
	 *	int	errno
	 *
	 * If this is called using the switch -l, then both starttime and
	 * endtime is NULL.
	 * */
	time_t	start;
	time_t	end;
	FILE	*fd;
	ssize_t	nread;
	char	*line = NULL;
	size_t	len = 0;
	char	*saveptr = NULL;
	struct tm *eventtime = NULL;
	int	cmp;
	int	diff_t1, diff_t2;
	int	istty = 0;

	if (starttime != NULL) {
		struct tm *buffer;
		buffer = malloc(BUFFERSIZE);
		buffer = getdate(starttime);
		if (getdate_err) {
			die(EXIT_FAILURE, getdate_err);
		}
		start = mktime(buffer);
	} else {
		start = time(NULL);
	}
	if (endtime != NULL) {
		struct tm *buffer;
		buffer = malloc(BUFFERSIZE);
		buffer = getdate(endtime);
		if (getdate_err) {
			die(EXIT_FAILURE, getdate_err);
		}
		end = mktime(buffer);
	} else {
		/* if the switch was -l then it goes here */
		diff_t2 = 1;
	}
	fd = fopen(database, "r");
	if (fd == NULL) {
		perror("fopen");
		die(EXIT_FAILURE, 9);
	}

	//int pos = -1;
	//pos = ftell(fd);
	int pos = 1;

	if (isatty(fileno(stdout))) {
		istty = 1;
	} else {
		istty = 0;
	}

	while ((nread = getline(&line, &len, fd)) != -1) {
		char *buffer, *tmp;
		buffer = malloc(BUFFERSIZE);
		tmp = malloc(BUFFERSIZE);
		strcpy(tmp, line);
		buffer = strtok_r(tmp, DELIM, &saveptr);
		eventtime = getdate(buffer);
		if (getdate_err != 0) {
			die(EXIT_FAILURE, 10);
		}
		cmp = mktime(eventtime);
		diff_t1 = difftime(cmp, start);

		if (endtime != NULL) {
			diff_t2 = difftime(end, cmp);
		}
		int today;
		today = difftime(cmp, time(NULL));
		if (diff_t1 > 0 && diff_t2 > 0 && today < 60*60*24 && istty) {
			/* print in bold if the event is
			 * happening in less than 24 hours
			 * */
			fprintf(stdout, ONBOLD"%i\t%s"OFFBOLD, pos, line);
		} else if (diff_t1 > 0 && diff_t2 > 0) {
			fprintf(stdout, "%i\t%s", pos, line);
		}
		free(buffer);
		//pos = ftell(fd);
		pos++;
	}
	free(line);
	fclose(fd);

	die(status, errno);
}

void launcheditor (char *database, int status, int errno)
{
	/*
	 * This should just be a wrapper to launch our favorite editor.
	 * Use some systemcall.
	 * */
	char *editor = getenv("EDITOR");
	char *buffer;

	assert (database != NULL);
	if (editor == NULL) {
		editor = "vim";
	}
	buffer = malloc(BUFFERSIZE);
	strcpy(buffer, editor);
	strcat(buffer, " ");
	strcat(buffer, database);
	errno = system(buffer);
	free(buffer);

	die(status, errno);
}

void removeevent (int rmid, char *database, int status, int errno)
{
	/*
	 * This function should take in an event id and remove
	 * the event from the database. Exits.
	 * input
	 *	int	id
	 *	int	status
	 *	int	errno
	 * */

	FILE	*fd, *fd_temp;
	int nread;
	char *buffer = NULL;
	int pos = 1;
	size_t	len = 0;

	assert (database != NULL);
	fd = fopen(database, "r");
	fd_temp = tmpfile();

	//pos = ftell(fd);
	while ((nread = getline(&buffer, &len, fd)) != -1) {
		if (rmid != pos && buffer != NULL) {
			fprintf(fd_temp, "%s", buffer);
		}
		//pos = ftell(fd);
		pos++;
	}
	free(buffer);
	fclose(fd);
	fseek(fd_temp, 0, SEEK_SET);

	fd = fopen(database, "w+");
	buffer = (char *) malloc(BUFFERSIZE);
	pos = ftell(fd_temp);
	while ((nread = getline(&buffer, &len, fd_temp)) != -1) {
		fprintf(fd, "%s", buffer);
	}

	free(buffer);
	fclose(fd_temp);
	fclose(fd);

	die(status, errno);
}

void addevent (char *event, char *database, int status, int errno)
{
	/*
	 * Add event specified in *event into the file *database
	 * */

	char		*starttime = NULL;
	char		*endtime = NULL;
	char		*desc = NULL;
	char		*saveptr = NULL;
	FILE		*fd;
	struct tm	*start;
	struct tm	*end;
	char		*buffer;

	starttime = strtok_r(event, DELIM, &saveptr);

	if (strpbrk(saveptr, DELIM) == NULL) {	/* check if endtime is specified */
		desc = saveptr;
	} else {
		endtime = strtok_r(NULL, DELIM, &saveptr);
		desc = strtok_r(NULL, DELIM, &saveptr);
	}

	start = getdate(starttime);		/* result is valid only
						   until next getdate call
						   */
	if (getdate_err) {
		die(EXIT_FAILURE, getdate_err);
	}

	fd = fopen(database, "a+");
	if (fd == NULL) {
		perror("fopen");
		die(EXIT_FAILURE, 11);
	}

	buffer = (char *) malloc(BUFFERSIZE);
	strftime(buffer, BUFFERSIZE, FORMAT, start);
	fprintf(fd, "%s"DELIM, buffer);
	free(buffer);

	if (endtime != NULL) {
		end = getdate(endtime);
		if (getdate_err) {
			die(EXIT_FAILURE, getdate_err);
		}
		buffer = (char *) malloc(BUFFERSIZE);
		strftime(buffer, BUFFERSIZE, FORMAT, end);
		fprintf(fd, "%s"DELIM, buffer);
		free(buffer);
	}

	fprintf(fd, "%s\n", desc);
	fclose(fd);

	buffer = (char *) malloc(BUFFERSIZE);
	strcpy(buffer, "sort -o ");
	strcat(buffer, database);
	strcat(buffer, " ");
	strcat(buffer, database);
	system(buffer);
	free(buffer);

	die(status, errno);
}

int main (int argc, char **argv)
{
	char	*date = NULL;
	char	*event = NULL;
	int	rmid;
	int	next_option = -1;
	char	flag = 0;
	char	*starttime = NULL;
	char	*endtime = NULL;
	char	*saveptr = NULL;

	/* ensure that we're using some datemsk file
	 * */
	if (getenv("DATEMSK") == NULL) {
		setenv("DATEMSK", DATEMSK, 0);
	}

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
				if (flag & PRINT_FLAG) {	/* -l and -d are set */
					fprintf(stderr, "make up your mind, do you want -l or -d?\n");
					die(EXIT_FAILURE, 127);
				}
				flag ^= PRINT_FLAG;
				/* check if starttime is defined */
				if (strpbrk(date, DELIM) != NULL) {
					starttime = strtok_r(date, (const char *) DELIM,\
							&saveptr);
					endtime = strtok_r(NULL, (const char *) DELIM,\
							&saveptr);
				} else {
					endtime = date;
				}
				break;

			case 'f':
				database = optarg;
				break;

			case 'r':
				rmid = strtol(optarg, NULL, 10);
				flag ^= REMOVE_FLAG;
				break;

			case 'l':
				if (flag & PRINT_FLAG) {	/* -l and -d are set */
					fprintf(stderr, "make up your mind, do you want -l or -d?\n");
					die(EXIT_FAILURE, 127);
				}
				flag ^= PRINT_FLAG;
				break;

			case 'e':
				flag ^= EDIT_FLAG;
				break;

			case 'a':
				event = optarg;
				flag ^= ADD_FLAG;
				break;

				break;

			case 'h':
				usage(EXIT_SUCCESS, 0);
				break;

			case '?':
				die(EXIT_FAILURE, 12);
				break;
		}
	}
	while (next_option != -1);

#ifdef DEBUG
	fprintf(stdout, "the datemsk is set at %s\n", getenv("DATEMSK"));
	fprintf(stdout, "the database is set at %s\n", database);
#endif
	if (optind != argc) {
		die(EXIT_FAILURE, 127);
	}

	switch (flag) {
		case EDIT_FLAG:
			launcheditor(database, EXIT_SUCCESS, 0);
			break;

		case ADD_FLAG:
			addevent(event, database, EXIT_SUCCESS, 0);
			break;

		case REMOVE_FLAG:
			removeevent(rmid, database, EXIT_SUCCESS, 0);
			break;

		case PRINT_FLAG:
			printcal(starttime, endtime, database, EXIT_SUCCESS, 0);
			break;

		printcal(starttime, endtime, database, EXIT_SUCCESS, 0);
	}

	die(EXIT_FAILURE, 127);
}
