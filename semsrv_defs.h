/*
 * Named semaphore server
 *
 */

#ifndef SEMSRV_DEFS_H_
#define SEMSRV_DEFS_H_

#include <mach/mach.h>
#include <servers/bootstrap.h>

typedef char sem_name_t[64];

#define SEMSRV_SERVICE "net.alastairs-place.semsrv"

#define SEMSRV_SUCCESS     KERN_SUCCESS
#define SEMSRV_NOT_FOUND   (KERN_RETURN_MAX + 1)

kern_return_t semsrv_create_semaphore (mach_port_t server,
                                       sem_name_t name,
                                       int value,
                                       semaphore_t *semaphore,
                                       boolean_t *created);
kern_return_t semsrv_get_semaphore (mach_port_t server,
                                    sem_name_t name,
                                    semaphore_t *semaphore);
kern_return_t semsrv_destroy_semaphore (mach_port_t server,
                                        sem_name_t name);

#endif /* SEMSRV_DEFS_H_ */
