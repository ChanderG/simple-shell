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

//parse the input and call the corresponding function
void handleCommand(char* input){
  //buffer the same size as input, for safety 
  char* command = (char*)malloc(sizeof(input));
  sscanf(input, "%s ", command);

  if(strcmp(command,"exit") == 0){
    free(command);
    preExit(input);
  }

  //understand the command and take relevant action
  if(strcmp(command, "pwd") == 0){
    commandPwd();
  }

  free(command);
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
