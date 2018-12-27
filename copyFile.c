
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <fcntl.h>
#include <stdlib.h>
#include <aio.h>
#include <errno.h>
#include <pthread.h>
#include <sys/stat.h>
#include <stdbool.h>
//#include <aiocb.h> // Asynchronous I/O control block.
#define sizeOfFileByte 100
#define sourceFile "source.txt"
#define destinationFile "destination.txt"
// Global variable.
int BUF_SIZE = 0;
// Count for thread number.
int countThreadNumber = 0;
// Control devide file size and thread number(for extra number).
bool checkDivision = false;
// Keep extra division number.
int extraDivisionNumber = 0;
// Control to enter creation for last thread.
bool enter = false;
// Define thread number take from user.
int threadNumber = 0;

// Make source file.
void createSourceFile(char source[50]);
void *readAndWrite(void *ptr);

struct thread_args
{
    char *sourceFilename;
    char *destinationFilename;
    int fileDescriptorForSource;
    int fileDescriptorForDestination;
};

struct thread_args Structthread;

int main(int argc, char *argv[])
{
    if (argc - 1 == 3)
    {
        char *num = argv[3];
        threadNumber = atoi(num);
        if ((sizeOfFileByte % threadNumber) != 0)
        {
            checkDivision = true;
            extraDivisionNumber = sizeOfFileByte % threadNumber;
        }
        BUF_SIZE = sizeOfFileByte / threadNumber;
        printf("Thread Number: %d\n", threadNumber);
    }
    char *str = argv[1];
    // Control dash for source file.
    int resultSourceDash = strcmp(str, "-");

    if (resultSourceDash == 0)
    {
        str = "./source.txt";
    }
    else
    {
        // For source file path.
        strcat(str, "/");
        strcat(str, sourceFile);
    }
    char *str2 = argv[2];
    // Control dash for destination file.
    int resultDestinationDash = strcmp(str2, "-");

    if (resultDestinationDash == 0)
    {
        str2 = "./destination.txt";
    }
    else
    {
        // For destination file path.
        strcat(str2, "/");
        strcat(str2, destinationFile);
    }
    // Call source file create function.
    createSourceFile(str);
    /***********************************************************/

    Structthread.sourceFilename = str;
    Structthread.destinationFilename = str2;
    int fd = open(str, O_RDONLY);
    Structthread.fileDescriptorForSource = fd;
    int fd2 = open(str2, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
    Structthread.fileDescriptorForDestination = fd2;

    if (fd == -1)
    {
        perror("open");
        exit(0);
    }
    if (fd2 == -1)
    {
        perror("open");
        exit(0);
    }

    pthread_t *threads;

    threads = (pthread_t *)malloc(threadNumber * sizeof(pthread_t));

    for (int j = 0; j < threadNumber; j++)
    {

        if ((j == threadNumber - 1) && checkDivision == 1)
        {
            enter = true;
            BUF_SIZE = BUF_SIZE + extraDivisionNumber;
        }
        pthread_create(threads + j, NULL, readAndWrite, (void *)&Structthread);
        pthread_join(*(threads + j), NULL);
        countThreadNumber++;
    }
    pthread_exit(NULL);

    return (0);
}
void createSourceFile(char source[50])
{

    // 1 MB = 1048576 byte.
    // Control max size of file 100 MB.
    if (sizeOfFileByte > 100 * 1048576)
    {
        exit(EXIT_FAILURE);
    }
    FILE *fp;
    fp = fopen(source, "a");
    srand(time(NULL));
    for (int i = 0; i < sizeOfFileByte; i++)
    {
        char theChar = ' ';
        int theInt = rand() % (90 + 1 - 65) + 65;
        theChar = (char)theInt;
        fprintf(fp, "%c", theChar);
    }
    fclose(fp);
}
void *readAndWrite(void *ptr)
{

    char *source = Structthread.sourceFilename;
    char *destination = Structthread.destinationFilename;

    int fd = Structthread.fileDescriptorForSource;
    int fd2 = Structthread.fileDescriptorForDestination;
    int err, ret, pidwait, child_status;
    char data[BUF_SIZE];
    struct aiocb aio_r, aio_w; // define aio control blocks

    int position = BUF_SIZE * countThreadNumber;

    if (enter == true)
    {
        position = sizeOfFileByte - BUF_SIZE;
    }
    lseek(fd, position, SEEK_SET);
    memset(&aio_r, 0, sizeof(aio_r)); // memset() is used to fill a block of memory with a particular value.
    memset(&data, 0, sizeof(data));
    aio_r.aio_fildes = fd;
    aio_r.aio_offset = position;
    aio_r.aio_buf = data;
    aio_r.aio_nbytes = sizeof(data);
    aio_r.aio_lio_opcode = LIO_READ; // aio_lio_opcode: The operation to be performed; enum { LIO_READ, LIO_WRITE, LIO_NOP };

    if (aio_read(&aio_r) == -1)
    {
        printf("Error at aio_read(): %s\n",
               strerror(errno));
        exit(2); // terminates the calling process. Any open file descriptors belonging to the process are closed.
    }
    while (aio_error(&aio_r) == EINPROGRESS)
    {
    }
    err = aio_error(&aio_r);
    ret = aio_return(&aio_r);

    if (err != 0)
    {
        printf("Error at aio_error() : %s\n", strerror(err));
        close(fd);
        exit(2);
    }

    if (ret != BUF_SIZE)
    {
        printf("Error at aio_return()\n");
        close(fd);
        exit(2);
    }
    printf("Data: %s\n", (char *)aio_r.aio_buf);
    //printf("Reading is finished\n");
    /**********************/
    lseek(fd2, position, SEEK_SET);
    memset(&aio_w, 0, sizeof(aio_w));
    aio_w.aio_fildes = fd2;
    aio_w.aio_offset = position;
    aio_w.aio_buf = (char *)aio_r.aio_buf;
    aio_w.aio_nbytes = sizeof(data);
    aio_w.aio_sigevent.sigev_notify = SIGEV_NONE;

    if (aio_write(&aio_w) == -1)
    {
        printf(" Error at aio_write(): %s\n", strerror(errno));
        close(fd2);
        exit(2);
    }
    while (aio_error(&aio_w) == EINPROGRESS)
    {
    }
    err = aio_error(&aio_w);
    ret = aio_return(&aio_w);

    if (err != 0)
    {
        printf("Error at aio_error() : %s\n", strerror(err));
        close(fd2);
        exit(2);
    }
    if (ret != sizeof(data))
    {
        printf("Error at aio_return()\n");
        close(fd2);
        exit(2);
    }
    printf("Writing is finished\n");
}
