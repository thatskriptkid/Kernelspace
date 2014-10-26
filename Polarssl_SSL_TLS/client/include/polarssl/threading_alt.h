/*
 * Redefine Polarssl threading_mutex_t type
 * 
*/

#if defined(POLARSLL_LINUX_KERNEL)
#include <linux/mutex.h>

#endif
typedef struct mutex threading_mutex_t;

