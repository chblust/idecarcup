#include "MK64F12.h"
#include "stdio.h"
#include "Camera.h"

// Default System clock value
// period = 1/20485760  = 4.8814395e-8
#define DEFAULT_SYSTEM_CLOCK 20485760u 
// Integration time (seconds)
// Determines how high the camera values are
// Don't exceed 100ms or the caps will saturate
// Must be above 1.25 ms based on camera clk 
//    (camera clk is the mod value set in FTM2)
#define INTEGRATION_TIME .0075f

#define CLK_PIN 9
#define SI_PIN 23
#define FTM2_MOD_SETTING (DEFAULT_SYSTEM_CLOCK)/100000

void init_FTM2(void);
void init_GPIO(void);
void init_PIT(void);
void init_ADC0(void);
void FTM2_IRQHandler(void);
void PIT1_IRQHandler(void);
void ADC0_IRQHandler(void);

// Pixel counter for camera logic
// Starts at -2 so that the SI pulse occurs
//        ADC reads start
int pixcnt = -2;
// clkval toggles with each FTM interrupt
int clkval = 0;
// line stores the current array of camera data
uint16_t line[128];

// These variables are for streaming the camera
//     data over UART
int debugcamdata = 1;
int capcnt = 0;
char str[100];

// ADC0VAL holds the current ADC value
uint16_t ADC0VAL;


/*?ADC0?Conversion?Complete?ISR? */
void ADC0_IRQHandler(void) {
    // Reading ADC0_RA clears the conversion complete flag
    //INSERT CODE HERE
ADC0VAL = ADC0_RA >> 4;
}

/* 
* FTM2 handles the camera driving logic
*    This ISR gets called once every integration period
*        by the periodic interrupt timer 0 (PIT0)
*    When it is triggered it gives the SI pulse,
*        toggles clk for 128 cycles, and stores the line
*        data from the ADC into the line variable
*/
void FTM2_IRQHandler(void){ //For FTM timer
    // Clear interrupt
  FTM2_SC |= FTM_SC_TOF_MASK;
    
    // Toggle clk
    GPIOB_PTOR |= (1 << CLK_PIN);
    clkval ^= 1;
    
    // Line capture logic
    if ((pixcnt >= 2) && (pixcnt < 256)) {
        if (!clkval) {    // check for falling edge
            // ADC read (note that integer division is 
            //  occurring here for indexing the array)
            line[pixcnt/2] = ADC0VAL;
        }
        pixcnt += 1;
    } else if (pixcnt < 2) {
        if (pixcnt == -1) {
            GPIOB_PSOR |= (1 << SI_PIN); // SI = 1
        } else if (pixcnt == 1) {
            GPIOB_PCOR |= (1 << SI_PIN); // SI = 0
            // ADC read
            line[0] = ADC0VAL;
        } 
        pixcnt += 1;
    } else {
        GPIOB_PCOR |= (1 << 9); // CLK = 0
        clkval = 0; // make sure clock variable = 0
        pixcnt = -2; // reset counter
        // Disable FTM2 interrupts (until PIT0 overflows
        //   again and triggers another line capture)
        FTM2_SC &= ~(FTM_SC_TOIE_MASK);
        
    
    }
    return;
}

/* PIT0 determines the integration period
*        When it overflows, it triggers the clock logic from
*        FTM2. Note the requirement to set the MOD register
*     to reset the FTM counter because the FTM counter is 
*        always counting, I am just enabling/disabling FTM2 
*        interrupts to control when the line capture occurs
*/
void PIT0_IRQHandler(void){
    if (debugcamdata) {
        // Increment capture counter so that we can only 
        //    send line data once every ~2 seconds
        capcnt += 1;
    }
    // Clear interrupt
    PIT_TFLG0 |= (PIT_TFLG_TIF_MASK);
    
    // Setting mod resets the FTM counter
    FTM2_MOD = FTM2_MOD_SETTING;
    
    // Enable FTM2 interrupts (camera)
    FTM2_SC |= FTM_SC_TOIE_MASK;
    
    return;
}


/* Initialization of FTM2 for camera */
void init_FTM2(){
    // Enable clock
    SIM_SCGC6 |= SIM_SCGC6_FTM2_MASK;

    // Disable Write Protection
    FTM2_MODE |= FTM_MODE_WPDIS_MASK;
    FTM2_EXTTRIG |= FTM_EXTTRIG_INITTRIGEN_MASK;
    // Set output to '1' on init
    GPIOB_PSOR |= (1<<CLK_PIN);
    clkval = 1;
    
    // Initialize the CNT to 0 before writing to MOD
    FTM2_CNT = 0;
    
    // Set the Counter Initial Value to 0
    capcnt = 0; // ?
    
    // Set the period (~10us)
    // 10us => 100kHz
    // (Sysclock) = clock after prescaler
    // (Sysclock)/100000 => 100kHz => 10us period
    FTM2_MOD = FTM2_MOD_SETTING;
    
    
    // 50% duty
    FTM2_CnV(0) = 210;
    
    // Set edge-aligned mode
    // Enable High-true pulses
    // ELSB = 1, ELSA = 0
    FTM2_CNTIN &= ~(FTM_CnSC_ELSA_MASK | FTM_CnSC_ELSB_MASK);
    FTM2_CNTIN |= FTM_CnSC_ELSB_MASK;
    
    //?Enable?hardware?trigger?from?FTM2
    //ADC0_SC2 |= (ADC_SC2_ADTRG_MASK);
    SIM_SOPT7 |= SIM_SOPT7_ADC0TRGSEL(0b1010);
    
    // Don't enable interrupts yet (disable)
    FTM2_SC &= ~(FTM_SC_TOIE_MASK);
    
    // No prescalar, system clock
    FTM2_SC &= ~(FTM_SC_PS_MASK);
    FTM2_SC &= ~(FTM_SC_CLKS_MASK);
    FTM2_SC |= FTM_SC_CLKS(0b01);
    
    // Set up interrupt
    FTM2_SC |= FTM_SC_TOIE_MASK;
    NVIC_EnableIRQ(FTM2_IRQn);
    
    
    return;
}

/* Initialization of PIT timer to control 
*         integration period
*/
void init_PIT(void){
    // Enable clock for timers
    SIM_SCGC6 |= SIM_SCGC6_PIT_MASK;
    
    // Setup periodic interrupt timer (PIT)
    // Looks like this is active low
    PIT_MCR &= ~(PIT_MCR_MDIS_MASK);
    
    
    
    // Enable timers to continue in debug mode
    //INSERT CODE HERE // In case you need to debug
    PIT_MCR &= ~PIT_MCR_FRZ_MASK;
    
    // PIT clock frequency is the system clock
    // Load the value that the timer will count down from
    // Counts of system clock to get integration time is  int period (s) / system period (s)
    // int period (s) = INTEGRATION_TIME
    // system period (s) = (1/DEFAULT_SYSTEM_CLOCK)
    //float system_clk_period = 1.0/((float)DEFAULT_SYSTEM_CLOCK);
    //unsigned int counter_val = (unsigned int) (INTEGRATION_TIME / system_clk_period);
    //PIT_LDVAL0 = counter_val;
    PIT_LDVAL0 = PIT_LDVAL_TSV(DEFAULT_SYSTEM_CLOCK * INTEGRATION_TIME);//(211*129*211*6);
    // Enable timer interrupts
    // Enable the timer
    PIT_TCTRL0 |= (PIT_TCTRL_TIE_MASK | PIT_TCTRL_TEN_MASK);

    // Clear interrupt flag
     PIT_TFLG0 |= (PIT_TFLG_TIF_MASK);
    // Enable PIT interrupt in the interrupt controller
    NVIC_EnableIRQ(PIT0_IRQn);
    return;
}

/* Set up pins for GPIO
*     PTB9         - camera clk
*        PTB23        - camera SI
*        PTB22        - red LED
*/
void init_GPIO(void){    
    // enable port B
    SIM_SCGC5 |= SIM_SCGC5_PORTB_MASK;
    
    // set all to GPIO
    PORTB_PCR9 &= PORT_PCR_MUX_MASK;
    PORTB_PCR9 |= PORT_PCR_MUX(0b001);
    PORTB_PCR23 &= PORT_PCR_MUX_MASK;
    PORTB_PCR23 |= PORT_PCR_MUX(0b001);
    
    // NOT doing LED init since its done in the leds.c module
    
    // set all to output mode
    GPIOB_PDDR |= ((1 << SI_PIN) | (1 << CLK_PIN));
    
    return;
}

/* Set up ADC for capturing camera data */
void init_ADC0(void) {
    unsigned int calib;
    // Turn on ADC0
   //Enable ADC Clock

	SIM_SCGC6 |= SIM_SCGC6_ADC0_MASK;

	// 16 bit single ended no div
	ADC0_CFG1 &= ~(ADC_CFG1_ADIV_MASK);
	ADC0_CFG1 |= ADC_CFG1_MODE_MASK;
    
    // Do ADC Calibration for Singled Ended ADC. Do not touch.
    ADC0_SC3 = ADC_SC3_CAL_MASK;
    while ( (ADC0_SC3 & ADC_SC3_CAL_MASK) != 0 );
    calib = ADC0_CLP0; calib += ADC0_CLP1; calib += ADC0_CLP2;
    calib += ADC0_CLP3; calib += ADC0_CLP4; calib += ADC0_CLPS;
    calib = calib >> 1; calib |= 0x8000;
    ADC0_PG = calib;
    
	// hardware trigger.
	ADC0_SC2 |= ADC_SC2_ADTRG_MASK;
	// Enable interrupt
	ADC0_SC1A |= ADC_SC1_AIEN_MASK;
	// single ended
	ADC0_SC1A &=   ~(ADC_SC1_DIFF_MASK);
	// Select DADP0 as channel
	ADC0_SC1A &= ~(ADC_SC1_ADCH_MASK);
	ADC0_SC1A |= ADC_SC1_ADCH(0);

	// adc0 ftm2 trigger
	SIM_SOPT7 |= SIM_SOPT7_ADC0TRGSEL(10);  
	SIM_SOPT7 |= SIM_SOPT7_ADC0ALTTRGEN_MASK; 
	SIM_SOPT7 &= ~SIM_SOPT7_ADC0PRETRGSEL_MASK; 

	// Enable NVIC interrupt
  NVIC_EnableIRQ(ADC0_IRQn);
}


void Camera_init(void)
{
		init_GPIO(); // For CLK and SI output on GPIO
    init_FTM2(); // To generate CLK, SI, and trigger ADC
    init_ADC0();
    init_PIT();
}

void Camera_capture(void)
{
	// pulse SI
	GPIOB_PCOR |= (1 << 22);
	for(int i = 0; i < 1000; i++);
	GPIOB_PSOR |= (1 << 22);
}

float Camera_get_light_level(unsigned int addr)
{
	addr = addr % 128;
	return ((float) line[addr]) / 4095.0;
}
