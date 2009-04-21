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
/*
 * in_transportSenderIBasic.c
 *
 *  Created on: Feb 10, 2009
 *      Author: frehberg
 */

/* interfaces */
#include "in_transportSenderIBasic.h"
#include "in__transportSender.h"

/* implementation */
#include "os_heap.h"
#include "in_socket.h"
#include "in__config.h"
#include "in__configDdsiService.h"
#include "in__configPartitioning.h"
#include "in_sendBuffer.h"

/** \brief abstract class  */
OS_STRUCT(in_transportSenderIBasic)
{
	OS_EXTENDS(in_transportSender);
	in_socket  socket;
	os_uint32 fragmentLength;
};

/** \brief declare virtual operations */
static OS_STRUCT(in_abstractSendBuffer)*
in_transportSenderGetBuffer_i(
		in_transportSender _this);

/** */
static os_result
in_transportSenderSendTo_i(
		in_transportSender _this,
		in_locator sendTo,
		in_abstractSendBuffer buffer,
		os_size_t length,
		os_boolean isControl,
		os_time *timeout);

/** */
static void
in_transportSenderPeriodicAction_i(
		in_transportSender _this,
		os_time *timeout);

/** */
static void
in_transportSenderReleaseBuffer_i(
		in_transportSender _this,
		in_abstractSendBuffer iBuffer);


/** */
static OS_STRUCT(in_abstractSendBuffer)*
in_transportSenderGetBuffer_i(in_transportSender _this);

/** */
static in_locator
in_transportSenderGetDataUnicastLocator_i(in_transportSender _this);

/** */
static in_locator
in_transportSenderGetDataMulticastLocator_i(in_transportSender _this);

/* define protected methods */
static OS_STRUCT(in_abstractSendBuffer)*
in_transportSenderGetBuffer_i(
        in_transportSender _this)
{
    /* narrow    */
    in_transportSenderIBasic narrowed =
        (in_transportSenderIBasic) _this;

	in_sendBuffer result =
		in_sendBufferNew(narrowed->fragmentLength);
	return (in_abstractSendBuffer) result;
}

static os_result
in_transportSenderSendTo_i(
		in_transportSender _this,
		in_locator receiver,
		in_abstractSendBuffer buffer,
		os_size_t length,
		os_boolean isControl,
		os_time *timeout)
{
	in_transportSenderIBasic narrowed =
		(in_transportSenderIBasic) _this;

	os_result result = os_resultSuccess;

	if (isControl && in_socketSupportsControl(narrowed->socket)) {
		in_socketSendControlTo(
				narrowed->socket,
				receiver,
				in_abstractSendBufferBegin(buffer),
				length);
	} else {
		in_socketSendDataTo(
				narrowed->socket,
				receiver,
				in_abstractSendBufferBegin(buffer),
				length);
	}
	return result;
}

static void
in_transportSenderPeriodicAction_i(
		in_transportSender _this,
		os_time *timeout)
{
	/* nop */
}

static void
in_transportSenderReleaseBuffer_i(
		in_transportSender _this,
		in_abstractSendBuffer iBuffer)
{
	in_sendBufferFree(in_sendBuffer(iBuffer));
}


/** */
static void
in_objectDeinit_i(
		in_object _this)
{
    /* narrow */
    in_transportSenderIBasic narrowed =
        in_transportSenderIBasic(_this);

	in_transportSenderDeinit(OS_SUPER(narrowed));
}

/** */
static in_locator
in_transportSenderGetDataUnicastLocator_i(in_transportSender _this)
{
    /* narrow */
     in_transportSenderIBasic narrowed =
         in_transportSenderIBasic(_this);

    return in_socketGetUnicastDataLocator(narrowed->socket);
}

/** */
static in_locator
in_transportSenderGetDataMulticastLocator_i(in_transportSender _this)
{
    /* narrow */
     in_transportSenderIBasic narrowed =
         in_transportSenderIBasic(_this);

    return in_socketGetMulticastDataLocator(narrowed->socket);
}

/** */
static in_locator
in_transportSenderGetControlUnicastLocator_i(in_transportSender _this)
{
    /* narrow */
     in_transportSenderIBasic narrowed =
         in_transportSenderIBasic(_this);

    in_locator result = in_socketGetUnicastControlLocator(narrowed->socket);
    if (!result) {
        result = in_socketGetUnicastDataLocator(narrowed->socket);
    }
    return result;
}

static const
OS_STRUCT(in_transportSenderPublicVTable) staticPublicVTable = {
        in_transportSenderGetBuffer_i,
        in_transportSenderSendTo_i,
        in_transportSenderPeriodicAction_i,
        in_transportSenderReleaseBuffer_i,
        in_transportSenderGetDataUnicastLocator_i,
        in_transportSenderGetDataMulticastLocator_i,
        in_transportSenderGetControlUnicastLocator_i};

static os_boolean
in_transportSenderIBasicInit(
		in_transportSenderIBasic _this,
		in_configChannel configChannel)
{
	os_boolean result = OS_TRUE;
	os_boolean supportsControl = OS_FALSE;
	os_uint32 fragmentLength = 0;

	fragmentLength =
		in_configChannelGetFragmentSize(configChannel);
	supportsControl =
		in_configChannelSupportsControl(configChannel);

	assert(supportsControl==OS_FALSE || supportsControl == OS_TRUE);
	assert(fragmentLength <= 0x00ffffff); /* anything else is not plausible */

	in_transportSenderInitParent(OS_SUPER(_this),
			IN_OBJECT_KIND_TRANSPORT_SENDER_BASIC,
			in_objectDeinit_i,
			&staticPublicVTable);

	_this->socket = in_socketSendNew(
			configChannel,
			supportsControl);

	_this->fragmentLength = fragmentLength;

	if (!(_this->socket)) {
		/* error occured, faulty channel configuration */
		result = OS_FALSE;
		in_transportSenderDeinit(OS_SUPER(_this));
	}

	return result;
}

/** \brief constructor */
in_transportSenderIBasic
in_transportSenderIBasicNew(in_configChannel config)
{
	in_transportSenderIBasic result =
		(in_transportSenderIBasic) os_malloc(OS_SIZEOF(in_transportSenderIBasic));

	if (result) {
		if (!in_transportSenderIBasicInit(result, config)) {
			os_free(result);
			result = NULL;
		}
	}

	return result;
}


/** \brief destructor */
void
in_transportSenderIBasicFree(
		in_transportSenderIBasic _this)
{
	in_objectFree(OS_SUPER(OS_SUPER(_this)));
}

