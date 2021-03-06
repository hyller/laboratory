/*
 * Copyright (C) 2008-2013 Nabto - All Rights Reserved.
 */
#ifndef _UNABTO_ENV_BASE_UNIX_H_
#define _UNABTO_ENV_BASE_UNIX_H_

/**
 * @file
 * The Basic environment for the Nabto Micro Device Server
 *
 * This file holds definitions, declarations and prototypes to be supplied by the host.
 */

#include "unabto_platform_types.h"

#include <modules/network/bsd/unabto_network_bsd.h>

#define NABTO_SET_TIME_FROM_ALIVE 0
#include <modules/timers/unix/unabto_unix_time.h>

#include <modules/log/unix/unabto_logging_unix.h>

#ifndef lengthof
#define lengthof(x) (sizeof((x)) / sizeof((x)[0]))
#endif

#endif
