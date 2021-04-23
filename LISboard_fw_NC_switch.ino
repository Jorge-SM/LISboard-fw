// Code written by Jorge Sola


#define NORMALLY_CLOSED_FW
//#define NORMALLY_OPEN_FW


#define SWITCH_A_INTERRUPT PORTA.INTFLAGS & PIN2_bm 
#define PA2_CLEAR_INTERRUPT_FLAG PORTA.INTFLAGS &= PIN2_bm

#define SWITCH_B_INTERRUPT PORTA.INTFLAGS & PIN3_bm 
#define PA3_CLEAR_INTERRUPT_FLAG PORTA.INTFLAGS &= PIN3_bm

#ifdef NORMALLY_CLOSED_FW
  #define SWITCH_A_ACTIVE !(PORTA.IN & PIN2_bm)
  #define SWITCH_B_ACTIVE !(PORTA.IN & PIN3_bm)
#else
  #define SWITCH_A_ACTIVE (PORTA.IN & PIN2_bm)
  #define SWITCH_B_ACTIVE (PORTA.IN & PIN3_bm)
#endif

// Output signal timer has a clock frequency of 2.5 MHz.
// At a clock speed of 2.5 MHz, it takes 3749 ticks to create a 1500 us pulse width
#define MID_POINT 3749

// Output signal timer has a clock frequency of 2.5 MHz.
// At a clock speed of 2.5 MHz, it takes 2250 ticks to create a 900 us pulse width
// This value is defined as the lowest admited input width
#define BOTTOM_SPEED 2250

// Output signal timer has a clock frequency of 2.5 MHz.
// At a clock speed of 2.5 MHz, it takes 3749 ticks to create a 2100 us pulse width
// This value is defined as the highes admited input width
#define TOP_SPEED 5250

void setup() {
  
  // Setup GPIOS
  PORTA.DIR = 0;
  PORTA.DIR = PORTA.DIR | PIN1_bm;
  PORTA.PIN6CTRL = PORT_PULLUPEN_bm;
  PORTA.PIN2CTRL = 3;
  PORTA.PIN3CTRL = 3;
  PORTMUX.CTRLB = 0;
  PORTMUX.CTRLA = 0;
  
  // Setup Timer A, used for Signal Output
  TCA0.SPLIT.CTRLD = 0;
  TCA0.SINGLE.CTRLA = TCA_SINGLE_CLKSEL_DIV8_gc | TCA_SINGLE_ENABLE_bm; // Clock at 20/8 MHz
  TCA0.SINGLE.CTRLB = TCA_SINGLE_WGMODE_SINGLESLOPE_gc | (1<<5);
  TCA0.SINGLE.PERBUF = 6249; // period of 400 hz
  TCA0.SINGLE.CMP1BUF = 3749;


  // Setup Timer B sued for capturin Sginal Input width
  TCB0.CTRLA = TCB_CLKSEL_CLKTCA_gc | TCB_ENABLE_bm;
  TCB0.CTRLB = TCB_CNTMODE_PW_gc; // pulse width measurement mode
  TCB0.EVCTRL = TCB_CAPTEI_bm;
  EVSYS.ASYNCCH0 = EVSYS_ASYNCCH0_PORTA_PIN6_gc;
  EVSYS.ASYNCUSER0 = EVSYS_ASYNCUSER0_ASYNCCH0_gc;
  PORTMUX.CTRLA = 1;
  
  
}

void loop() {
  
  // For reading compare match value: TCB.CCMPL and TCB.CCMPH
  int pulse_in = (TCB0.CCMPH << 8) | TCB0.CCMPL;

    if(SWITCH_A_ACTIVE & pulse_in < MID_POINT){
      TCA0.SINGLE.CMP1BUF = MID_POINT;
    }
    else if(SWITCH_B_ACTIVE & pulse_in > MID_POINT){
      TCA0.SINGLE.CMP1BUF = MID_POINT;
    }
    else{
      if(pulse_in > BOTTOM_SPEED && pulse_in < TOP_SPEED){
        TCA0.SINGLE.CMP1BUF = pulse_in;
      }
      else{
        TCA0.SINGLE.CMP1BUF = MID_POINT;
      }
    }
}


// We use interrupts in order to set a middle throttle output (zero speed) as soon
// as the switch is activated, so that we dont wait to a slower main loop.
ISR(PORTA_PORT_vect)
{
 if(SWITCH_A_INTERRUPT)
 {
  TCA0.SINGLE.CMP1BUF = MID_POINT;
  PA2_CLEAR_INTERRUPT_FLAG;
 }

  if(SWITCH_B_INTERRUPT)
 {
  TCA0.SINGLE.CMP1BUF = MID_POINT;
  PA3_CLEAR_INTERRUPT_FLAG;
 }
}
