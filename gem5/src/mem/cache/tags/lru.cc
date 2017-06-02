/*
 * Copyright (c) 2012-2013 ARM Limited
 * All rights reserved.
 *
 * The license below extends only to copyright in the software and shall
 * not be construed as granting a license to any other intellectual
 * property including but not limited to intellectual property relating
 * to a hardware implementation of the functionality of the software
 * licensed hereunder.  You may use the software subject to the license
 * terms below provided that you ensure that this notice is replicated
 * unmodified and in its entirety in all distributions of the software,
 * modified or unmodified, in source code or in binary form.
 *
 * Copyright (c) 2003-2005,2014 The Regents of The University of Michigan
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met: redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer;
 * redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in the
 * documentation and/or other materials provided with the distribution;
 * neither the name of the copyright holders nor the names of its
 * contributors may be used to endorse or promote products derived from
 * this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * Authors: Erik Hallnor
 */

/**
 * @file
 * Definitions of a LRU tag store.
 */

#include "mem/cache/tags/lru.hh"

#include "debug/CacheRepl.hh"
#include "mem/cache/base.hh"

LRU::LRU(const Params *p)
    : BaseSetAssoc(p)
{
}

CacheBlk*
LRU::accessBlock(Addr addr, bool is_secure, Cycles &lat)
{
    CacheBlk *blk = BaseSetAssoc::accessBlock(addr, is_secure, lat);

    if (blk != nullptr) {
        // move this block to head of the MRU list
        sets[blk->set].moveToHead(blk);
        DPRINTF(CacheRepl, "set %x: moving blk %x (%s) to MRU\n",
                blk->set, regenerateBlkAddr(blk->tag, blk->set),
                is_secure ? "s" : "ns");
    }

    return blk;
}

CacheBlk** 
LRU::accessBlock_horizontal(Addr addr, bool is_secure,Cycles &lat,
                            int & block_pos,int & set_pos) 
{
    CacheBlk ** blk =BaseSetAssoc::accessBlock_horizontal(addr,is_secure,lat,block_pos,set_pos);
    DPRINTF(Cache_yj,"move to the head position is %#llx %d\n",addr,block_pos);
    if(block_pos!=-1)
    {
       sets[blk[block_pos]->set].moveToHead(blk[block_pos]);
       DPRINTF(Cache_yj,"LRU accessBlock_horizontal set %d %d\n",blk[block_pos]->set,blk[block_pos]->origin_set);
       DPRINTF(CacheRepl, "set %x: moving blk %x (%s) to MRU\n",
                blk[block_pos]->set, regenerateBlkAddr(blk[block_pos]->tag, blk[block_pos]->set),
                is_secure ? "s" : "ns");    
    }
    return blk;
}
CacheBlk* 
LRU::findHorizontalVictim(Addr addr)
{
    int set=extractHorizontalset(addr);
    //int way_location = extractWaylocation(addr);
    BlkType *blk = nullptr;

    int i=0;
    int count=0;
    DPRINTF(Cache_yj,"findHorizontalVictim start\n");

    for(i=0;i<assoc;i++)
    {
	       BlkType * blk =sets[set].blks[i];
	       if(blk->isValid())
		      count++;
    }
    if(count==assoc)
	       conflictmiss++;

    for (int i = assoc - 1; i >= 0; i--) {
        BlkType *b = sets[set].blks[i];
      
        if (b->way < allocAssoc) {
          //  if(b->isValid())
          //      b->origin_set=extractSet(addr);

            blk = b;
            break;
        }
    }

    assert(!blk || blk->way < allocAssoc);
    if (blk && blk->isValid()) {
        DPRINTF(CacheRepl, "set %x: selecting blk %x for replacement\n",
                set, regenerateBlkAddr(blk->tag, set));
    }

    return blk;
}
CacheBlk*
LRU::findVictim(Addr addr)
{
    int set = extractSet(addr);
    // grab a replacement candidate
    BlkType *blk = nullptr;
    for (int i = assoc - 1; i >= 0; i--) {
        BlkType *b = sets[set].blks[i];
        if (b->way < allocAssoc) {
            blk = b;
            break;
        }
    }
    assert(!blk || blk->way < allocAssoc);

    if (blk && blk->isValid()) {
        DPRINTF(CacheRepl, "set %x: selecting blk %x for replacement\n",
                set, regenerateBlkAddr(blk->tag, set));
    }

    return blk;
}

void
LRU::insertBlock(PacketPtr pkt, BlkType *blk,bool Horizontal_mode)
{
    BaseSetAssoc::insertBlock(pkt, blk, Horizontal_mode);

    int set;
    if(Horizontal_mode)
        set = extractHorizontalset(pkt->getAddr());
    else
        set = extractSet(pkt->getAddr());
    sets[set].moveToHead(blk);
}

void
LRU::invalidate(CacheBlk *blk,bool Horizontal_mode)
{
    BaseSetAssoc::invalidate(blk);

    // should be evicted before valid blocks
    int set;
    if(Horizontal_mode)
        set=blk->set;
    else
        set=blk->set;

    sets[set].moveToTail(blk);
}

LRU*
LRUParams::create()
{
    return new LRU(this);
}
