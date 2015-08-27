/**********************************************************************
* Copyright (c) 2009 Wuhan Tecom Co., Ltd.
* All Rights Reserved
* No portions of this material may be reproduced in any form without the
* written permission of:
*   Wuhan Tecom Co., Ltd.
*   #18, Huaguang Road
*   Wuhan, PR China 430074
* All information contained in this document is Wuhan Tecom Co., Ltd.
* company private, proprietary, and trade secret.
***********************************************************************/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "tcm_util.h"

void tcm_doSystemAction(const char* from, char *cmd)
{
    tcmLog_debug("%s: run [%s]", from, cmd);
    system(cmd);
}

UBOOL8 tcm_getIfSubnet(const char *ipaddr, const char *netmask, char* ipSubnet) 
{
    char *ptr;
    struct in_addr ip, mask, subnet;
    UBOOL8 ret = FALSE;

    memset(&ip, 0, sizeof(ip));
    memset(&mask, 0, sizeof(mask));
    memset(&subnet, 0, sizeof(subnet));

    if (inet_aton(ipaddr, &ip) && inet_aton(netmask, &mask)) 
    {
        subnet.s_addr = ip.s_addr & mask.s_addr;
        if ((ptr = inet_ntoa(subnet)) != NULL) 
        {
            strcpy(ipSubnet, ptr);
            ret = TRUE;
        }
    }
   
   return ret;
}

void tcm_toUpperCase(char * string)
{
        int i,len;

        len = strlen(string);

        for(i = 0; i<len;i++)
        {
            string[i] = toupper(string[i]);
        }
}

void tcm_toLowerCase(char * string)
{
        int i,len;

        len = strlen(string);

        for(i = 0; i<len;i++)
        {
            string[i] = tolower(string[i]);
        }
}
