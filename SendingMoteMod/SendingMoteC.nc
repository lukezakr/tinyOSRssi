/*  T13 == BaseStation, T6 == Node 1
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
  uses interface Leds;
  
  
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
  uint8_t ActiveFlag = 1;
  uint8_t counter = 2;
  uint16_t totalNodes = 6;
  int i;
  
  
  //task void radioSend();
  
  
   event message_t* Receive.receive(message_t *msg1, void *payload, uint8_t len) {
    
    message_t *ret = msg1;
    RssiMsg *rssiMsg = (RssiMsg*) payload;
    rssiMsg->rssi = getRssi(msg1);
    rssiMsg->targetID = rssiMsg->senderID;
    ActiveFlag = rssiMsg->activeFlag;
    counter = rssiMsg->passiveFlag;
    call Leds.led0On();
    call Leds.led1Off();
    if (rssiMsg->activeFlag == TOS_NODE_ID)
    {
        if (totalNodes == TOS_NODE_ID)
        {
            counter = 1;
            ActiveFlag = 1;
        }
        
        else if (counter == totalNodes)
        {
            rssiMsg->changeActiveFlag = 1;
        }
        
        else
        {
            counter = counter + 1;
        }
    }
    else
        if (rssiMsg->changeActiveFlag == 1)
        {
            counter = ActiveFlag + 2;
            ActiveFlag = ActiveFlag + 1;
            rssiMsg->changeActiveFlag = 0;
            
            if (ActiveFlag == totalNodes)
            {
               ActiveFlag = 0;
               counter = 0;
            }
        }
            
    
     message = *ret;
     //for (i = 0; i < 6000; i++) 
     //post radioSend();
      return ret;
   }
  

  
  event void Boot.booted(){
    call RadioControl.start();
    /*if (TOS_NODE_ID == 1)
    {
        counter = 2;
    }*/
  }

  event void RadioControl.startDone(error_t result){
    srand(TOS_NODE_ID);
    call SendTimer.startPeriodic(SEND_INTERVAL_MS);
  }

  event void RadioControl.stopDone(error_t result){}

/*task void radioSend() {
   // for (i = 0; i < 400; i++) 
    RssiMsg* packet = (RssiMsg*) (call RssiMsgSend.getPayload(&message, sizeof (RssiMsg)));
      if ((packet->senderID == packet->sendingFlag) || (packet->sendingFlag == TOS_NODE_ID))
      {
      packet->senderID = TOS_NODE_ID;
    call RssiMsgSend.send(AM_BROADCAST_ADDR, &message, sizeof(RssiMsg));
      }  
}*/


  event void SendTimer.fired(){
      RssiMsg* packet = (RssiMsg*) (call RssiMsgSend.getPayload(&message, sizeof (RssiMsg)));
      if ((ActiveFlag == TOS_NODE_ID) || (counter == TOS_NODE_ID))
      {
      packet->senderID = TOS_NODE_ID;
      packet->activeFlag = ActiveFlag;
      packet->passiveFlag = counter;
    call RssiMsgSend.send(AM_BROADCAST_ADDR, &message, sizeof(RssiMsg));
      
      call Leds.led0Off();
    call Leds.led1On();
      }
      
  }

  event void RssiMsgSend.sendDone(message_t *m, error_t error){}
  
#ifdef __CC2420_H__  
  uint16_t getRssi(message_t *msg){
    return (uint16_t) call CC2420Packet.getRssi(msg);
  }
#elif defined(CC1K_RADIO_MSG_H)
    uint16_t getRssi(mecounter = ActiveFlag + 2;ssage_t *msg){
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
