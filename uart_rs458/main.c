//******************************************************************************
//   MSP430G2xx3 Demo - USCI_A0, 9600 UART Echo ISR, DCO SMCLK
//
//   Description: Echo a received character, RX ISR used. Normal mode is LPM0.
//   USCI_A0 RX interrupt triggers TX Echo.
//   Baud rate divider with 1MHz = 1MHz/9600 = ~104.2
//   ACLK = n/a, MCLK = SMCLK = CALxxx_1MHZ = 1MHz
//
//                MSP430G2xx3
//             -----------------
//         /|\|              XIN|-
//          | |                 |
//          --|RST          XOUT|-
//            |                 |
//            |     P1.2/UCA0TXD|------------>
//            |                 | 9600 - 8N1
//            |     P1.1/UCA0RXD|<------------
//
//   D. Dang
//   Texas Instruments Inc.
//   February 2011
//   Built with CCS Version 4.2.0 and IAR Embedded Workbench Version: 5.10
//******************************************************************************
#include  "msp430g2553.h"

void tx(unsigned char c);
void send_enable();
void recv_enable();

unsigned char rx = 'n';
unsigned int blink_count = 0;

#define BLINK_DELAY 65535

void main(void)
{
  int i, j;
  P1DIR |= BIT0 + BIT3 + BIT4;  // set ports for output: 1.0 (LED 1), 1.3 (RS485 direction control), 1.4 (led)
  P1OUT &= ~(BIT0 + BIT4);

  WDTCTL = WDTPW + WDTHOLD;                 // Stop WDT
  BCSCTL1 = CALBC1_1MHZ;                    // Set DCO
  DCOCTL = CALDCO_1MHZ;
  P1SEL = BIT1 + BIT2 ;                     // P1.1 = RXD, P1.2=TXD
  P1SEL2 = BIT1 + BIT2 ;                     // P1.1 = RXD, P1.2=TXD
  UCA0CTL1 |= UCSSEL_2;                     // SMCLK
  UCA0BR0 = 8;                              // 1MHz 115200
  UCA0BR1 = 0;                              // 1MHz 115200
  UCA0MCTL = UCBRS2 + UCBRS0;               // Modulation UCBRSx = 5
  UCA0CTL1 &= ~UCSWRST;                     // **Initialize USCI state machine**
  IE2 |= UCA0RXIE;                          // Enable USCI_A0 RX interrupt

  // set up timer interrupt
  //CCTL0 = CCIE;
  //CCTL1 = CCIE;
  //TACTL = TASSEL_2 + MC_2 + TAIE;

  /*
  while(1) {
    for(i=0;i<1000;i++) {
      for(j=0;j<1000;j++) { nop(); }
    }
    P1OUT ^= BIT0;  // toggle the LED
  } */

  recv_enable();

  while(0) {
	  send_enable();
	  tx(rx);
	  tx('\n');
	  tx('\r');
	  recv_enable();
	  for(i=0;i<30000;i++) { nop(); }
  }

  __bis_SR_register(LPM0_bits + GIE);       // Enter LPM0, interrupts enabled
}

void send_enable() { P1OUT |= BIT3; }
void recv_enable() { int i; for(i=0;i<500;i++) {nop();} P1OUT &= ~BIT3; } // better way to do delay?

#pragma vector=USCIAB0RX_VECTOR
__interrupt void USCI0RX_ISR(void)
{
	int i;
	P1OUT ^= BIT0;  // toggle the LED
	for(i=0;i<500;i++) {nop();}
	send_enable();
	tx(UCA0RXBUF);
	recv_enable();
}

#pragma vector=TIMER0_A0_VECTOR
__interrupt void Timer_A0(void)
{
	if(++blink_count % 3 == 0) {
		P1OUT ^= BIT4;
		blink_count = 0;
	}
	CCR0 += BLINK_DELAY;
}

#pragma vector=TIMER0_A1_VECTOR
__interrupt void Timer_A1(void)
{
	P1OUT ^= BIT0;
	CCR1 += 100;
}

void tx(unsigned char c)
{
  while (!(IFG2&UCA0TXIFG));        // USCI_A0 TX buffer ready?
  UCA0TXBUF = c;                    // TX -> RXed character
}


