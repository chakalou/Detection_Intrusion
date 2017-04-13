
#include <Manchester.h>

/*

  Manchester Receiver example
  
  In this example receiver will receive one byte of payoad and 4 bit sender id per transmittion
  message also contains a checksum and receiver can check if the data has been transmited correctly

  try different speeds using this constants, your maximum possible speed will 
  depend on various factors like transmitter type, distance, microcontroller speed, ...

  MAN_300 0
  MAN_600 1
  MAN_1200 2
  MAN_2400 3
  MAN_4800 4
  MAN_9600 5
  MAN_19200 6
  MAN_38400 7


*/
#define MAX_DETECETEUR 10
#define RX_PIN 9
#define TEMPODETEC 60000

uint8_t data;
uint8_t id;
char toto[64];
long tabdetecencours[MAX_DETECETEUR];

void setup() {
  Serial.begin(9600);
  Serial.println("start");
  man.setupReceive(RX_PIN, MAN_1200);
  man.beginReceive();
  
  short i;
  for(i=0;i<MAX_DETECETEUR;i++)
  {
    tabdetecencours[i]=-1;
  }
}

void loop() {
  if (man.receiveComplete()) { //received something
    uint16_t m = man.getMessage();
    man.beginReceive(); //start listening for next message right after you retrieve the message
    if (man.decodeMessage(m, id, data)) { //extract id and data from message, check if checksum is correct
      //id now contains ID of sender (or receiver(or any other arbitrary 4 bits of data))
      //data now contains one byte of data received from sender
      //both id and data are BELIEVED (not guaranteed) to be transmitted correctly
      
      // On vérifie que l'id recu est bon
      if(id<MAX_DETECETEUR && id>0)
      {
        if(tabdetecencours[id-1]>=0)
        {
          if(((tabdetecencours[id-1]+TEMPODETEC)%2147483647)<=millis())
          {
              tabdetecencours[id-1]=-1;
              sprintf(toto,"Fin tempo attente pour codedetec=%d",id);
              Serial.println(toto);
          }
        }
        if(tabdetecencours[id-1]<=0)
        {
            sprintf(toto,"message recu: m=%d, id=%d, data=%d",m,id,data);
            Serial.println(toto);
            
            tabdetecencours[id-1]=millis();
        }
        else
        {
          sprintf(toto,"Detection inhibe pour codedetec=%d",id);
            Serial.println(toto);
        
        }
      }
    }
  }
}


