/*
 * Copyright (c) 2008 Dimas Abreu Dutra
 * All rights reserved
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * - Redistributions of source code must retain the above copyright
 *   notice, this list of conditions and the following disclaimer.
 * - Redistributions in binary form must reproduce the above copyright
 *   notice, this list of conditions and the following disclaimer in the
 *   documentation and/or other materials provided with the
 *   distribution.
 * - Neither the name of the Stanford University nor the names of
 *   its contributors may be used to endorse or promote products derived
 *   from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL DIMAS ABREU
 * DUTRA OR HIS CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
 * OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/**
 * @author Dimas Abreu Dutra
 */

#include "ApplicationDefinitions.h"
#include "RssiDemoMessages.h"
#include "AM.h"

module SendingMoteC {
  uses interface Boot;
  uses interface Timer<TMilli> as SendTimer;
  uses interface Intercept as RssiMsgIntercept;
  
  uses interface AMSend as RssiMsgSend;
  uses interface Receive;
  uses interface SplitControl as RadioControl;

#ifdef __CC2420_H__
  uses interface CC2420Packet;
#elif defined(TDA5250_MESSAGE_H)
  uses interface Tda5250Packet;    
#else
  uses interface PacketField<uint8_t> as PacketRSSI;
#endif 
  
} implementation {
  message_t message;
  uint16_t getRssi(message_t *msg);
  uint16_t totalNodes = 4; //needs to be set to the number of nodes currently being used in the system
  uint16_t nodecount = 1;
  uint16_t nodeTracker[20] = {0}; //each space in the array holds a flag for a node in the network, there is space for a max of 20 nodes
  uint16_t nodeTrackerSum = 0;
  int i;
  
  
   event message_t* Receive.receive(message_t *msg1, void *payload, uint8_t len) {
    message_t *ret = msg1;
    RssiMsg *rssiMsg = (RssiMsg*) payload;
    nodeTracker[(rssiMsg->senderID)-1] = 1; //records that the master node has recieved a packet from each sending node 
    for (i = 0; i < 19; i++)
    {
        nodeTrackerSum = nodeTrackerSum + nodeTracker[i];
    }
    
    if (nodeTrackerSum == totalNodes)
    {
        nodecount = nodecount + 1;
        for (i = 0; i < 19; i++)
    {
        nodeTracker[i] = 0;
    }
    }
    
    nodeTrackerSum = 0;
    //rssiMsg->rssi = getRssi(msg1);
    //rssiMsg->targetID = rssiMsg->senderID;
    
    message = *ret;
      return ret;
   }
  
  event bool RssiMsgIntercept.forward(message_t *msg1,
                      void *payload,
                      uint8_t len) {
    RssiMsg *rssiMsg = (RssiMsg*) payload;
    rssiMsg->rssi = 43981;
    
    return TRUE;
  }
  
  event void Boot.booted(){
    call RadioControl.start();
  }

  event void RadioControl.startDone(error_t result){
    call SendTimer.startPeriodic(SEND_INTERVAL_MS);
  }

  event void RadioControl.stopDone(error_t result){}


  event void SendTimer.fired(){
      RssiMsg* packet = (RssiMsg*) (call RssiMsgSend.getPayload(&message, sizeof (RssiMsg)));
      packet->senderID = TOS_NODE_ID;
      packet->sendingFlag = nodecount;
    call RssiMsgSend.send(AM_BROADCAST_ADDR, &message, sizeof(RssiMsg));    
  }

  event void RssiMsgSend.sendDone(message_t *m, error_t error){}
  
#ifdef __CC2420_H__  
  uint16_t getRssi(message_t *msg){
    return (uint16_t) call CC2420Packet.getRssi(msg);
  }
#elif defined(CC1K_RADIO_MSG_H)
    uint16_t getRssi(message_t *msg){
    cc1000_metadata_t *md =(cc1000_metadata_t*) msg->metadata;
    return md->strength_or_preamble;
  }
#elif defined(PLATFORM_IRIS) || defined(PLATFORM_UCMINI)
  uint16_t getRssi(message_t *msg){
    if(call PacketRSSI.isSet(msg))
      return (uint16_t) call PacketRSSI.get(msg);
    else
      return 0xFFFF;
  }
#elif defined(TDA5250_MESSAGE_H)
   uint16_t getRssi(message_t *msg){
       return call Tda5250Packet.getSnr(msg);
   }
#else
  #error Radio chip not supported! This demo currently works only \
         for motes with CC1000, CC2420, RF230, RFA1 or TDA5250 radios.  
#endif
}
