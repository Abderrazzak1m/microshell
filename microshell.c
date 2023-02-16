#include<libc.h>
void print(char *s1, char *s2)
{
    while(*s1)
    {
        write(2, &s1[0], 1);
        s1++;
    }
    while(*s2)
    {
        write(2, &s2[0], 1);
        s2++;
    }
    write(2, "\n", 1);
}

int cont(char **av)
{
    int i = 0;
    while(av[i] && strcmp(av[i] , "|") && strcmp(av[i] , ";"))
        i++;
    return(i);
}
void exc(char **args, char **env, int i, int in)
{
    args[i] = NULL;
    dup2(in , 0);
    close(in);
    if(!execve(args[0], args, env))
        print("fatal :error","");
    print("error: cannot execute ", args[0]);
}
void func(char **av, char **env)
{
    int i = 0;
    int in = dup(0);
    int fd[2];
    while(av[i] && av[i + 1])
    {
         av = &av[i + 1];
        i = cont(av);
        if(!strcmp(av[0], "cd"))
        {
            if(i != 2)
                print("error: cd: bad arguments", "");
            else if(chdir(av[1]) != 0)
                print("error: cd: cannot change directory to ", av[1]);
        }
        else if( (i != 0 && av[i] == NULL) || (i != 0 && !strcmp(av[i], ";") ))
        {
            if(!fork())
            {
                exc(av, env, i, in);
                return ;
            }
            else
            {
                close(in);
                while(waitpid(-1, NULL, 0) != -1);
            }
            in = dup(0);
        }
        else if(i != 0 && !strcmp(av[i], "|"))
        {
            pipe(fd);
            if(!fork())
            {
                dup2(fd[1], 1);
                close(fd[1]);
                close(fd[0]);
                exc(av, env, i, in);
                return ;
            }
            else
            {
                close(fd[1]);
                close(in);
                in = fd[0];
            }
        }
    }
    close(in);
}

int main(int ac, char **av, char **env)
{
    (void)ac;
    func(av, env);
}