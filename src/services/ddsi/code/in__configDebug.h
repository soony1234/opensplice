/*
 *                         OpenSplice DDS
 *
 *   This software and documentation are Copyright 2006 to 2009 PrismTech 
 *   Limited and its licensees. All rights reserved. See file:
 *
 *                     $OSPL_HOME/LICENSE 
 *
 *   for full copyright notice and license terms. 
 *
 */
/* Prevent failure due to multiple inclusion of this file. */
#ifndef IN_CONFIG_DEBUG_H
#define IN_CONFIG_DEBUG_H

/* OS abstraction includes. */
#include "os_defs.h"
#include "os_classbase.h"
#include "os_stdlib.h"
#include "in__configTypes.h"

/**
 * Allow usage of this C code from C++ code.
 */
#if defined (__cplusplus)
extern "C" {
#endif



/**
 * Macro that allows the implementation of type checking when casting an
 * object. The signature of the 'casting macro' must look like this:
 */
#define in_configDebug(_this) ((in_configDebug)_this)

os_uint32
in_configDebugGetWaitTime(
    in_configDebug _this);

os_char*
in_configDebugGetPathName(
    in_configDebug _this);

/* Close the brace that allows the usage of this code in C++. */
#if defined (__cplusplus)
}
#endif

#endif /* IN_CONFIG_DEBUGGING_H */
