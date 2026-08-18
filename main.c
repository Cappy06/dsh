#include <stdio.h>
#define DSH_RL_BUFSIZE 1024

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

int main(int argc, char **argv){
// Load config files, if any

	// Run command loop
	dsh_loop();
       //Perform and shutdown/cleanup
	return EXIT_SUCESS;
}

