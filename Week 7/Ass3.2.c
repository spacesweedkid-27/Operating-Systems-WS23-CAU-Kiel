#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <limits.h>
#include <sys/wait.h>
#include <sys/fcntl.h>
/**
 * @brief How many background tasks are allowed? Adjust this definement based on this...
*/
#define MAX_JOBS 3

/**
 * @brief The COMMAND structure contains a parsed command.
 */
typedef struct
{
	int background;	  /*!< 1 if the command shall be executed in the background, 0 otherwise */
	char *outFile;	  /*!< if redirection of stdout is specified, outfile points to the filename of the output file, NULL otherwise */
	char *parseError; /*!< points to an error message if the command line could not successfully be parsed, NULL if the parse was successful */
	char *cmdLine;	  /*!< un-tokenized commandline (with & and > if present) */
	char **argv;	  /*!< array of the command - not including &, > or outFile */
} COMMAND;

/**
 * @brief Structure to manage a list of processes in background
*/
typedef struct
{
	pid_t jobPid[MAX_JOBS]; // processids in the list
	char *jobCommand[MAX_JOBS]; // string that saves the commands of the process
	int cells[MAX_JOBS]; // list to tell if a cell in list is free
	int running_jobs; // int that tells how many jobs are currently running in background
} PLIST;


/**
 * @brief Parses a command line.
 *
 * This function parses a command line string that may contain & (for
 * background execution) and > for stdout redirection.  parseCommand will
 * generate the argv array for the given commandline and return a COMMAND
 * structure containing the result. The COMMAND structure is dynamically
 * allocated and needs to be free'd by the caller (i.e., YOU).
 *
 *  @param   cmdLine the cmdLine to be parsed. cmdLine is tokenized during parseCommandLine.
 *  @return  pointer to the parsed COMMAND or NULL on error, with errno set appropriately. In case a custom error message is supplied, the function returns a COMMAND structure that has the parseError field set to a custom error message. If the parseError field is set to a value other than null, parsing the command line has failed and the remaining fields of the COMMAND structure must not be interpreted.
 *
 */
COMMAND *parseCommandLine(char *cmdLine);

/**
 * @brief Prints the status of a terminated or stopped child process.
 *
 * @param cmdLine command line of the child process
 * @param status  process status as returned by wait(2)
 */
void printStat(const char *cmdLine, int status);

/**
 * @brief Prints a prompt symbol including the working directory.
 */
void prompt();

/**
 * \brief Collects zombies and prints status of terminated child processes.
 */
static void collectBGProc();

static void execute(COMMAND *cmd);

static int printJob(pid_t pid, const char *cl);

static void printJobs();

static int insertJob(pid_t pid, char* command);

static int removeJob(pid_t pid, char* comandPointer);

PLIST plist;

COMMAND *parseCommandLine(char *cmdLine)
{
	// init the structures we need to save the parsed data
	COMMAND *cmd;
	size_t cmdsize;
	size_t realsize;
	size_t len = strlen(cmdLine);
	size_t ceillen;
	int argc = 0;

	// strip newline at the end of every command (due to the '\n' to enter the command)
	if (cmdLine[len - 1] == '\n')
	{
		cmdLine[--len] = '\0';
	}

	// allocate command structure
	ceillen = ((len + 1) / sizeof(int) + 1) * sizeof(int); // ceil to sizeof(int)
	cmdsize = sizeof(*cmd);
	cmdsize += ceillen;		   // for cmdLine copy
	cmdsize += sizeof(char *); // for argv array
	cmdsize += sizeof(char *); // for NULL terminator in argv
	realsize = cmdsize + 128;

	// malloc the whole size of the command struct
	cmd = malloc(realsize);
	if (cmd == NULL)
	{
		return NULL;
	}

	// create copy of tokenized command line
	char *cmdLineCopy = ((char *)cmd) + sizeof(*cmd);
	strncpy(cmdLineCopy, cmdLine, len + 1);
	cmd->cmdLine = cmdLineCopy;

	// split the command into seperated strings unsing strtok with whitespaces as seperators
	char **argvPos = (char **)(((char *)cmd) + sizeof(*cmd) + ceillen);
	for (char *atok = strtok(cmdLine, " \t"); atok; atok = strtok(NULL, " \t"))
	{
		cmdsize += sizeof(char *);
		if (cmdsize > realsize)
		{
			realsize = cmdsize + 128;
			// reallocate the command based on the amount of arguments
			COMMAND *ncmd = realloc(cmd, realsize);
			if (ncmd == NULL)
			{
				free(cmd);
				return NULL;
			}
			argvPos = (char **)ncmd + (argvPos - (char **)cmd); // adjust to new position
			cmd = ncmd;
		}
		*(argvPos++) = atok;
		argc++;
	}
	*argvPos = NULL;

	// Regenerate cmd structure
	cmdLineCopy = ((char *)cmd) + sizeof(*cmd);
	cmd->cmdLine = cmdLineCopy;

	cmd->argv = (char **)(cmdLineCopy + ceillen);

	// check for empty command
	if (argc < 1)
	{
		cmd->argv[0] = 0;
		return cmd;
	}

	// check if the last argument is a '&' char to make the command run in background
	/* Command example: yes & */
	if (strcmp(*(argvPos - 1), "&") == 0)
	{
		cmd->background = 1;
		argc--;
		if (argc < 1)
		{
			cmd->parseError = "background execution of empty command requested";
			return cmd;
		}
		argvPos--;
		*argvPos = NULL;
	}

	// check wether the last argument starts with a '>' and save the filename/path
	/* Command example: wc <file> >text.txt */
	if ((*(argvPos - 1))[0] == '>')
	{
		cmd->outFile = &(*(argvPos - 1))[1];

		if (strlen(cmd->outFile) < 1)
		{
			cmd->parseError = "stdout redirection to zero length filename requested";
			return cmd;
		}

		argc--;
		if (argc < 1)
		{
			cmd->parseError = "stdout redirection of empty command requested";
			return cmd;
		}
		argvPos--;
		*argvPos = NULL;
	}

	// checks if a '>' is in the command
	/* Command example: ls > text.txt */
	if(argc > 2) {
		if (strcmp(*(argvPos - 2), ">") == 0) {
			cmd->outFile = *(argvPos - 1);
			if (strlen(cmd->outFile) < 1)
			{
				cmd->parseError = "stdout redirection to zero length filename requested";
				return cmd;
			}
			argc -= 2;
			if (argc < 1)
			{
				cmd->parseError = "stdout redirection of empty command requested";
				return cmd;
			}
			argvPos--;
			*argvPos = NULL;
			argvPos--;
			*argvPos = NULL;
		}
	}

	return cmd;
}

static long getMaxLineLength(void) {
	//TODO: implement me
}

int main() {
	char* buffer = malloc(128);
	scanf("%s", buffer);
	COMMAND* example = parseCommandLine(buffer);
	printf("COMMAND = %s\tARG = %s\nRAW = %s\nPARSEERROR = %s\nOUTFILE = %s\nBACKGROUND = %d\n", example->argv[0], example->argv[1], example->cmdLine, example->parseError, example->outFile, example->background);

	return 0;

	char line[256];
	char* cwd = malloc(128);
	while(1) {
		getcwd(cwd, 128);

		fprintf(stdout, "%s", cwd); // HIER IST NE BAUSTELLE, ':' WIRD BEI MIR SCHNELLER GEPRINTED ALS DER REST
		fprintf(stderr, ":");		// DIE WOLLEN ABER, DASS DAS IN DER MITTE IST, ALSO WIE ES HIER STEHT
		fprintf(stdout, " ");		// SUCH MAL WAS DARÜBER ODER FIXE ES

		scanf("%s", line);

		if (getchar() == EOF){
			printf("\n");
			break;
		}

		printf("%s\n", line);
		char* argv[] = {line, NULL};

		COMMAND cmd = {0, NULL, NULL, argv[0], argv}; //parseCommandLine(line);
		execute(&cmd); // example
	}
}

/**
 * \brief Executes a command in a child process.
 * Blocks for foreground processes until termination of the foreground process.
 * @arg cmd pointer to the command struct that shall be executed
 */
static void execute(COMMAND *cmd) {
	if (cmd->parseError){
		printf("%s\n", cmd->parseError);
		return;
	}

	int pid_id_child = fork();
	int status;

	if (pid_id_child == 0){
		execv(cmd->argv[0], cmd->argv); // quits automatically so there does not need to be a "quit(0);" after this.
	}

	if (!cmd->background){
		waitpid(pid_id_child, &status, 0);	// Wait for child to be destroyed.
	}
	printf("Exitstatus [%s] = %d\n", cmd->cmdLine, status);
}

/* Helper for background task list command
returns the cell it saved it into, -1 for error */
static int insertJob(pid_t pid, char *command) {
	// locate next free cell
	int freeCell = -1;
	for (int i = 0; i < MAX_JOBS; i++) {
		// command should be NULL if empty
		if (!plist.cells[i]) {
			freeCell = i;
		}
		if (plist.jobPid[i] == pid) {
			freeCell = -1;
			break;
		}
	}
	if (freeCell == -1) {
		return -1;
	} else {
		plist.running_jobs++;
		plist.jobPid[freeCell] = pid;
		plist.jobCommand[freeCell] = calloc(strlen(command) + 1, sizeof(char));
		strcpy(plist.jobCommand[freeCell], command);
		plist.cells[freeCell] = 1;
		return freeCell;
	}
}

/* Helper function to remove a pid from the background process list
returns the cell the process was in, -1 if any error accures*/
static int removeJob(pid_t pid, char *comandPointer) {
	int foundCell = -1;
	for (int i = 0; i < MAX_JOBS; i++) {
		if (plist.jobPid[i] == pid) {
			foundCell = i;
			break;
		}
	}
	plist.cells[foundCell] = 0;
	strcpy(comandPointer, plist.jobCommand[foundCell]);
	free(plist.jobCommand[foundCell]);
	plist.jobCommand[foundCell] = NULL;
	return foundCell;
}

/* Helpers for the jobs command */
static int printJob(pid_t pid, const char *cl) {
	//TODO: implement me
}

static void printJobs() {
	//TODO: implement me
}

/* collects terminated children and prints status */
static void collectBGProc() {
	//TODO: implement me
}

/* prints status of terminated child processes */
void printStat(const char *cmdLine, int status) {
	//TODO: implement me
}

/*
 * PATH_MAX is the maximum _relative_ path length (cf. [1]), not the maximum
 * absolute path length. Therefore neither PATH_MAX nor pathconf(".",
 * _PC_PATH_MAX) can be used here. In addition getcwd() is required to be able
 * to write paths longer than PATH_MAX if the buffer is bigger (cf. [2]).
 *
 * [1]: http://pubs.opengroup.org/onlinepubs/9699919799/functions/pathconf.html#tag_16_157_08
 * [2]: http://pubs.opengroup.org/onlinepubs/9699919799/xrat/V4_xsh_chap02.html#tag_22_02_03
 */
void prompt() {
	//TODO: implement me
}
