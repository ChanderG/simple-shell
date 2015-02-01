//ChanderG - 12CS30011 - chandergovind@gmail.com

//An attempt at a shell
#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<limits.h>
#include<string.h>
#include<readline/readline.h>
#include<readline/history.h>
#include<error.h>

#include<dirent.h>  //for ls
#include<sys/stat.h>
#include<fcntl.h>  //for O_RDONLY flag

//needed constants
 //mem size for prompt string
#define PROMPT_SIZE PATH_MAX + 2

#define BUFFERSIZE 4096
#define COPYMODE 0644

//calculate the prompt string using current directory
void getPrompt(char prompt[]){
  if(getcwd(prompt, PROMPT_SIZE) == NULL){
    perror("Failed to get cwd path.");
    strcpy(prompt, "path/not/found");
  }
  //add decorations
  strcat(prompt, "> "); 
  return;
}

//handle clean up after a command
void postCommandHandler(char* input){
  //store input to history

  //free space
  free(input);
  return;
}  

//function to be called to "exit" the shell
void preExit(char* input){
  //execute normal command completion routine
  postCommandHandler(input);
  //game over
  exit(1);
}

//handle the command "pwd"
void commandPwd(){ 
  char* path = (char*)malloc(PATH_MAX* sizeof(char));
  if(getcwd(path, PATH_MAX) == NULL){
    perror("Failed to get cwd path.");
    free(path);
    return;
  }
  puts(path);
  free(path);
}

//handle the command "cd"
void commandCd(char** command_list){
  //needs to be cd
  if(command_list[1] == NULL){
    //handle "cd" without args
    //TODO: jump to home directory 
    printf("cd: ");
    puts("No directory specified.");
  }
  else if(-1 == chdir(command_list[1])){
    perror(command_list[1]);
  }
}

//handle the command "mkdir"
void commandMkdir(char** command_list){
  int i = 1;
  while(command_list[i] != NULL){
    if( -1 == mkdir(command_list[i], 0777)){
      perror(command_list[i]);
    }
    i++;
  }
}

//handle the command "rmdir"
void commandRmdir(char** command_list){
  int i = 1;
  while(command_list[i] != NULL){
    if( -1 == rmdir(command_list[i])){
      perror(command_list[i]);
    }
    i++;
  }
}

//handle the command "ls"
//this implementation for now only handles ls (of current directory) and ls -l of the same
//no directory specification supported
void commandLs(char** command_list){
  DIR* dirp;  //the current directory opened for reading
  if(NULL == (dirp = opendir("."))){
    perror("Error in opening directory "); 
    return;
  }

  struct dirent* direntp;

  if(command_list[1] == NULL){
    //simple ls command
    //TODO: needs better alignment when printing
    //TODO: needs colored output
    //TODO: needs ordering of names
    while(NULL != (direntp = readdir(dirp))){
      //do not display hidden files
      if(direntp->d_name[0] != '.')  
	printf("%s  ", direntp->d_name);
    }
    printf("\n");
  }
  else if(strcmp(command_list[1], "-l") == 0){
    //ls -l command
    struct stat statp;
    while(NULL != (direntp = readdir(dirp))){
      if( -1 == stat(direntp->d_name, &statp)){
        //error in running stat command 
	perror("STAT");
      }
      else{
        //was able to successfully run stat
	if(direntp->d_name[0] != '.'){  
	  //only for non-hidden files
	  //TODO: update rough hard coded sizes, eg most size fits in 5, but GB goes to 10
	  //TODO: get user name, group name from getpwnam(3)
	  printf("%2d ", (int)statp.st_nlink);
	  printf("%5d ", (int)statp.st_size);
	  printf("%s\n", direntp->d_name);
	}  
      }
    }
  }
  //handle illegal cases 

  if(-1 == closedir(dirp)){
    perror("Error in closing directory ");
  }
}

void commandCp(char** command_list){
 
  if(command_list[1] == NULL){
    printf("cp: Illegal usuage\n");
    printf("Correct syntax: cp <source file> <destination file>\n");
    return;
  }
  else if(command_list[2] == NULL){
    printf("cp: Missing destination file\n");
    printf("Correct syntax: cp <source file> <destination file>\n");
    return;
  }

  //check if file1 exists
  if(-1 == access(command_list[1], F_OK)){
    printf("cp: file \"%s\" does not exist\n", command_list[1]);
    return;
  }

  if(-1 == access(command_list[1], R_OK)){
    printf("cp: you do not have read access to file \"%s\"\n", command_list[1]);
    return;
  }

  //ready to read from source
  int fd = open(command_list[1], O_RDONLY);
  if (fd == -1){
    perror("Unable to open source file for copying: ");
    return;
  }

  if(0 == access(command_list[2], F_OK)){
    //destination file does exist
    //check time stamps 
    struct stat statps,statpd;
    if( -1 == stat(command_list[2], &statpd)){
      //error in running stat command 
      printf("Error in getting time of dest file\n");
      close(fd);
      return;
    }
    if( -1 == stat(command_list[1], &statps)){
      //error in running stat command 
      printf("Error in getting time of source file\n");
      close(fd);
      return;
    }

    //both files stat struct in hand
    //if dest file was modified later
    if(statpd.st_atime > statps.st_atime){
      printf("cp: warning! destination file modified later that source file\n");
      printf("Aborting operation!!\n");
      close(fd);
      return;
    }
  }
  
  int fd_out = creat(command_list[2],COPYMODE);
  if (fd_out == -1){
    perror("Unable to open destination file for writing: ");
    return;
  }

  int status;
  char buffer[BUFFERSIZE];
  //TODO: needs better handling if failure occurs in between
  while(1){
    status = read(fd, buffer, BUFFERSIZE);
    if(status < 0){
      perror("Error copying file");
      return;
    }
    else if(status == 0){
      break;
    }
    else{
      if(status != write(fd_out, buffer, status)){
        perror("Error writing file");
	return;
      }
    }
  } 

  close(fd);
  close(fd_out);
}

void commandGeneral(char** command_list){
  //fork a child process and execute it
  int pid;
  int background = 0;

  pid = fork();
  if(pid == -1){
    //error
    perror("Fork error");
  }
  else if(pid == 0){
    //child code
    //immidiately overwrite self
    //if last char of command is '&'
    //TODO: if & seperated by space
    if(command_list[0][strlen(command_list[0])-1] == '&'){
      command_list[0][strlen(command_list[0]) - 1] = '\0';
      background = 1;
    }  

    //normal case
    if(-1 == execvp(command_list[0], command_list)){
      //unable to exit
      //perror(command_list[0]);
      printf("%s: command not found\n", command_list[0]);
    }
  }
  else{
    //parent code 
    if(background == 1){
      //run process in background
      //TODO: find if setsid and/or closing fd 0,1 and 2 must be done
    }
    else{
     //normal case 
      if(-1 == waitpid(pid, NULL, 0)){
	perror("Error in wait from child.");
      }
    }
  }

}

//parse the input and call the corresponding function
void handleCommand(char* input){
  //buffer the same size as input, for safety 
  char* command = (char*)malloc(sizeof(input));
  sscanf(input, "%s ", command);

  if(strcmp(command,"exit") == 0){
    free(command);
    preExit(input);
  }

  //routine to parse the commandline and store each word in a seperate char array
  //the whole array is finished by a NULL pointer
  //TODO: modularize this out
  char* word;
  int no_words= 0;

  char* input_copy = strdup(input);
  word = strtok(input_copy, " ");
  while(word != NULL){
    no_words++;
    word = strtok(NULL, " ");
  }
  free(input_copy);
  char** command_list = (char**)malloc((no_words+1)*sizeof(char*)); 

  no_words = 0;
  input_copy = strdup(input);
  word = strtok(input_copy, " ");
  while(word != NULL){
    command_list[no_words] = (char*)malloc(strlen(word)*sizeof(char));
    strcpy(command_list[no_words], word);
    word = strtok(NULL, " ");
    no_words++;
  }  
  command_list[no_words] = NULL;
  free(input_copy);


  //understand the command and take relevant action
  if(strcmp(command, "pwd") == 0){
    commandPwd();
  }
  else if(strcmp(command, "cd") == 0){
    commandCd(command_list);
  }
  else if(strcmp(command, "mkdir") == 0){
    commandMkdir(command_list);
  }
  else if(strcmp(command, "rmdir") == 0){
    commandRmdir(command_list);
  }
  else if(strcmp(command, "ls") == 0){
    commandLs(command_list);
  }
  else if(strcmp(command, "cp") == 0){
    commandCp(command_list);
  }
  else{
    commandGeneral(command_list);
  }
 

  free(command);
  free(command_list);   //is this valid?
  return;
}  

int main(int argc, char** argv){
  
  char* input;                        //to store input
  char prompt[PROMPT_SIZE];           //prompt to be displayed to user

  //main loop
  while(1){
    //calculate prompt
    getPrompt(prompt);

    input = readline(prompt);
    
    //main routine 
    handleCommand(input); 

    //handle clean up
    postCommandHandler(input);
  }

  return 0;
}
