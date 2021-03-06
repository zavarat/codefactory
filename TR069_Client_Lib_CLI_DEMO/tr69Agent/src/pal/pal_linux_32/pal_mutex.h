#ifndef _PAL_MUTEX_H_
#define _PAL_MUTEX_H_

#include "pal_types.h"
#include <pthread.h>

typedef   pthread_mutex_t PAL_MUTEX_T;

extern PAL_INT32_T pal_mutex_init ( PAL_MUTEX_T *mutex );
extern PAL_INT32_T pal_mutex_lock ( PAL_MUTEX_T *mutex );
extern PAL_INT32_T pal_mutex_unlock ( PAL_MUTEX_T *mutex );
extern PAL_INT32_T pal_mutex_destroy ( PAL_MUTEX_T *mutex );

#endif /* _PAL_MUTEX_H_ */

