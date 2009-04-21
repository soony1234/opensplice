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
#ifndef NW_PLUGPARTITIONS_H
#define NW_PLUGPARTITIONS_H

#include "nw_commonTypes.h"
#include "nw_plugTypes.h"
#include "nw_partitions.h"

NW_CLASS(nw_plugPartitions);

nw_seqNr
nw_plugPartitionsGetNofPartitions(
    nw_plugPartitions plugPartitions);
    
void
nw_plugPartitionsGetPartition(
    nw_plugPartitions plugPartitions,
    nw_partitionId partitionId,
    nw_bool *found,
    nw_partitionAddress *partitionAddress,
    nw_bool *connected);

nw_bool
nw_plugPartitionsGetDefaultPartition(
    nw_plugPartitions plugPartitions,
    nw_partitionAddress *partitionAddress);
    
#endif /*NW_PLUGPARTITIONS_H_*/
