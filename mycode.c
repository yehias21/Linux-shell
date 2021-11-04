#include <unistd.h>
#include <limits.h>
//FOR Clear command
#define clear() printf("\033[H\033[J")
//pseudo hashmap
static const char * const cmd[] = {"cd","exit","pwd","clear"};
//log file
FILE *f;
//a directory print function(mimic the shell)
//int printdir()
//{
//    char cwd[PATH_MAX];
//    if (getcwd(cwd, sizeof(cwd)) != NULL) {
//        printf("%s: ", cwd);
//    } else {
//        perror("getcwd() error");
//        return 1;
//    }
//}
//getInput a function that readline
//todo:Search for a better way
int getInput(char* str)
{   int i=0;char c;
    while( scanf("%c",&c)&&c!='\n'){
        str[i]=c;
        i++;
    }
    str[i]='\0';
    if(str[0]=='\0')
        return 1;
    return 0;
}
//todo: read on pointer to pointer arithmatics and build same function but with malloc and sscanf/strtok
int parser(char* line,char** tokens) {
    int i;
    for ( i= 0; i < MAXLIST; i++) {
        tokens[i] = strsep(&line, " ");
        if (tokens[i] == NULL)
            break;
        if (strlen(tokens[i]) == 0)
            i--;
    }
    if(!strcmp(tokens[i-1],"&"))
    {  tokens[i-1]=NULL;
        return 0;
    }
    else{
        return 1;
    }
}
// a pseudo hashmap
int checkcmd(char* token)
{
    int n=sizeof(cmd)/sizeof(char*);
    for (int i = 0; i<n; i++)
        if(!strcmp(cmd[i],token))
            return i+1;
    return 0;

}
//executor helper function for non-program files
void execcmd(int type,char ** tokens)
{
    char cwd[PATH_MAX];
    switch(type){
        case 1:
            chdir(tokens[1]);
            break;
        case 2:
            exit(0);
            break;
        case 3:
            printf("%s",getcwd(cwd, sizeof(cwd)));
            break;
        case 4:
            clear();
            break;
        default:
            perror("The command is not supported!");
    }
}
//function append signal output to file
void sighandler(int signum) {
    f=fopen("logs.txt", "a");
    if(f==NULL) {
        perror("Error opening file.");}
    else{
        fprintf(f, "%s", "Child process was terminated\n");
    }
    fclose(f);
}
//the executor
void exec(char**tokens,int flag){
    int type=checkcmd(tokens[0]);
    if(type)
    {
        execcmd(type,tokens);
    }
    else{
    pid_t pid = fork();
    if (pid == -1) {
        perror("Child forking failed\n");
        return;
    } else if (pid == 0) {
        if (execvp(tokens[0], tokens) < 0)
        {
            perror("Execution failed\n");
        }
        exit(-1);
    }
    else
        if(flag)
            wait(NULL);
}}

int main()
{   char line[1000];
    char* tokens[100];
    int type;
    signal(SIGCHLD, sighandler);
    while(1)
    {
        printf("%s", "Shell> ");
        if(getInput(line))
            continue;
        type= parser(line,tokens);
        exec(tokens,type);
    }
}