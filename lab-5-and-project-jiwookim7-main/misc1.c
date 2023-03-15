/************* misc1.c file **************/

#ifndef __MISC1_C__
#define __MICS1_C__

#include "write_cp.c"

int menu()
{
    printf("\t\t******************** Menu *******************\n");
    printf("\t\tmkdir\tcreat\tmount\tumount\trmdir\n");
    printf("\t\tcd\tls\tpwd\tstat\trm\n");
    printf("\t\tlink\tunlink\tsymlink\tchmod\tchown\ntouch\n");
    printf("\t\topen\tpfd\tlseek\tclose\n");
    printf("\t\tread\twrite\tcat\tcp\tmv\n");
    printf("\t\tcs\tfork\tps\tkill\tquit\n");
    printf("\t\t=============   Usage Examples ==============\n");
    printf("\t\tmkdir\tfilename\n");
    printf("\t\tmount\tfilesys\t/mnt\n");
    printf("\t\tchmod\tfilename\t0644\n");
    printf("\t\topen\tfilename\tmode (0|1|2|3 for R|W|RW|AP)\n");
    printf("\t\twrite\tfd\ttext_string\n");
    printf("\t\tread\tfd\tnbytes\n");
    printf("\t\tpfd\t(display opened file descriptors)\n");
    printf("\t\tcs\t(switch process)\n");
    printf("\t\tfork\t(fork child process)\n");
    printf("\t\tps\t(show process queue as Pi[uid]==>)\n");
    printf("\t\tkill\tpid\t(kill a process)\n");
    printf("\t\t*********************************************\n");
}

int my_stat(char *pathname)
{
    struct stat myst;

    return 1;
}

int my_chmod(char *pathname, int mode)
{
    return 1;
}

int my_utime(char *pathname)
{
    return 1;
}

int my_chown(char *pathname, int uid)
{
    return 1;
}

int cs()
{
  return 1;
}

int fork()
{
  return 1;
}

int ps()
{
  return 1;
}

int kill(int pid)
{
  return 1;
}

int my_mv(char *pathname)
{
  return 1;
}

#endif