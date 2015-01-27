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

//needed constants
 //mem size for prompt string
#define PROMPT_SIZE PATH_MAX + 2

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
	  printf("%d ", (int)statp.st_size);
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
