#include <sys/socket.h>
#include <sys/un.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <errno.h>
//#include "conflib.h"
#include "tcm_msg.h"
//#include <tpciserv.h>
#define SCHED_MESSAGE_ADDR "/home/nick/etc/sched_msg_svr_addr"

/** Number of micro-seconds in 1 second. */
#define USECS_IN_SEC  1000000

/** Number of micro-seconds in a milli-second. */
#define USECS_IN_MSEC 1000

/** Number of milliseconds in 1 second */
#define MSECS_IN_SEC  1000

tcmRet oalMsg_send(int fd, const tcmMsgHeader *buf);

tcmRet oalMsg_init(tcmEntityId eid, void **msgHandle)
{
   tcmMsgHandle *handle;
   const tcmEntityInfo *eInfo;
   struct sockaddr_un serverAddr;
   int rc;

   if( (eInfo = tcmEid_getEntityInfo(eid)) == NULL)
   {
      return TCMRET_INVALID_PARAMETER;
   }

   if( ( handle = malloc(sizeof(tcmMsgHandle))) == NULL )
   {
      printf( "handle = malloc(sizeof(tcmMsgHandle)))\n" );
      return TCMRET_INTERNAL_ERROR;
   }

   handle->eid = eid;

   //Socket Resource Creation
   handle->commFd = socket(AF_LOCAL, SOCK_STREAM, 0);
   if (handle->commFd < 0)
   {
      printf( "handle->commFd < 0\n" );
      free(handle);
      return TCMRET_INTERNAL_ERROR;
   }

   /*
    * Set close-on-exec, even though all apps should close their
    * fd's before fork and exec.
    */
   if ((rc = fcntl(handle->commFd, F_SETFD, FD_CLOEXEC)) != 0)
   {
      printf( "rc = fcntl(handle->commFd, F_SETFD, FD_CLOEXEC))\n" );
      close(handle->commFd);
      free(handle);
      return TCMRET_INTERNAL_ERROR;
   }

   /*
    * Connect to .
    * sched app  .
    */
   memset(&serverAddr, 0, sizeof(serverAddr));
   serverAddr.sun_family = AF_LOCAL;
   strncpy(serverAddr.sun_path, SCHED_MESSAGE_ADDR, sizeof(serverAddr.sun_path));

   rc = connect(handle->commFd, (struct sockaddr *) &serverAddr, sizeof(serverAddr));
   if (rc != 0)
   {
      printf( "rc != 0\n" );
      close(handle->commFd);
      free(handle);
      return TCMRET_INTERNAL_ERROR;
   }

   /* send a launched message to sched */
   {
      tcmRet ret;
      tcmMsgHeader launchMsg = EMPTY_MSG_HEADER;

      launchMsg.type = TCM_MSG_APP_LAUNCHED;
      launchMsg.src = (eInfo->flags & APPCFG_MULTIPLE_INSTANCES ) ? MAKE_SPECIFIC_EID(getpid(), eid):eid;
      launchMsg.dst = ID_SCHED;
      launchMsg.flags_event = 1;

      if ((ret = oalMsg_send(handle->commFd, &launchMsg)) != TCMRET_SUCCESS)
      {
         printf( "(ret = oalMsg_send(handle->commFd, &launchMsg)) != TCMRET_SUCCESS\n" );
         close(handle->commFd);
         free(handle);
         return TCMRET_INTERNAL_ERROR;
      }
   }

   /* successful, set handle pointer */
   *msgHandle = (void *) handle;

   return TCMRET_SUCCESS;
}

void oalMsg_cleanup(void **msgHandle)
{
   tcmMsgHandle *handle = (tcmMsgHandle *) *msgHandle;

   if (handle->commFd != TCM_INVALID_FD)
   {
      close(handle->commFd);
   }

   if( msgHandle != NULL )
      free(msgHandle);

   return;
}

tcmRet oalMsg_getEventHandle(const tcmMsgHandle *msgHandle, void *eventHandle)
{
   int *fdPtr = (int *) eventHandle;

   *fdPtr = msgHandle->commFd;

   return TCMRET_SUCCESS;
}

tcmRet oalMsg_send(int fd, const tcmMsgHeader *buf)
{
   unsigned int totalLen;
   int rc;
   tcmRet ret=TCMRET_SUCCESS;

   totalLen = sizeof(tcmMsgHeader) + buf->dataLength;

   rc = write(fd, buf, totalLen);
   if (rc < 0)
   {
      if (errno == EPIPE)
      {
         /*
          * This could happen when smd tries to write to an app that
          * has exited.  Don't print out a scary error message.
          * Just return an error code and let upper layer app handle it.
          */
         return TCMRET_DISCONNECTED;
      }
      else
      {
         ret = TCMRET_INTERNAL_ERROR;
      }
   }
   else if (rc != (int) totalLen)
   {
      ret = TCMRET_INTERNAL_ERROR;
   }

   return ret;
}

static tcmRet waitForDataAvailable(int fd, unsigned int timeout)
{
   struct timeval tv;
   fd_set readFds;
   int rc;

   FD_ZERO(&readFds);
   FD_SET(fd, &readFds);

   tv.tv_sec = timeout / MSECS_IN_SEC;
   tv.tv_usec = (timeout % MSECS_IN_SEC) * USECS_IN_MSEC;

   rc = select(fd+1, &readFds, NULL, NULL, &tv);
   if ((rc == 1) && (FD_ISSET(fd, &readFds)))
   {
      return TCMRET_SUCCESS;
   }
   else
   {
      return TCMRET_TIMED_OUT;
   }
}

tcmRet oalMsg_receive(int fd, tcmMsgHeader **buf, unsigned int *timeout)
{
   tcmMsgHeader *msg;
   int rc;
   tcmRet ret;

   if (buf == NULL)
   {
      return TCMRET_INVALID_PARAMETER;
   }
   else
   {
      *buf = NULL;
   }

   if (timeout)
   {
      if ((ret = waitForDataAvailable(fd, *timeout)) != TCMRET_SUCCESS)
      {
         return ret;
      }
   }

   /*
    * Read just the header in the first read.
    * Do not try to read more because we might get part of
    * another message in the TCP socket.
    */
   msg = (tcmMsgHeader *) malloc(sizeof(tcmMsgHeader));
   if (msg == NULL)
   {
      return TCMRET_INTERNAL_ERROR;
   }

   rc = read(fd, msg, sizeof(tcmMsgHeader));
   if ((rc == 0) ||
       ((rc == -1) && (errno == 131)))  /* new 2.6.21 kernel seems to give us this before rc==0 */
   {
      /* broken connection */
      free(msg);
      return TCMRET_DISCONNECTED;
   }
   else if (rc < 0 || rc != sizeof(tcmMsgHeader))
   {
      free(msg);
      return TCMRET_INTERNAL_ERROR;
   }

   if (msg->dataLength > 0)
   {
      int totalReadSoFar=0;
      int totalRemaining=msg->dataLength;
      char *inBuf;

      /* there is additional data in the message */
      msg = (tcmMsgHeader *) realloc(msg, sizeof(tcmMsgHeader) + msg->dataLength);
      if (msg == NULL)
      {
         free(msg);
         return TCMRET_DISCONNECTED;
      }

      inBuf = (char *) (msg + 1);
      while (totalReadSoFar < msg->dataLength)
      {
         if (timeout)
         {
            if ((ret = waitForDataAvailable(fd, *timeout)) != TCMRET_SUCCESS)
            {
               free(msg);
               return ret;
            }
         }

         rc = read(fd, inBuf, totalRemaining);
         if (rc <= 0)
         {
            free(msg);
            return TCMRET_INTERNAL_ERROR;
         }
         else
         {
            inBuf += rc;
            totalReadSoFar += rc;
            totalRemaining -= rc;
         }
      }
   }

   *buf = msg;

   return TCMRET_SUCCESS;
}

