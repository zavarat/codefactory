#include <sys/mman.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>

typedef struct{
  char name[4];
  int age;
}people;

int main( int argc, char** argv)
{
  int i;
  people *p_map;
  char temp;
  p_map = (people*)mmap(NULL,sizeof(people)*10,PROT_READ|PROT_WRITE,MAP_SHARED|MAP_ANONYMOUS,-1,0);

  if(0 == fork())
  {
    sleep(2);
    for(i=0;i<5;i++)
     printf("child read: the %d people'age is %d\n",i+1,(*(p_map+i)).age);
    (*p_map).age = 100;
   munmap(p_map,sizeof(people)*10);
   exit(0);
  }
  temp = 'a';
  for(i=0;i<5;i++)
  {
    temp += 1;
    memcpy((*(p_map+i)).name,&temp,2);
    (*(p_map+i)).age=20+i;
  }
  sleep(5);
  printf("parent read:the first people's age is %d\n",(*p_map).age );
  printf("umap\n");
  munmap( p_map,sizeof(people)*10);
  printf( "unmap ok\n" );
}

/*
liwei@Suse-11-bdsoft-0:~/test/mmap> ./relative_map 
child read: the 1 people'age is 20
child read: the 2 people'age is 21
child read: the 3 people'age is 22
child read: the 4 people'age is 23
child read: the 5 people'age is 24
parent read:the first people's age is 100
umap
unmap ok
*/