/*
   Name: Mario Shamhon
    
   Instructor: John Carroll
    
   Description: This program acts like a commandline shell. It is able to do the following: Run vaild commands, input and output redirection, commands
   that involve pipes, and lastly it can run commands in the background. The shell will keep running until a user enters "-255". This shell also performs 
   error checking when approprate, and notifies the user then presents a new prompt for the user to enter a new command. Some examples of the error checking 
   the shell performs is: not overwritting existing file, and telling the user when they enter an invalid command. This shell has some built-in functions. 
   An example of this is the change directory command ("cd"). The cd command can change a user's working directory. If cd is the only command then the user's 
   working directory will be redirected to the user's home directory. The user can also supply one another argument to the cd command. The one other argument
   being a valid directory to be redirected to. If the directory is not valid the shell will alert the user. This program also makes use of a seperate function 
   called "parse". Parse is responsible for taking all of the words typed onto the commandline by the user, and placing them into a newArgv array so that the 
   shell can process commands correctly. Parse also sets flags when characters that have a special meaning are found. Parse also calls a seperate function  
   called getword that reads the characters typed into the commandline, and assembles them into words. Words are delimated by a space, tab, newline, or                                                          meta-characters. These special meta-characters are: "<", ">", "|", "$", and "&". The first three meta-characters all deal with some type of redirection,
   but when an "&" character is the last word found on the commandline that command will run in the background, print out the process ID (PID) of the process 
   executing the background job and allow the user to run other commands while that one finishes. The newline and "$" characters act like line terminators. 
   This shell handles one line of commands at a time  Here is a breakdown behind  the basic logic of this shell: 

   1. The user types a command, then parse is called, and places all the "words" on one line into our newArgv array. It also sets up any built-in functions, 
      input, output, or pipe redirections if necessary. 

   2. If the command is not a built-in function a new process will be created to execute the command. The shell first checks if there are any errors the 
      shell will report them to the user then issue a new prompt for the user to reneter their command correctly. If there are no errors the shell will 
      execute the command, then present a new prompt ready to handle the next valid command. This will continue until a user types "-255" then the shell 
      will terminate.

   The shell has been updated to include some new features. These features include using commands with up to ten pipelines, a new built-in called environ, 
   hereis documents, when a dollar sign is typed in front of a word it checks if that word is valid enviroment variable, and finally when a user types 
   a "~" followed by a string of words the shell now looks up that username and returns the whole path of that string words if a valid username was given.   
   
   Here is what the new environ built-in command can do: 
   
   1. When given one argument it looks up that enviroment variable and prints it the user if the enviroment variable is valid. 
   
   2. When given two arguments it sets the enviroment variable that was given in the first arugment to the new value in the second argument. 
      For example, "environ HOME /tmp" will cause the value in the enviroment variable HOME to now be set to "/tmp".    
    
   Side Note: This shell can handle a maximum of 254 characters per word, and a one hundred words per line so in total this shell can handle 25,400 words 
              per line.     
   
   Project: Four 
   
   Class: CS570
   
   Due Date: Tuesday 11/29/2020
*/

#include "p2.h" 

/* START: This section has all the global variable declarations */
char *newArgv[MAXITEM]; //This is array that will store all the "words" on the line.  
int newArgc = 0; //This will keep track of how many arguments newArgv has.
int eofFlag = 0; // A flag to indicate whether EOF was found. 
const int newEOF = -255; // This replaces the real EOF signal to fit the p2 specs
int newlineFlag = 0; // Creating a custom newline variable so the code is more readable.
char *inputfilename = ""; // This will hold the filename for input redirection.
int lessthanFlag = 0; // This a flag that indicates that a "<" character was found.
char *outputfilename = ""; // This will hold the filename for output redirection.
int greaterthanFlag = 0; // This a flag that indicates that a ">" character was found.
extern int lastampFlag; // This a flag from getword that indicates that a "&" character was the last word on the line.
int pipeFlag = 0; // This a flag that indicates that a "|" character was found.
int cdFlag = 0; //This a flag that indicates that "cd" was found.
char const *devNull = "/dev/null"; //This holds the address to "/dev/null"
int environFlag = 0; //This a flag that indicates that "environ" was found.
int dollarSignFlag = 0; //This flag indicates whether a "$" was found in front of a word.
int invalidEvFlag = 0; //This flag indicates if the user gave an invaild enviroment variable. 
extern int tildeFlag; //This a flag from getword that indicates that a "~" character was the first word on the line.
int tildeErrorFlag = 0; //This flag indicates that an invaild username was typed.
int cdWasTyped = 0; //This flag indicates that a user typed cd, so the prompt can be modified correctly if need be. 
char *heredocumenttempfile = "tempfile"; //This will hold the filename for the temp file needed for heredocument redirection.
char *delimiter = ""; //This holds the delimiter for the here document.
int lastSetOfPipes = 0; //Holds the index for the last set of pipes 
int lastPipeCmdIndex = 0; //Holds the index for the last pipe command.
int totalPipeFds = 0; //Holds the total number of pipeline file descriptors that we will need.
int pipemiddleguys = 0; //The number of "middle guys" needed with pipe commands
int hereDocFlag = 0; //A flag to let us know a "<<" was typed.  
int invalidAmpOutputRedirectionFlag = 0; //This is a flag that indicates when a user tries to use "&" as outputfilename and its the last thing on the commandline.
/* END: end of section. */

/* START: This section states all of the exit status codes for p2.
exit(1) - Means that the fork() call failed.
exit(2) - Means that the execvp() call failed.
exit(3) - Means that the open() call failed.
exit(4) - Means that the dup2() call failed.
exit(5) - Means that the close() call failed.
exit(6) - Means cannot create output file. 
exit(7) - Means pipe() call failed  
exit(8) - Means won't overwrite existing file.
exit(9) - Means failed to delete the temp file in hereis document cases.
END: end of section. 
*/

int killpg(int pgrp, int sig); //decleration for killpg() call so splint doesn't give a warning 
int setenv(const char *name, const char *value, int overwrite); //decleration for setenv() call so splint doesn't give a warning
ssize_t getline(char **lineptr, size_t *n, FILE *stream); //decleration for getline() call so splint doesn't give a warning
void parse(); //parse function prototype. 

/*START: START of myhandler function */
void myhandler(int signum) //we dont do anything here, this just catches the kill signal. 
{
  
}
/*END: End of myhandler function */

/* START: start of main */
int main() 
{
 int inputfd = 0; // A file descriptor to handle input redirection.
 int outputfd = 0; // A file descriptor to handle output redirection.
 int pipefd[20]; //These are file descriptors for the pipe function
 pid_t childProcess; //This is the child process all shell functions except when "&" is the last word
 pid_t grandchildProcess; //This is the grandchild process for the pipe function
 pid_t genericMiddleProcess; //This is used in the pipe commands to run all the "middle" commands.
 char cwd[1000] = ""; //This holds the current working directory 
 int i = 0; //This is used to track all the indexes that the pipeline read from and write
 int k = 0; //This counter is used in for loops to close all pipe file descriptors.
 int m = 0; //This counter is used when creating the pipes to ensure we make the correct number of pipes needed.
 int hereDocumentfd = 0; // A file descriptor to handle here document redirection.
 int nextPipeCmd = 0; //This holds the index for the appropriate pipe command.
 int offset = 0; //This keeps track of the offset for the pipe commands
 int gencount = 0; //This keeps track of what "generation" we are when using pipelines.

 (void) signal(SIGTERM, myhandler); //Calls the signal handler to catch the kill signal
    
 /* START: This section handles placing p2 in its own process group. */
 if((setpgid(0, 0)) == -1) //This uses p2's PID as the process group ID, then puts p2 into its own process group.
 {
    perror("setpgid failed");
 } 
  /* END: End of placing p2 in its own process group section. */
 
 for(;;)
 { 
   if(cdWasTyped == 0) //checks if the user typed "cd" or not to show the appropriate prompt. 
   {
      printf(":570: "); //prints the prompt without no directory shown (user hasn't typed cd yet).
   }
   
   if(cdWasTyped == 1) //checks if the user typed "cd" or not to show the appropriate prompt. 
   {
     printf("%s:570: ", basename(cwd)); //prints the prompt the current working directory shown (user  typed cd).
   }
   parse(); // Calls parse function
        
   /* START: This section does all of the error checks for all "<" functions */
   if(lessthanFlag > 1) //Handles when there is more than one "<" character, and prints an error message
   {
     fprintf(stderr, "Error: ambiguous input redirection. Please try again.\n");
     continue;
   }
   
   if( (newArgc == 0) && (lessthanFlag == 1) ) //If a "<" was typed, but no commands were typed not given print error.
   {
     fprintf(stderr, "Invaild input redirection, the filename: %s was specifed, but not a vaild command. Please try again.\n", inputfilename);
     continue; 
   }
   
   if( (*inputfilename == 0) && (lessthanFlag == 1) ) //If there "<", but a input filename was not given print error. 
   {
     fprintf(stderr, "Input redirection failed, a filename was not typed after the '<' character please provide a valid filename.\n");
     continue;
   }
   /* END: End of the error checks for all "<" functions section */
   
   /* START: This section does all of the error checks for all ">" functions */
   if(greaterthanFlag > 1) //Handles when there is more than one ">" character, and prints an error message
   {
     fprintf(stderr, "Error: ambiguous output redirection. Please try again.\n");
     continue;
   }
     
   if( (newArgc == 0) && (greaterthanFlag == 1) ) //If a ">" was typed, but no commands were typed not given print error.
   {
     fprintf(stderr, "Invaild output redirection, the filename: %s was specifed, but not a vaild command. Please try again.\n", outputfilename);
     continue; 
   }
   
   if( (*outputfilename == 0) && (greaterthanFlag == 1) ) //If a ">" was typed, but a output filename was not given print error. 
   {
     fprintf(stderr, "Output redirection failed, a filename was not typed after the '>' character please provide a valid filename.\n");
     continue;
   }
   
   if( (greaterthanFlag == 1) && ( (strcmp(outputfilename, "<") == 0) || (strcmp(outputfilename, "|") == 0) || (strcmp(outputfilename, "&") == 0) )   ) //If a metacharacter is used as filename for output redirection. 
   {
     fprintf(stderr, "Output redirection error cannot use metacharacters as output filenames\n");
     continue;
   }
   
   if(invalidAmpOutputRedirectionFlag == 1)//checks if "&" was used as an output filename, and was the last thing typed on the command.
   {
     fprintf(stderr, "Output redirection error cannot use metacharacters as output filenames\n");
     continue;
   }
   
   /* END: End of the error checks for all ">" functions section */
   
   /* START: This section does all of the error checks for all "|" functions */   
   if( (newArgc == 0) && (pipeFlag == 1) ) //If a "|" was typed, but no commands were typed not given print error.
   {
     fprintf(stderr, "Invaild pipe redirection, a '|'  was specifed, but not vaild commands. Please try again.\n");
     continue; 
   }
   
   if( (newArgv[0] == NULL) && (pipeFlag > 0) ) //If there is no first command. Error will print
   {
     fprintf(stderr, "ambiguous pipe redirection. No first command. Please try again\n");
     continue;
   }
   
   if( (newArgv[lastPipeCmdIndex] == NULL) && (pipeFlag > 0) ) //If there is no last command. Error will print
   {
     fprintf(stderr, "ambiguous pipe redirection. Please try again\n");
     continue;
   }

   /* END: End of the error checks for all "|" functions section */
   
   if(invalidEvFlag == 1) //Error check if an invalid enviroment variable typed with a dollar sign in front.
   { continue; }
   
   if(tildeErrorFlag == 1) //Error check if an invalid user typed with a tilde character in front.
   { continue; }
   
   if( (lessthanFlag == 1) && (hereDocFlag == 1) ) //checks if user tried to do both types of input redirection on the same line.
   {
     fprintf(stderr, "Error: ambiguous input redirection. Please try again.\n");
     continue;
   }
   
   if( (newArgc == 0) && (hereDocFlag == 1) ) //checks if user typed "<<" with a delimiter but no program name.
   {
     fprintf(stderr, "Error: ambiguous input redirection no command was typed before '<<' Please try again.\n");
     continue;
   }
   
   if( (hereDocFlag == 1) && (*delimiter == 0) ) // check if a delimiter was not typed in hereis cases.
   {
      fprintf(stderr, "Error: no delimiter was typed after the <<. Please try again\n"); //print error message.
      continue;
   }
    
   /* START: This section check if there no words on the line or if we should terminate the shell */
   if( (eofFlag == 1) && (newArgc == 0) ) // if  EOF is found break out of the loop, and terminate p2. 
   {
      break; 
   } 

   if(newArgc == 0)  // If line is blank start over.
   {
     continue;
   }
   /* END: end of check if there no words on the line or if we should terminate the shell section. */

   /* START: This section handles the built-in cd command */
   if(cdFlag == 1) //Handles all cases when "cd" is found
   {
     if(newArgc > 2) // checks for too many arguments, and prints error message.
     {
       fprintf(stderr, "ambiguous cd command, too many arguments. Please try again.\n");
       if (getcwd(cwd, sizeof(cwd)) == NULL) //gets the current working directory, if getcwd fails prints error message.
       {
         fprintf(stderr, "getcwd() call failed. Please try again.\n");
         continue;
       }
       else{continue;} //if getcwd didn't fail continue
     }

     if(newArgc == 1) // Handles the case when "cd" has no arguments.
     {
       if(chdir(getenv("HOME") ) == -1) //This calls chdir and checks the return value 
       {
         fprintf(stderr, "Error: Could not go to home directory. Please try again.\n"); //prints error message if chdir fails
       }
       
       if (getcwd(cwd, sizeof(cwd)) == NULL) //gets the current working directory, if getcwd fails prints error message.
       {
         fprintf(stderr, "getcwd() call failed. Please try again.\n");
         continue;
       }
       else{continue;} //if getcwd didn't fail continue
     }

     if(newArgc == 2) // Handles the case when "cd" has one argument.
     { 
       if(chdir(newArgv[1]) == -1) //This calls chdir and checks the return value 
       {
         fprintf(stderr, "%s is a invalid directory. Please try again\n", newArgv[1]); //prints error message if chdir fails
       }
       
       if (getcwd(cwd, sizeof(cwd)) == NULL) //gets the current working directory, if getcwd fails prints error message.
       {
         fprintf(stderr, "getcwd() call failed. Please try again.\n");
         continue;
       }
       else{continue;} //if getcwd didn't fail continue
     }
        
   } /* END: end of "cd" buit-in code. */
   
   /* START: This section handles the built-in environ command */
   if(environFlag == 1) //Handles all cases when "environ" is found
   {
     if(newArgc > 2) // checks for too many arguments, and prints error message.
     {
       fprintf(stderr, "ambiguous environ command, too many arguments. Please try again.\n");
       continue;
     }

     if(newArgc == 1) // Handles the case when "environ" has one argument.
     {
       if(getenv(newArgv[1]) != NULL) //This calls getenv and checks the return value 
       {
          printf("%s\n", getenv(newArgv[1]));
          continue; 
       }
       else //if getenv returns null.
       {
         printf("\n"); //prints a newline when an environ look up variable isn't valid.
         continue;
       } //if chdir didn't fail continue
     }

     if(newArgc == 2) // Handles the case when "environ" has two arguments.
     { 
       if(setenv(newArgv[1], newArgv[2], 1) == -1) //This calls setenv and checks the return value 
       {
         fprintf(stderr, "%s is a invalid enviroment could not make the new value %s. Please try again\n", newArgv[1], newArgv[2]); //prints error message if setenv fails
         continue;
       }
       else{continue;} //if setenv didn't fail continue
     }
        
   } /* END: end of "environ" buit-in code. */
   
   /* START: Start of child process section for all of the functions of the shell. */
  if(newArgc > 0)  
  {    
   fflush(stdout); //This flushes out the stdout so the child process gets a clean buffer before it's created
     
   fflush(stderr); //This flushes out the stderr so the child process gets a clean buffer before it's created
   
   if((childProcess = fork()) == -1) //this calls fork to create a child process to handle non built-in related commands Checks to see if fork failed, prints error message, and exits
   {
     fprintf(stderr, "Failed to fork a child process.\n");
     continue; //The reason we use continue here instead of exit is because if fork ever failed the exit call would cause the shell to terminate the continue allows the shell to just print another prompt to the user. 
   }

   if(childProcess == 0) //we are now the child process 
   { 
   
   /* START: This sections handles input redirection */
    if(lessthanFlag == 1) // This checks if the line from the shell is valid to do input redirection
    {
     if((inputfd = open(inputfilename, O_RDONLY)) == -1) //This opens the file we want to send to stdin, but if it fails, prints an error message
     {
       fprintf(stderr, "Error could not open the following file: %s. Please try again.\n", inputfilename);
       exit(3);
     }
     
     if(dup2(inputfd, STDIN_FILENO) == -1) //This redirects stdin to the input file but checks the return value, and prints, an error message if it failed.
     {
       perror("dup2 failed");
       exit(4);
     }
     
     if(close(inputfd) == -1) //This closes our file descriptor when were done using it, but checks the return value, and prints, an error message if failed.
     {
       perror("close failed");
       exit(5);
     }
   
   } /* END: end of input redirection section. */
   
      /* START: This section handles all here document redirection */
   if(hereDocFlag == 1) //This check if there a here Document command typed.
   {
     if((hereDocumentfd = open(heredocumenttempfile, O_RDONLY)) == -1) //This opens the temp file that has all the heredocument input we want to send to stdin, but if it fails, prints an error message.
     {
       fprintf(stderr, "Error could not open the following file: %s. Please try again.\n", heredocumenttempfile);
       exit(3);
     }
     
     if(dup2(hereDocumentfd, STDIN_FILENO) == -1) //This redirects stdin to the here document temp file, but checks the return value, and prints, an error message if it failed.
     {
       perror("dup2 failed");
       exit(4);
     }
     
     if(close(hereDocumentfd) == -1) //This closes our file descriptor when were done using it, but checks the return value, and prints, an error message if failed.
     {
       perror("close failed");
       exit(5);
     }
     
     if (remove(heredocumenttempfile) == -1) //This deletes the here document temp file after were done redirecting input.
     {
       fprintf(stderr, "Error could not delete the following file: %s. Please try again.\n", heredocumenttempfile);
       exit(9); 
     } 
   
   } /* END: end of here document section. */
   
   /* START: This sections handles output redirection */
   if(greaterthanFlag == 1) // This checks if the line from the shell is valid to do output redirection
   {  
     if((outputfd = access(outputfilename, F_OK)) == 0)// checks if the file already exists and prints, an error message if needed
     {
       fprintf(stderr, "Error the file: %s already exists will not overwrite file. Please try again.\n", outputfilename);
       exit(8);
     }
       
     if((outputfd = creat(outputfilename, (mode_t)S_IRUSR|(mode_t)S_IWUSR)) == -1) //creates the output file, and if creat fails, and prints an error message if needed.
     {
       fprintf(stderr, "Error: could not create file. Please try again\n");
       exit(6);
     }
     
     if(dup2(outputfd, STDOUT_FILENO) == -1) //redirects STDOUT to the output file, checks if dup2 fails, and prints error message 
     {
       perror("dup2 failed");
       exit(4);
     }
     
     if(close(outputfd) == -1) //This closes our file descriptor when were done using it, but checks the return value, and prints, an error message if failed.
     {
       perror("close failed");
       exit(5);
     }
     
   } /* END: end of output redirection section. */
   
    /* START: This section handles all pipe functions */
   if(pipeFlag > 0) 
   { 
     if(childProcess == 0) //we are now the child process
     {
       for(m = 0; m <= totalPipeFds-1; m+=2) //make all the needed pipes 
       {  
         if(pipe(pipefd + m) == -1) //Creates all pipes.
         {
           perror("pipe failed");
           exit(7);
         }
       } 
            
         if((grandchildProcess = fork()) == -1) //This calls fork to create a grandchild process to all pipe related commands, Checks to see if fork failed, prints error message, and exits
         {
           fprintf(stderr, "Failed to fork a grandchild process.\n");
           exit(1);
         }
       
         if(grandchildProcess == 0) //we are now the grandchild process 
         {  
          if(pipemiddleguys > 0) //Checks if we need to create any middle guys to handle any commands.
          {
           for(;;) //we create all the middle guys here 
           {  
                  if(pipemiddleguys == gencount) //if he's the last guy 
                  {
                      i = 0; //set i to 0 so the last generation writes to the first pipe.
                     if(dup2(pipefd[i+1], STDOUT_FILENO) == -1) //redirects STDOUT to the write end of the pipe if dup2 fails, and prints error message  
                     {
                       perror("dup2 failed");
                       exit(4);
                     }
        
                     for (k = 0; k < totalPipeFds; k++) //closes all pipe fds.
                     {
                       close(pipefd[k]);
                     }
         
                     if(execvp(newArgv[0], newArgv) == -1) //runs the first command 
                     {
                       fprintf(stderr, "[%s] is not a valid command, please try again.\n", newArgv[0]);
                       exit(2);
                     }            
                  }
                  
                  //if were not the last guy 
                  for(;;) //This loop gets the index for the next command.
                  {
                    if(newArgv[offset+1] != NULL)
                    {
                      offset++;
                    }
                   else
                  {
                    nextPipeCmd = (offset) + 2;
                    offset = nextPipeCmd;
                    break; 
                  }
                }
                 
               i = i + 2; //increments the i "index" so we now which pipes ends to read and write from
                gencount++; //increments the counter so we can keep track of the "generations" 
                
               if((genericMiddleProcess = fork()) == -1) //This calls fork to the next generation to handle all middle pipe commands. Checks to see if fork failed, prints error message, and exits
               {
                 fprintf(stderr, "Failed to fork a process.\n");
                 exit(1);
               }
               
               if(genericMiddleProcess == 0) //We are now the newly forked "next generation" 
               {
                 continue; //the next generation goes to the top of the loop to check if he's the "last generation".
               }
               else //the parent of the generation that was just created. 
               {
                 break; //the parent of the generation that was just created breaks out of the loop to read and write to the appropriate pipes. 
               }
               
         }// end of for loop 
               
         if(dup2(pipefd[i-2], STDIN_FILENO) == -1) //reads from the pipe if dup2 fails, and prints error message  
         {
               perror("dup2 failed");
               exit(4);
         }
         
        } //end of if pipemiddleguys > 0
         
         if(dup2(pipefd[i+1], STDOUT_FILENO) == -1) //redirects STDOUT to the write end of the pipe if dup2 fails, and prints error message  
         {
           perror("dup2 failed");
           exit(4);
         }
        
         for (k = 0; k < totalPipeFds; k++) //closes all pipe fds
         {
           close(pipefd[k]);
         }
         
         if(execvp(newArgv[nextPipeCmd], newArgv + nextPipeCmd) == -1) //runs all the middle commands 
         {
           fprintf(stderr, "[%s] is not a valid command, please try again.\n", newArgv[nextPipeCmd]);
           exit(2);
         }  
      
        }//end of grandchild eqauls zero section
        
        
       if(dup2(pipefd[lastSetOfPipes], STDIN_FILENO) == -1) //redirects STDIN to the read end of the last pipe if dup2 fails, and prints error message 
       {
         perror("dup2 failed");
         exit(4);
       }
              
       for (k = 0; k < totalPipeFds; k++) //closes all pipe fds in the child process.
       {
         close(pipefd[k]);
       }
              
       if(execvp(newArgv[lastPipeCmdIndex],newArgv+ lastPipeCmdIndex) == -1) //runs the last command for the pipe, and checks to see if execvp  failed, prints an error message, and exits 
       { 
           fprintf(stderr, "[%s] is not a valid command, please try again.\n", newArgv[lastPipeCmdIndex]);
           exit(2);
       }
       
     }// end of childprocess eqauls zero pipe section.
     
  }//End of if pipeflag ==1
   
   /* END: end of all pipe functions section */
   
   /* START: This section executes all commands except  when "&" is the last word */
   if(lastampFlag == 0) //This executes the command when "&" is not the last word on the line
   {  
     if(execvp(newArgv[0],newArgv) == -1) //Checks to see if execvp failed, prints an error message, and exits
     {
       fprintf(stderr, "[%s] is not a valid command, please try again.\n", newArgv[0]);
       exit(2);
     }
   }  /* END: end of execvp section. */
     
     
   } //end of if childprocess == 0 statement 
      
  } /* END: end of  if newArgc > 0 */ 
   
   /*START: This section handles the parent the waiting for the child process to finish (besides when "&" is the last word) */
   if( (childProcess > 0)  &&  (lastampFlag == 0) ) //we are the parent process again, here wait for child to complete when & is not the last word.
   {
     for(;;) //This loop reaps zombie children proccesses
     {
        pid_t pid;
        
        CHK(pid = wait(NULL)); //CHK is a nice macro that does the error checking 
        
        if(pid == childProcess) //This waits for the correct process to be found then breaks out of the loop.
        {break;} 
      
      } //End of for loop 

   } /* END: End of parent process waits for child processes to finish section. */
   
   /*START: This section handles when "&" is the last word */
   if(lastampFlag == 1)
   { 
      if(childProcess == 0) // we are the childProcess process 
      {
         if(lessthanFlag == 0) // Redirect the background child process stdin to "/dev/null" when there is no other input redirection
         {
           if((inputfd = open(devNull, O_RDONLY)) == -1) //opens the file to "/dev/null" we want to send to stdin, but if it fails, prints an error message
           {
             fprintf(stderr, "Failed to redirect the background child process standard input to %s.\n", devNull);
             exit(3);
           }
     
           if(dup2(inputfd, STDIN_FILENO) == -1) //redirects the child process stdin to "/dev/null", but checks the return value, and prints, an error message if it failed.
           {
             perror("dup2 failed");
             exit(4);
           }
     
           if(close(inputfd) == -1) //closes our file descriptor when were done using it, but checks the return value, and prints, an error message if failed.
           {
             perror("close failed");
             exit(5);
           }
   
         } //end of lessthanFlag == 0
             
         /* START: This section executes all commands for background jobs (when "&" was the last word found.) */
         if(execvp(newArgv[0],newArgv) == -1) //Checks to see if execvp failed, prints an error message, and exits
         {
           fprintf(stderr, "[%s] is not a valid command, please try again.\n", newArgv[0]);
           exit(2);
         } /* END: End of this section executes all commands when "&" was the last word found. */  
         
      }// end of childProcess == 0  
      
      else if(childProcess > 0) //we are the  parent process 
      { 
        printf("%s [%d]\n", newArgv[0], childProcess); //The parent will print the program name, and the child's pid that is executing the program.
      }
      
   }//end of lastampFlag == 1
   
   /* END: End of executes all commands for background jobs (when "&" was the last word found.) section */
          
 }// End of for loop.
  
 killpg(getpgrp(), SIGTERM); // Terminate any children that are still running.

 printf("p2 terminated.\n");

 exit(0);   

} /*END: end of main */



/*START: START of parse function */

/* Descrption of function parse: This function's responbility is to take words put them in 
 * the newAgrv array for processing and set the approprate flags when meta-characters, and 
 * other special "words" are found. The way this function operates is as follows: 
 * It calls another function getword, and sends it a pointer, and a buffer to use. 
 * getword then assembles the characters read-in from the standard input stream 
 * into words, and returns an address pointing to each individual word to parse. 
 * Parse keeps calling getword until a newline or a lone dollar sign is found.
 * Here's the basic logic behind this function: 
 
 1. Parse calls getword passing it a buffer, and a pointer to use
    getword then returns that pointer that now points to the address 
    of word getword collected. 

 2. Most of the time parse will then place the address of the returned 
    word into the newArgv array. Although when a special word is found 
    parse handles those special words differently. These special words 
    are these meta-characters: "<", ">", "|", and "&". Other special 
    words are: "cd", and "-255". In the case where "<", ">", or "|"
    are found either input, output, or pipe redirection is set up. 
    When "cd" is the first word found on the line parse will 
    set up our built cd function. When "-255" is the first word 
    found on the line parse will set things up so the shell knows 
    to terminate. The way that parse sets up all these features is
    by setting flags in the appropriate situations which tell our
    shell what to do in the main function.   

 Parse has now been updated and now sets up new features. 
 These new features includie: setting up the new built-in 
 "environ", hereisdocutments when a "<<" is found, username 
 lookups when a "~" is typed and contains a string after it, 
 and finally looks up enviroment variables when a dollar sign 
 is found in front of a word, and parse places the string of 
 that enviroment into the newArgv array to be processed correctly.   
   
*/

void parse()
{

char wordArray[MAXCHARACTERS]; //This is sent to getword to use as a buffer for the words.
int wordSize = 0; //This holds the length of the word.
int index = 0; //This holds the index of the array.
int pointerToWord = 0; //This is the pointer address where the words will point to.
int delimiterlen = 0; //This holds the length of the delimiter in heredoc cases.

 /* START: Reset flags & counters */
 newArgc = 0; //resets the newArgc counter.
 cdFlag = 0; //resets the cdFlag.
 lessthanFlag = 0; //resets the lessthanFlag.
 greaterthanFlag = 0; //resets the greaterthanFlag
 pipeFlag = 0; //resets the pipeFlag.
 lastampFlag = 0; //resets the lastampFlag.
 inputfilename = ""; //resets the input file name holder for input redirection
 outputfilename = ""; //resets the output file name holder for output redirection 
 environFlag = 0; //resets the environFlag.
 dollarSignFlag = 0; //resets the dollarSignFlag.
 invalidEvFlag = 0; //resets the invalidEvFlag.
 tildeFlag = 0; //resets the tildeFlag.
 tildeErrorFlag = 0; //resets the tildeErrorFlag.
 hereDocFlag = 0; //resets the hereDocFlag.
 invalidAmpOutputRedirectionFlag = 0; //resets the invalidAmpOutputRedirectionFlag
 /* END: end of section  */ 
 
 while( (wordSize = getword(wordArray + pointerToWord) ) != newlineFlag) //This loop keeps getting words until a newline or dollar sign is found.
 {
   /*START: This section handles words that are returned with a negative */
   if(wordSize == -( abs(wordSize) ) && (wordSize != newEOF) ) //This checks if getword returned a negative word size. 
   {
     wordSize  = abs(wordSize); //remove the negative word size.
     dollarSignFlag = 1; //sets a flag indicating a "$" was found in front of a word. 
   } 
   /* END: end of section. */
   
   /* START: This section places non-methcharacters into newArgv */
   if( (strcmp(wordArray + pointerToWord, "<") != 0) && (strcmp(wordArray + pointerToWord, ">") != 0) && (strcmp(wordArray + pointerToWord, "|") != 0) && (lastampFlag == 0)                                     && (strcmp(wordArray + pointerToWord, "cd") != 0) && (strcmp(wordArray + pointerToWord, "environ") != 0) && (strcmp(wordArray + pointerToWord, "<<") != 0)                                                    && (strcmp(wordArray + pointerToWord, "-255") != 0) && (dollarSignFlag == 0) && (tildeFlag == 0) && (wordSize != newEOF ) ) //places all non-metacharacter words in newArgv. 
   {
     newArgv[index] = &wordArray[pointerToWord]; // Puts the address of non-metacharacters into newArgv.
     pointerToWord = pointerToWord + wordSize + 1; //Moves to a new pointer address for the next word. 
     index++; // Moves the index. 
     newArgc++; //increases the number of agruments in newArgc by one.
     continue; 
   } //END: end of section.  
  
   /*START: This section sets up input redirection */
   if( (strcmp(wordArray + pointerToWord, "<") == 0) ) //Handles when a "<" is found
   {
     pointerToWord = pointerToWord + wordSize + 1; //Moves to a new pointer address for the next word.
     wordSize = getword(wordArray + pointerToWord); //Calls getword to get the input filename
     
     if(wordSize == newlineFlag) //checks if an input file name was typed.
     {
        lessthanFlag = 1;
        break; 
     }
     
     inputfilename = (wordArray + pointerToWord); //Stores the inputfile name in the variable. 
     pointerToWord = pointerToWord + wordSize + 1; //Moves to a new pointer address for the next word.
     lessthanFlag++;
     continue;
   } /*END: end of section. */
  
   /*START: This section sets up output redirection */
   if( (strcmp(wordArray + pointerToWord, ">") == 0) ) //Handles when a ">" is found
   {
     pointerToWord = pointerToWord + wordSize + 1; //Moves to a new pointer address for the next word.
     wordSize = getword(wordArray + pointerToWord); //Calls getword to get the output filename
     
     if(lastampFlag == 1) //checks to see if an "&" was used an outputfile name when it was the last thing on the commandline 
     { 
       invalidAmpOutputRedirectionFlag = 1; //sets a flag letting us know the user tried to use an "&" as a outputfilename when it was the last thing on the commandline.
       break; 
     }
     
     if(wordSize == -( abs(wordSize) ) && (wordSize != newEOF) ) //This checks if the name of the output file was an enviroment variable 
     {
       wordSize  = abs(wordSize); //remove the negative word size.
       if(getenv(wordArray + pointerToWord) != NULL) //This calls getenv and checks if the given enviroment variable is valid. 
       {
         outputfilename = getenv(wordArray + pointerToWord); //Put the path to the enviroment variable in outfilename.
         pointerToWord = pointerToWord + wordSize + 1; //Moves to a new pointer address for the next word. 
         greaterthanFlag++;
         continue; 
       }
       else //if getenv returns null.
       {
         fprintf(stderr, "Error: [%s] is not a valid enviroment variable for output redirection. Please try again.\n", wordArray + pointerToWord); //prints error message if getenv fails
         invalidEvFlag = 1; //sets the flag one indaicating that an invaild enviroment variable was typed by the user.
         continue;
       } 
       
     }
     outputfilename = (wordArray + pointerToWord); //Stores the outputfile name in the variable. 
     pointerToWord = pointerToWord + wordSize + 1; //Moves to a new pointer address for the next word.
     greaterthanFlag++;
     
   } /*END: end of section. */
   
   /*START: This section sets up pipe redirection */
   if( (strcmp(wordArray + pointerToWord, "|") == 0) ) //Handles when a "|" is found
   { 
     newArgv[index] = NULL; //puts a NULL in place of the "|" character 
     pointerToWord = pointerToWord + wordSize + 1; //Moves to a new pointer address for the next word.
     index++;             
     lastSetOfPipes = pipeFlag * 2; //Gives us the index of the last set of pipe file descriptors.
     pipeFlag++;
     lastPipeCmdIndex = newArgc + pipeFlag; //Tells us the index of last pipe command.
     totalPipeFds = pipeFlag * 2; //Tells us how pipe file descriptors we need. 
     pipemiddleguys = pipeFlag -1; //Tells us how many "middle guys" we need for pipe commands.
   } /*END: end of section. */ 
   
   if(  (wordSize == newlineFlag) &&  ( (lessthanFlag == 1) || (greaterthanFlag == 1) )  ) //The reason this is here is because when you have a "<" or ">" character, but a filename was not typed, getword      returns a zero. //This makes sure the next line  will be put into newArgv correctly.                                                                                 
   { 
      break;
   }
   
   /*START: This section sets up the built-in "cd" function */
   if( (strcmp(wordArray + pointerToWord, "cd") == 0) && (newArgc == 0) ) //checks to see if "cd" was the first word on the line.
   {
     newArgv[index] = &wordArray[pointerToWord]; // Puts the address "cd" into newArgv.
     pointerToWord = pointerToWord + wordSize + 1; //Moves to a new pointer address for the next word. 
     index++; // Moves the index. 
     newArgc++; //increases the number of agruments in newArgc by one.     
     cdFlag = 1;
     cdWasTyped = 1; //This lets us to know print the correct prompt to the user if they typed cd at least once.
   } /*END: end of section. */
   
   /*START: This section handles when "cd" is found in the middle of the line */
   if( (strcmp(wordArray + pointerToWord, "cd") == 0) && (newArgc > 0) ) //checks to see if "cd" was found in the middle of the line
   {
     newArgv[index] = &wordArray[pointerToWord]; // Puts the address "cd" into newArgv.
     pointerToWord = pointerToWord + wordSize + 1; //Moves to a new pointer address for the next word. 
     index++; // Moves the index. 
     newArgc++; //increases the number of agruments in newArgc by one.     
   } /*END: end of section. */
   
   /*START: This section sets up the built-in "environ" function */
   if( (strcmp(wordArray + pointerToWord, "environ") == 0) && (newArgc == 0) ) //checks to see if "environ" was the first word on the line.
   {
     newArgv[index] = &wordArray[pointerToWord]; // Puts the address "environ" into newArgv. 
     pointerToWord = pointerToWord + wordSize + 1; //Moves to a new pointer address for the next word. 
     index++; // Moves the index. 
     environFlag = 1; //Sets a flag so we know we have a "environ" case to deal with in main().
   } /*END: end of section. */
   
   /*START: This section handles when "environ" is found in the middle of the line */
   if( (strcmp(wordArray + pointerToWord, "environ") == 0) && (newArgc > 0) ) //checks to see if "cd" was found in the middle of the line
   {
     newArgv[index] = &wordArray[pointerToWord]; // Puts the address "environ" into newArgv.
     pointerToWord = pointerToWord + wordSize + 1; //Moves to a new pointer address for the next word. 
     index++; // Moves the index. 
     newArgc++; //increases the number of agruments in newArgc by one.     
   } /*END: end of section. */
   
   /*START: This section handles when a dollar sign is found in front of a word */
   if(dollarSignFlag == 1)  //When a dollar sign is in front of a word.
   {
      if(getenv(wordArray + pointerToWord) != NULL) //This calls getenv and checks if the given enviroment variable is valid. 
       {
         newArgv[index] = getenv(wordArray + pointerToWord); // Puts the address of the enviroment variable into newArgv.
         pointerToWord = pointerToWord + wordSize + 1; //Moves to a new pointer address for the next word. 
         index++; // Moves the index. 
         newArgc++; //increases the number of agruments in newArgc by one.  
         dollarSignFlag = 0; //Resets the flag to deal with the next word or words.
         continue; 
       }
       else //if getenv returns null.
       {
         fprintf(stderr, "Error: [%s] is not a valid enviroment variable. Please try again.\n", wordArray + pointerToWord); //prints error message if getenv fails
         invalidEvFlag = 1; //sets the flag one indaicating that an invaild enviroment variable was typed by the user.
         continue;
       } 
   } /*END: end of section. */
   
   /*START: This section handles when a tilde is the second word on the line followed by other characters. Ex. echo ~cs570/Data2 */
   if(tildeFlag == 1) 
   {
     char *lineReadFromFile = ""; //The buffer that the line we read from file gets stored in.
     char *finalString = ""; //This holds the final string that goes into the newArgv array
     size_t lineSize = 0; //This is the the size of the lineReadFromFile buffer
     ssize_t linesize = 0; //Holds the lenth of the line returned by getline
     
     char *firstPiece = ""; //Holds the username we're looking for.
        
     char *searchFor = strtok(wordArray + pointerToWord, "/"); //This takes the input string and saves the token we need to get the correct line from the passwd file.
     char *lastPiece = strtok(NULL, "/"); //This saves the other part of the input string. 
     char *rv = ""; //This points to the return value of strstr().
   
    
     FILE *fp = fopen("/etc/passwd", "r"); //This opens the passwd file so we can find the line we need.
 
     while((int)linesize != EOF)  //This loop searches each line read-in from the file using the token to find the correct line. (typecast to int to avoid splint warning)
     {
       linesize = getline(&lineReadFromFile, &lineSize, fp); //gets each line from the passwd file.
       rv = strstr(lineReadFromFile, searchFor); //checks if the line might match the username were looking for
       
        if(rv != NULL) //checks if the line we found is the right one
        {
          firstPiece =  strtok(lineReadFromFile, ":"); //gets the first word of the line we found
          
          if(strcmp(searchFor, firstPiece) != 0) //If the current line doesn't have the username were looking for keep looking at other lines in the passwd file 
          { 
            continue; 
          }
          
          if(strcmp(searchFor, firstPiece) == 0) //when we find the line with the correct username breaks out of the loop.
          { 
            break; 
          }
          
        } 
     }
     
     fclose(fp); //closes the passwd file after we have found the line we needed.
     
     
     if((int)linesize == EOF)  //checks if the user name given was vaild. (typecast to int to avoid splint warning)
     {
       fprintf(stderr, "Error: [%s] is a unknown user. Please try again.\n", searchFor); //prints error message if username was not valid.
       tildeErrorFlag = 1; //Turn of flag indicating error occured.
       tildeFlag = 0; //Turn off the flag because we are finished dealing with the tilde "word"
       continue;
     }
 
     while(firstPiece != NULL) //Loop through the rest of the words on the line.
     {
       if( (strncmp(firstPiece, "/", 1) == 0) ) //checks each word to if its the piece we need. 
       { 
         break;
       } 
       firstPiece = strtok(NULL, ":"); //get the rest of the words in the line. 
     }
 
     finalString = firstPiece; //Take the first piece of the string and store it in finalString.
  
     if(lastPiece != NULL) //Checks if there was another piece to the input string.
     { 
       strcat(finalString, "/"); //Place a "/" in between the first and second pieces of the new final string.
       strcat(finalString, lastPiece); //Place the final piece of the new string in finalString. 
     }
     
     newArgv[index] = finalString; // Puts the new string in the newArgv array.
     pointerToWord = pointerToWord + wordSize + 1; //Moves to a new pointer address for the next word. 
     index++; // Moves the index. 
     newArgc++; //increases the number of agruments in newArgc by one. 
     tildeFlag = 0; //reset the flag after we are done handling the username lookup. 
     continue;
   } /*END: end of section. */
   
   /*START: This section handles when << (here document) is found */
   if( (strcmp(wordArray + pointerToWord, "<<") == 0) ) //Handles when a "<<" is found.
   {
     pointerToWord = pointerToWord + wordSize + 1; //Moves to a new pointer address for the next word.
     wordSize = getword(wordArray + pointerToWord); //Calls getword to get the delimiter.
     if(wordSize == newlineFlag) //If no delimter was typed set error flag.
     {
        delimiter = ""; //reset the delimiter to blank.
        hereDocFlag = 1; //set the error flag so we can print error message in main()
        break;
     }
     delimiter = (wordArray + pointerToWord); //Saves the delimiter.
     pointerToWord = pointerToWord + wordSize + 1; //Moves to a new pointer address for the next word.
     delimiterlen = wordSize + 1;  //get the length of delimiter 
     hereDocFlag = 1;  //set flag so we know we have to deal with a hereis doc situation in main().
     continue;
   } /*END: end of section. */
     
   /*START: This section handles when -255 is the first "word" found */
   if( (strcmp(wordArray + pointerToWord, "-255") == 0) ) //when -255 is the first "word" found
   {
     eofFlag = 1;
     break;
   } /*END: end of section. */
   
   /*START: This section handles when the actual number -255 is returned*/                                                                                    
   if(wordSize == newEOF) //included because sometimes instead of typing the commands I just feed p2 with commands from a file, so when we reach EOF in getword.c, and the actual number -255 is returned                              //this will handle it.
   {
     eofFlag = 1;
     break;
   }
   /*END: end of section. */

 }// End of while loop. 
 
  if(wordSize == newlineFlag) //Handles when a newline is found
  { 
    newArgv[index] = NULL; //places a NULL at the end of the newArgv array.
  }
  
  /* START: This section handles the heredoc input*/
  if( (hereDocFlag == 1) && (newArgc > 0) && (lessthanFlag == 0) && (delimiterlen > 0) ) //This checks if there a here Document command was typed now we gather input.
   {
     FILE *tempFilePath = fopen(heredocumenttempfile, "w"); //This opens the temp file for writing.
     size_t sizeOfBuffer = 0; //the size of the buffer getline uses.
     int foundDelimiter = 0; //A flag to let us know when the delimiter has been found in heredocument commands.
     char *inputFromKeyboard = ""; //This saves the line readin from the user 
     ssize_t linesize = 0; //This holds the length of line returned by getline().

     while(foundDelimiter != 1) //keep reading input from the keyboard and write the input to the temp file until delimiter is found.
     {
       linesize = getline(&inputFromKeyboard, &sizeOfBuffer, stdin); //reading input from the keyboard.
       
       if( (strstr(inputFromKeyboard, delimiter) )  != NULL && (delimiterlen == (int)linesize) ) //when we find the delimiter stop getting input.
       {  
         foundDelimiter = 1; //Set foundDelimiter to 1 to indicate the delimiter was found.
         continue;
       } 
       fputs(inputFromKeyboard, tempFilePath); //write input from stdin to temp file 
     } 
     fclose(tempFilePath); //close temp file after we are done writting to it.
   } /*END: end of section. */

} /*END: end of parse function */
