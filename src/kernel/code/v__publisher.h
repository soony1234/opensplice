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

#ifndef V__PUBLISHER_H
#define V__PUBLISHER_H

#include "v_publisher.h"
#include "v_entity.h"
#include "v_event.h"

#define v_publisherIsSuspended(_this) \
        (c_timeCompare((_this)->suspendTime, C_TIME_INFINITE) != C_EQ)

v_publisherQos
v_publisherGetQosRef(
    v_publisher _this);

v_result
v_publisherSetQos(
    v_publisher _this,
    v_publisherQos qos);

void
v_publisherNotify(
v_publisher _this,
    v_event e);

#endif
