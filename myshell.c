//ChanderG - 12CS30011 - chandergovind@gmail.com

//An attempt at a shell
#include<stdio.h>
#include<stdlib.h>
#include<limits.h>
#include<string.h>
#include<readline/readline.h>
#include<readline/history.h>

//needed constants
#define PROMPT_SIZE PATH_MAX + 2

void getPrompt(char prompt[]){
  getcwd(prompt, PROMPT_SIZE);  
  strcat(prompt, "> "); 
  return;
}

int main(int argc, char** argv){
  
  //main loop
  char* input;                        //to store input
  char prompt[PROMPT_SIZE];           //prompt to be displayed to user

  while(1){
    //calculate prompt
    getPrompt(prompt);

    input = readline(prompt);
    puts("Input accepted.");
    if(strcmp(input,"exit") == 0) exit(1);
  }
  return 0;
}
