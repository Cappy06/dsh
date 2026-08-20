#include <stdio.h>
#define DSH_RL_BUFSIZE 1024
#define DSH_TOK_BUFSIZE 64
#define DSH_TOK_DELIM " \t\r\n\a"



void dsh_loop(void);
void *dsh_read_line(void);
char **dsh_split_line(char *line)
void dsh_loop(void){
   char *line;
   char **args;
   int status; //to determine when to execute
   do {
     printf("> ");
     line = dsh_read_line(); //read the command from standard input
     args = dsh_read_args(line); //seperate the command string into a program and arguements
     status = dsh_execute(args); //run the parsed command

     free(line);
     free(args);
   }while(status);
}

char *dsh_read_line(void){
//WITHOUT GETLINE
/*int bufsize = DSH_RL_BUFSIZE;
int position = 0;
char *buffer = malloc(sizeof(char) * bufsize);
int c;

if (!buffer) {
  fprintf(stderr, "dsh: allocation error\n");
  exit(EXIT_FAILURE);
}
while(1){
  // Read a character
  c = getchar();
  //If we hit EOF, replace it with a null character and return
  if (c == EOF || c == '\n'){
   buffer[position] = '\0';
   return buffer;
  } else {
   buffer[position] = c;
  }
  position++;
  //if we excedded the buffer, reallocate
  if (position >= bufsize) {
    bufsize += DSH_RL_BUFSIZE;
    bufsize = realloc(buffer, bufsize);
    if (!buffer){
      fprintf(stderr, "dsh: allocation error\n");
      exit(EXIT_FAILURE);
    }
  }
}*/
//WITH GETLINE

char *line = NULL;
ssize_t bufsize = 0; //have getline allocate a buffer for us
if (getline(&line, &bufsize, stdin) == -1){ //ssize_t getline(char **lineptr, size_t *n, FILE*stream); contains mallocs(when *lineptr is set to NULL and *n is 0 , and realloc() (when *lineptr allready points to allocated memory, getline() will use realloc if the line exceeds the current buffer size. n (size_t *): A pointer to the variable holding the size of the allocated buffer. getline() updates this if the buffer is resized. stream ( FILE * ): The input filestream to read from (e.g , stdin or a file pointer opened with fopen());
   if (feof(stdin)){
     exit(EXIT_SUCCESS); //we recieved an EOF
   } else{
     perror("readline");
     exit(EXIT_FAILURE);
   }

}
return line;
}

char **dsh_split_line(char *line){
   int bufsize = DSH_TOK_BUFSIZE, position = 0;
   char **tokens = malloc(bufsize * sizeof(char*));
   char *token;
   if(!tokens){
    fprintf(stderr, "dsh: allocation error\n");
    exit(EXIT_FAILURE);
   }
   token = strtok(line, DSH_TOK_DELIM); //first call = strtok find the first token, replaces the delimiter after it with a null character (\0) and returns a pointer to the start of the token
   while (token != NULL){
     tokens[position] = token;
     position++;
     if (position >= bufsize){
       bufsize += DSH_TOK_BUFSIZE;
       tokens = realloc(tokens, bufsize * sizeof(char*));
       if (!tokens) {
       fprintf(stderr, "dsh: allocation error\n");
       exit(EXIT_FAILURE);
       }
     }
     token = strtok(NULL, DSH_TOK_DELIM);
   }
   tokens[position] = NULL;
   return tokens;


}

int dsh_launch(char **args){

   pid_t pid, wpid;
   int status;
   pid = fork();
   if (pid == 0){
	   //Child process
	   if (execvp(args[0], args) == -1) //expectsa program name an array (also called a vector, hence the 'v') of string arguements (the first one has to be the programm name. The 'p' mean that instead of providing the full file path of the programm to run , we are going to give its name, and let the operating system search for the program in the path
		   perror("dsh");
	   exit(EXIT_FAILURE);
   } else if (pid < 0) {
     // Error forking
     perror("lsh");
   } else {
     /*// Parent process
     do {
       wpid = waitpid(pid, &status, WUNTRACED); //tells waitpd to return not only when the child terminates, but also if the child is stopped. (for e.g if the user presses ctrl+z button)
     } while(!WIFEXITED(status) && !WIFSIGNALED(status)); WIFEXITED(status) = returns true(1) if the child terminates normally (e.g reached the end of its code or calld exit()). WIFSIGNALED(status) = returns true( 1 ) if the child was terminated forcefull by an unhandled singal (e.g a segmentation fauly, or being killed)*/
	while(1){
          wpid = waitpid(pid, &status, WUNTRACED);
	  //Handle system call errors first
	  if (wpid == -1) {
               if (errno == EINTR)
		       continue; //Wait was interruped by a signal; try again
	       perror("waitpid failed");
	       break; //Fatal error, exit the loop
	  }
	  //Explicitly handle every possible state
	  if (WIFEXITED(status))
		  break; //child finished normally
	  else if (WIFSIGNALED(status))
		  break; //child was killed (e.g, SegFault, Ctrl+c)
	  else if (WIFSTOPPED(status)) {
		  int stop_signal = WSTOPSIG(status);
		  printf("[%d]+ Stopped              %s\n", pid, args[0]);
		  break; //child was suspended (e.g Ctrl+z)
			}

	}
   }
   return 1;


}

int main(int argc, char **argv){
// Load config files, if any

	// Run command loop
	dsh_loop();
       //Perform and shutdown/cleanup
	return EXIT_SUCESS;
}

