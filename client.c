#include <mach/mach.h>
#include <mach/semaphore.h>
#include <stdio.h>

#include "semsrv.h"

int
main (int argc, char **argv)
{
  kern_return_t ret;
  const char *srvname = SEMSRV_SERVICE;
  mach_port_t semsrv_port;
  semaphore_t semaphore;
  boolean_t created;

  if (argc != 3) {
    fprintf (stderr, 
             "usage: semtest <create|get|destroy|signal|signal-all|wait> <name>\n");
    return 0;
  }

  ret = bootstrap_look_up (bootstrap_port, (char *)srvname, &semsrv_port);

  if (ret != BOOTSTRAP_SUCCESS) {
    mach_error ("bootstrap_look_up:", ret);
    return 1;
  }

  if (strcasecmp (argv[1], "create") == 0) {
    ret = semsrv_create_semaphore (semsrv_port,
                                   argv[2],
                                   0,
                                   &semaphore,
                                   &created);

    if (ret != SEMSRV_SUCCESS) {
      mach_error ("semsrv_create_semaphore:", ret);
      return 1;
    }

    if (created)
      printf ("Created %s: %d\n", argv[2], semaphore);
    else
      printf ("Got %s (already exists): %d\n", argv[2], semaphore);
  } else if (strcasecmp (argv[1], "get") == 0) {
    ret = semsrv_get_semaphore (semsrv_port,
                                argv[2],
                                &semaphore);

    if (ret != SEMSRV_SUCCESS) {
      mach_error ("semsrv_get_sempahore:", ret);
      return 1;
    }

    printf ("Got %s: %d\n", argv[2], semaphore);
  } else if (strcasecmp (argv[1], "destroy") == 0) { 
    ret = semsrv_destroy_semaphore (semsrv_port,
                                    argv[2]);

    if (ret != SEMSRV_SUCCESS) {
      mach_error ("semsrv_destroy_semaphore:", ret);
      return 1;
    }

    printf ("Destroyed %s\n", argv[2]);
  } else if (strcasecmp (argv[1], "signal") == 0) {
    ret = semsrv_get_semaphore (semsrv_port,
                                argv[2],
                                &semaphore);

    if (ret != SEMSRV_SUCCESS) {
      mach_error ("semsrv_get_sempahore:", ret);
      return 1;
    }

    ret = semaphore_signal (semaphore);
    if (ret != KERN_SUCCESS) {
      mach_error ("semaphore_signal:", ret);
      return 1;
    }

    printf ("Signalled %s\n", argv[2]);
  } else if (strcasecmp (argv[1], "signal-all") == 0) {
    ret = semsrv_get_semaphore (semsrv_port,
                                argv[2],
                                &semaphore);

    if (ret != SEMSRV_SUCCESS) {
      mach_error ("semsrv_get_sempahore:", ret);
      return 1;
    }

    ret = semaphore_signal_all (semaphore);
    if (ret != KERN_SUCCESS) {
      mach_error ("semaphore_signal_all:", ret);
      return 1;
    }

    printf ("Signalled %s\n", argv[2]);
  } else if (strcasecmp (argv[1], "wait") == 0) {
    ret = semsrv_get_semaphore (semsrv_port,
                                argv[2],
                                &semaphore);

    if (ret != SEMSRV_SUCCESS) {
      mach_error ("semsrv_get_sempahore:", ret);
      return 1;
    }

    printf ("Waiting for %s...", argv[2]);
    fflush (stdout);
    ret = semaphore_wait (semaphore);
    if (ret != KERN_SUCCESS) {
      mach_error ("semaphore_wait:", ret);
      return 1;
    }

    printf ("done\n");
  }

  return 0;
}
