/*
 * Named semaphore server
 *
 */

#include <mach/mach.h>
#include <mach/semaphore.h>

#include <map>
#include <string>

extern "C" {
#include "semsrv_defs.h"
}

typedef std::map<std::string,semaphore_t> semaphore_map_t;
static semaphore_map_t semaphores;

static mach_port_t server_port;

extern "C" boolean_t semsrv_server(mach_msg_header_t *inhdr,
                                   mach_msg_header_t *outhdr);

static void
server_setup (void)
{
  kern_return_t ret;
  const char *srvname = SEMSRV_SERVICE;

  ret = bootstrap_check_in (bootstrap_port, (char *)srvname,
                            &server_port);
  if (ret != BOOTSTRAP_SUCCESS) {
    mach_port_deallocate (mach_task_self(), server_port);
    mach_error ("bootstrap_check_in:", ret);
    exit (1);
  }
}

static void
server_loop (void)
{
  mach_msg_server (semsrv_server,
                   512,
                   server_port,
                   MACH_MSG_TIMEOUT_NONE);
}

extern "C" kern_return_t
semsrv_create_semaphore (mach_port_t server,
                         sem_name_t name,
                         int value,
                         semaphore_t *semaphore,
                         boolean_t *created)
{
#pragma unused(server)
  semaphore_map_t::const_iterator it = semaphores.find(name);

  *semaphore = MACH_PORT_NULL;
  *created = FALSE;

  if (it == semaphores.end()) {
    semaphore_t sem;
    kern_return_t ret;
    
    ret = semaphore_create (mach_task_self(),
                            &sem,
                            SYNC_POLICY_FIFO,
                            value);

    if (ret == KERN_SUCCESS) {
      semaphores[name] = sem;
      *semaphore = sem;
      *created = TRUE;
    }
    
    return ret;
  } else {
    *semaphore = it->second;
    *created = FALSE;

    return SEMSRV_SUCCESS;
  }
}

extern "C" kern_return_t
semsrv_get_semaphore (mach_port_t server,
                      sem_name_t name,
                      semaphore_t *semaphore)
{
#pragma unused(server)
  semaphore_map_t::const_iterator it = semaphores.find(name);

  if (it == semaphores.end())
    return SEMSRV_NOT_FOUND;

  *semaphore = it->second;
  return SEMSRV_SUCCESS;
}

extern "C" kern_return_t
semsrv_destroy_semaphore (mach_port_t server,
                          sem_name_t name)
{
#pragma unused(server)
  semaphore_map_t::iterator it = semaphores.find(name);

  if (it == semaphores.end())
    return SEMSRV_NOT_FOUND;

  semaphore_t sem = it->second;
  semaphores.erase (it);

  return semaphore_destroy (mach_task_self(), sem);
}

int
main (void)
{
  server_setup ();
  server_loop ();
  return 0;
}
