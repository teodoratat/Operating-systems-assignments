#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "a2_helper.h"
#include <stdlib.h>
#include <pthread.h>
/*void thread1(void *arg)
{
    info(BEGIN, 2, 1);

    info(END, 2, 1);
}
void thread2(void *arg)
{
    info(BEGIN, 2, 2);

    info(END, 2, 2);
}
void thread3(void *arg)
{
    info(BEGIN, 2, 3);

    info(END, 2, 3);
}
void thread4(void *arg)
{
    info(BEGIN, 2, 4);

    info(END, 2, 4);
}
void thread5(void *arg)
{
    info(BEGIN, 2, 5);

    info(END, 2, 5);
}
void create_thread(pthread_t threads[5])
{
    // pthread_t threads[5];
    pthread_create(&threads[0], NULL, (void *)thread1, NULL);
    pthread_create(&threads[1], NULL, (void *)thread1, NULL);
    pthread_create(&threads[2], NULL, (void *)thread1, NULL);
    pthread_create(&threads[3], NULL, (void *)thread1, NULL);
    pthread_create(&threads[4], NULL, (void *)thread1, NULL);
    pthread_join(threads[0], 0);
    pthread_join(threads[1], 0);
    pthread_join(threads[2], 0);
    pthread_join(threads[3], 0);
    pthread_join(threads[4], 0);
}
void threads()
{
}
*/
int main()
{
    init();
    //pthread_t threads[5];
    pid_t id[7];
    info(BEGIN, 1, 0);
    id[0] = fork(); // p2
    if (id[0] < 0)
    { // if not, terminate the parent too
        perror("Error creating new process");
        exit(1);
    }
    if (id[0] == 0)
    {
        info(BEGIN, 2, 0);
        //create_thread(threads);
        info(END, 2, 0);
    }
    else
    {
        id[1] = fork(); // p3
        if (id[1] < 0)
        { // if not, terminate the parent too
            perror("Error creating new process");
            exit(2);
        }
        if (id[1] == 0)
        {
            info(BEGIN, 3, 0);
            id[2] = fork(); // p6
            if (id[2] < 0)
            { // if not, terminate the parent too
                perror("Error creating new process");
                exit(3);
            }
            if (id[2] == 0)
            {
                info(BEGIN, 6, 0);
                info(END, 6, 0);
            }
            else
            {
                id[3] = fork(); // p7
                if (id[3] < 0)
                {
                    perror("Error creating new process");
                    exit(4);
                }
                if (id[3] == 0)
                {
                    info(BEGIN, 7, 0);
                    info(END, 7, 0);
                }
                else
                {
                    waitpid(id[2], NULL, 0);
                    waitpid(id[3], NULL, 0);
                    info(END, 3, 0);
                }
            }
        }
        else
        {
            id[4] = fork(); // p4
            if (id[4] < 0)
            {
                perror("Error creating new process");
                exit(5);
            }
            if (id[4] == 0)
            {
                info(BEGIN, 4, 0);
                info(END, 4, 0);
            }
            else
            {
                id[5] = fork(); // p5
                if (id[5] < 0)
                {
                    perror("Error creating new process");
                    exit(6);
                }
                if (id[5] == 0)
                {
                    info(BEGIN, 5, 0);
                    info(END, 5, 0);
                }
                else
                {
                    waitpid(id[0], NULL, 0);
                    waitpid(id[1], NULL, 0);
                    waitpid(id[4], NULL, 0);
                    waitpid(id[5], NULL, 0);
                    info(END, 1, 0);
                }
            }
        }
    }
    return 0;
}
