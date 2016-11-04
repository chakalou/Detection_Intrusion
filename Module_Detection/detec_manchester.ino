#include <Manchester.h>
#include <avr/sleep.h>
#include <avr/interrupt.h>


#define TX_PIN  3 //pin où le transmitter est connecté
#define DETECTION_PIN 2 //pin associé au détecteur 
#define SENDER_ID 5   //identifiant du détecteur (doit être unique)
#define ALIM_TX_PIN 4 //pin pour alimenter le transmiter onde radio (economie d'energie)
#define MAX_MSG_ENV 3 //nombre de fois que le message est envoyé pour être sur de le recevoir


uint8_t transmit_data = 0;

void setup() {
  //On initialise la pin reliée au détecteur en tant qu'entrée
  pinMode(DETECTION_PIN, INPUT);
  pinMode(ALIM_TX_PIN, OUTPUT);
  digitalWrite(ALIM_TX_PIN, LOW); //on coupe l'alim du transmitter
  man.workAround1MhzTinyCore(); //add this in order for transmitter to work with 1Mhz Attiny85/84
  man.setupTransmit(TX_PIN, MAN_1200); // Initialisation
}

void loop() {
  //on dors en attendant une interruption
  sleep();
}


void sleep() {

    GIMSK |= _BV(PCIE);                     // Enable Pin Change Interrupts
    PCMSK |= _BV(PCINT2);                   // Use PB2 as interrupt pin
    ADCSRA &= ~_BV(ADEN);                   // ADC off
    set_sleep_mode(SLEEP_MODE_PWR_DOWN);    // replaces above statement

    sleep_enable();                         // Sets the Sleep Enable bit in the MCUCR Register (SE BIT)
    delay(1);
    sei();                                  // Enable interrupts
    sleep_cpu();                            // sleep

    cli();                                  // Disable interrupts
    PCMSK &= ~_BV(PCINT2);                  // Turn off PB2 as interrupt pin
    sleep_disable();                        // Clear SE bit
    ADCSRA |= _BV(ADEN);                    // ADC on

    sei();                                  // Enable interrupts
    } // sleep

ISR(PCINT0_vect) {
    if (PINB & (1<<PB2)) // detection de front montant
	{  
		transmettre();
	}
    //Si front descendant, on ne fait rien !!! :)
    }
 //Fonction permettant de transmettre la détection   
    void transmettre()
{
      /*On active l'alim du transmiter*/
      digitalWrite(ALIM_TX_PIN, HIGH);
      short i;
      
      for(i=0;i<MAX_MSG_ENV;i++)
      {
        man.transmit(man.encodeMessage(SENDER_ID, transmit_data));
        transmit_data++;
      }
      /*On désactive l'alim du transmiter*/
      digitalWrite(ALIM_TX_PIN, LOW);
      delay(5000);
}
