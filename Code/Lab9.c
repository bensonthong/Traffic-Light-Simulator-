#include <stdio.h>
#include <stdlib.h>
#include <xc.h>
#include <math.h>
#include <p18f4620.h>
#include <usart.h>

#pragma config OSC = INTIO67
#pragma config WDT = OFF
#pragma config LVP = OFF
#pragma config BOREN = OFF
#pragma config CCP2MX = PORTBE


#define _XTAL_FREQ      8000000                                                 // Set operation for 8 Mhz
#define TMR_CLOCK       _XTAL_FREQ/4                                            // Timer Clock 2 Mhz

#define TFT_DC      PORTDbits.RD0                                               // Location of TFT D/C
#define TFT_CS      PORTDbits.RD1                                               // Location of TFT Chip Select
#define TFT_RST     PORTDbits.RD2                                               // Location of TFT Reset

#define SEC_LED     PORTDbits.RD7                                               // define SEC_LED on PORTD bit 7 

#define NS_RED      PORTAbits.RA1                                               // define NS_RED on PORTA bit 1
#define NS_GREEN    PORTAbits.RA2                                               // define NS_GREEN on PORTA bit 2

#define NSLT_RED    PORTDbits.RD3                                               // define NSLT_RED on PORTD bit 3
#define NSLT_GREEN  PORTDbits.RD4                                               // define NSLT_GREEN on PORTD bit 4

#define EW_RED      PORTDbits.RD5                                               // define EW_RED on PORTD bit 5
#define EW_GREEN    PORTDbits.RD6                                               // define EW_GREEN on PORTD bit 6

#define EWLT_RED    PORTEbits.RE0                                               // define EWLT_RED on PORTE bit 0
#define EWLT_GREEN  PORTEbits.RE1                                               // define EWLT_GREEN on PORTE bit 1
#define MODE_LED    PORTEbits.RE2                                               // define MODE_LED on PORTE bit 2

#define NS_LT       PORTCbits.RC1                                               // define NS_LT on PORTC bit 1
#define EW_LT       PORTCbits.RC2                                               // define EW_LT on PORTC bit 2

#define OFF 		0                                                           // Defines OFF as decimal value 0
#define RED 		1                                                           // Defines RED as decimal value 1
#define GREEN 		2                                                           // Defines GREEN as decimal value 2
#define YELLOW 		3                                                           // Defines YELLOW as decimal value 3

#define Circle_Size     7                                                       // Size of Circle for Light
#define Circle_Offset   15                                                      // Location of Circle
#define TS_1            1                                                       // Size of Normal Text
#define TS_2            2                                                       // Size of PED Text
#define Count_Offset    10                                                      // Location of Count

#define XTXT            30                                                      // X location of Title Text 
#define XRED            40                                                      // X location of Red Circle
#define XYEL            60                                                      // X location of Yellow Circle
#define XGRN            80                                                      // X location of Green Circle
#define XCNT            100                                                     // X location of Sec Count

#define EW              0                                                       // Number definition of East/West
#define EWLT            1                                                       // Number definition of East/West Left Turn
#define NS              2                                                       // Number definition of North/South
#define NSLT            3                                                       // Number definition of North/South Left Turn
 
#define Color_Off       0                                                       // Number definition of Off Color
#define Color_Red       1                                                       // Number definition of Red Color
#define Color_Green     2                                                       // Number definition of Green Color
#define Color_Yellow    3                                                       // Number definition of Yellow Color

#define EW_Txt_Y        20
#define EW_Cir_Y        EW_Txt_Y + Circle_Offset
#define EW_Count_Y      EW_Txt_Y + Count_Offset
#define EW_Color        ST7735_CYAN

#define EWLT_Txt_Y      50
#define EWLT_Cir_Y      EWLT_Txt_Y + Circle_Offset
#define EWLT_Count_Y    EWLT_Txt_Y + Count_Offset
#define EWLT_Color      ST7735_WHITE

#define NS_Txt_Y        80
#define NS_Cir_Y        NS_Txt_Y + Circle_Offset
#define NS_Count_Y      NS_Txt_Y + Count_Offset
#define NS_Color        ST7735_BLUE 

#define NSLT_Txt_Y      110
#define NSLT_Cir_Y      NSLT_Txt_Y + Circle_Offset
#define NSLT_Count_Y    NSLT_Txt_Y + Count_Offset
#define NSLT_Color      ST7735_MAGENTA

#define PED_EW_Count_Y  30
#define PED_NS_Count_Y  90
#define PED_Count_X     2
#define Switch_Txt_Y    140

#include "ST7735_TFT.c"

char buffer[31];                                                                // general buffer for display purpose
char *nbr;                                                                      // general pointer used for buffer
char *txt;

char EW_Count[]     = "00";                                                     // text storage for EW Count
char EWLT_Count[]   = "00";                                                     // text storage for EW Left Turn Count
char NS_Count[]     = "00";                                                     // text storage for NS Count
char NSLT_Count[]   = "00";                                                     // text storage for NS Left Turn Count

char PED_EW_Count[] = "00";                                                     // text storage for EW Pedestrian Count
char PED_NS_Count[] = "00";                                                     // text storage for NS Pedestrian Count

char SW_EWPED_Txt[] = "0";                                                      // text storage for EW Pedestrian Switch
char SW_EWLT_Txt[]  = "0";                                                      // text storage for EW Left Turn Switch
char SW_NSPED_Txt[] = "0";                                                      // text storage for NS Pedestrian Switch
char SW_NSLT_Txt[]  = "0";                                                      // text storage for NS Left Turn Switch

char SW_MODE_Txt[]  = "D";                                                      // text storage for Mode Light Sensor
	
char Act_Mode_Txt[]  = "D";                                                     // text storage for Actual Mode
char FlashingS_Txt[] = "0";                                                     // text storage for Emergency Status
char FlashingR_Txt[] = "0";                                                     // text storage for Flashing Request

char NS_PED_SW;                                                                 // RAM variable for NS_PED_SW, not connected to any ports
char EW_PED_SW;                                                                 // RAM variable for EW_PED_SW, not connected to any ports         
char FLASHING_REQUEST;                                                          // RAM variable for FLASHING_REQUEST
char FLASHING;                                                                  // RAM variable for FLASHING

char dir;
char Count;                                                                     // RAM variable for Second Count
char PED_Count;                                                                 // RAM variable for Second Pedestrian Count

char SW_EWPED;                                                                  // RAM variable for EW Pedestrian Switch
char SW_EWLT;                                                                   // RAM variable for EW Left Turn Switch
char SW_NSPED;                                                                  // RAM variable for NS Pedestrian Switch
char SW_NSLT;                                                                   // RAM variable for NS Left Turn Switch

char SW_MODE;                                                                   // RAM variable for Mode Light Sensor
int MODE;                                                                       // RAM variable for MODE
char direction;                                                                 // RAM variable for direction
float volt;                                                                     // RAM variable for volt
    
unsigned int get_full_ADC(void);                                                // Start of function prototype
void Init_ADC(void);
void init_IO(void);
void Do_Init(void);
void update_LCD_misc(void);

void Set_NS(char color);
void Set_NSLT(char color);
void Set_EW(char color);
void Set_EWLT(char color);

void PED_Control( char Direction, char Num_Sec);
void Day_Mode(void);
void Night_Mode(void);

void Wait_One_Second(void);
void Wait_Half_Second(void);
void Wait_N_Seconds (char);
void Wait_One_Second_With_Beep(void);

void update_LCD_color(char direction , char color);
void update_LCD_PED_Count(char direction, char count);
void Initialize_Screen(void);
void update_LCD_misc(void);
void update_LCD_count(char direction, char count);

void Do_Flashing();
void INT0_ISR(void);
void INT1_ISR(void);
void INT2_ISR(void);
void interrupt high_priority chkisr(void);                                      // end of function prototype           

void Initialize_Screen()
{
  LCD_Reset();
  TFT_GreenTab_Initialize();
  fillScreen(ST7735_BLACK);
  
  /* TOP HEADER FIELD */
  txt = buffer;
  strcpy(txt, "ECE3301L Spring 21-S1");  
  drawtext(2, 2, txt, ST7735_WHITE, ST7735_BLACK, TS_1);
  
  /* MODE FIELD */
  strcpy(txt, "Mode:");
  drawtext(2, 10, txt, ST7735_WHITE, ST7735_BLACK, TS_1);
  drawtext(35,10, Act_Mode_Txt, ST7735_WHITE, ST7735_BLACK, TS_1);

  /* EMERGENCY REQUEST FIELD */
  strcpy(txt, "FR:");
  drawtext(50, 10, txt, ST7735_WHITE, ST7735_BLACK, TS_1);
  drawtext(70, 10, FlashingR_Txt, ST7735_WHITE, ST7735_BLACK, TS_1);
  
  /* EMERGENCY STATUS FIELD */
  strcpy(txt, "FS:");
  drawtext(80, 10, txt, ST7735_WHITE, ST7735_BLACK, TS_1);
  drawtext(100, 10, FlashingS_Txt, ST7735_WHITE, ST7735_BLACK, TS_1); 
  
  /* SECOND UPDATE FIELD */
  strcpy(txt, "*");
  drawtext(120, 10, txt, ST7735_WHITE, ST7735_BLACK, TS_1);
      
  /* EAST/WEST UPDATE FIELD */
  strcpy(txt, "EAST/WEST");
  drawtext  (XTXT, EW_Txt_Y, txt, EW_Color, ST7735_BLACK, TS_1);
  drawRect  (XTXT, EW_Cir_Y-8, 60, 18, EW_Color);
  fillCircle(XRED, EW_Cir_Y, Circle_Size, ST7735_RED);
  drawCircle(XYEL, EW_Cir_Y, Circle_Size, ST7735_YELLOW);
  drawCircle(XGRN, EW_Cir_Y, Circle_Size, ST7735_GREEN);  
  drawtext  (XCNT, EW_Count_Y, EW_Count, EW_Color, ST7735_BLACK, TS_2);

  /* EAST/WEST LEFT TURN UPDATE FIELD */
  strcpy(txt, "E/W LT");
  drawtext  (XTXT, EWLT_Txt_Y, txt, EWLT_Color, ST7735_BLACK, TS_1);
  drawRect  (XTXT, EWLT_Cir_Y-8, 60, 18, EWLT_Color);  
  fillCircle(XRED, EWLT_Cir_Y, Circle_Size, ST7735_RED);
  drawCircle(XYEL, EWLT_Cir_Y, Circle_Size, ST7735_YELLOW);
  drawCircle(XGRN, EWLT_Cir_Y, Circle_Size, ST7735_GREEN);   
  drawtext  (XCNT, EWLT_Count_Y, EWLT_Count, EWLT_Color, ST7735_BLACK, TS_2);

  /* NORTH/SOUTH UPDATE FIELD */
  strcpy(txt, "NORTH/SOUTH");
  drawtext  (XTXT, NS_Txt_Y  , txt, NS_Color, ST7735_BLACK, TS_1);
  drawRect  (XTXT, NS_Cir_Y-8, 60, 18, NS_Color);
  drawCircle(XRED, NS_Cir_Y  , Circle_Size, ST7735_RED);
  drawCircle(XYEL, NS_Cir_Y  , Circle_Size, ST7735_YELLOW);
  fillCircle(XGRN, NS_Cir_Y  , Circle_Size, ST7735_GREEN);
  drawtext  (XCNT, NS_Count_Y, NS_Count, NS_Color, ST7735_BLACK, TS_2);
    
  /* NORTH/SOUTH LEFT TURN UPDATE FIELD */
  strcpy(txt, "N/S LT");
  drawtext  (XTXT, NSLT_Txt_Y, txt, NSLT_Color, ST7735_BLACK, TS_1);
  drawRect  (XTXT, NSLT_Cir_Y-8, 60, 18, NSLT_Color);
  fillCircle(XRED, NSLT_Cir_Y, Circle_Size, ST7735_RED);
  drawCircle(XYEL, NSLT_Cir_Y, Circle_Size, ST7735_YELLOW);
  drawCircle(XGRN, NSLT_Cir_Y, Circle_Size, ST7735_GREEN);  
  drawtext  (XCNT, NSLT_Count_Y, NSLT_Count, NSLT_Color, ST7735_BLACK, TS_2);
  

  /* EAST/WEST PEDESTRIAM UPDATE FIELD */  
  drawtext(2, PED_EW_Count_Y, PED_EW_Count, EW_Color, ST7735_BLACK, TS_2);
  strcpy(txt, "PEW");  
  drawtext(3, EW_Txt_Y, txt, EW_Color, ST7735_BLACK, TS_1);

  /* NORTH/SOUTH PEDESTRIAM UPDATE FIELD */
  strcpy(txt, "PNS");  
  drawtext(3, NS_Txt_Y, txt, NS_Color, ST7735_BLACK, TS_1);
  drawtext(2, PED_NS_Count_Y, PED_NS_Count, NS_Color, ST7735_BLACK, TS_2);
    
  /* MISCELLANEOUS UPDATE FIELD */  
  strcpy(txt, "EWP EWLT NSP NSLT MD");
  drawtext(1,  Switch_Txt_Y, txt, ST7735_WHITE, ST7735_BLACK, TS_1);
  drawtext(6,  Switch_Txt_Y+9, SW_EWPED_Txt, ST7735_WHITE, ST7735_BLACK, TS_1);
  drawtext(32, Switch_Txt_Y+9, SW_EWLT_Txt, ST7735_WHITE, ST7735_BLACK, TS_1);
  drawtext(58, Switch_Txt_Y+9, SW_NSPED_Txt, ST7735_WHITE, ST7735_BLACK, TS_1);
  drawtext(87, Switch_Txt_Y+9, SW_NSLT_Txt, ST7735_WHITE, ST7735_BLACK, TS_1);
  
  drawtext(112,Switch_Txt_Y+9, SW_MODE_Txt, ST7735_WHITE, ST7735_BLACK, TS_1);
}

void init_UART()
{
    OpenUSART (USART_TX_INT_OFF & USART_RX_INT_OFF &
    USART_ASYNCH_MODE & USART_EIGHT_BIT & USART_CONT_RX &
    USART_BRGH_HIGH, 25);
    OSCCON = 0x60;
}

void putch (char c)
{
    while (!TRMT);
    TXREG = c;
}


void INT0_ISR() 
{ 
 INTCONbits.INT0IF=0;                                                           // Clear the interrupt flag 

 if(MODE == 1){                                                                 // Check if Daytime Mode
     NS_PED_SW = 1;                                                             // Set NS_PED_SW to 1
 }
} 

void INT1_ISR() 
{ 
 INTCON3bits.INT1IF=0;                                                          // Clear the interrupt flag

 if(MODE == 1){                                                                 // Check if Daytime Mode
     EW_PED_SW = 1;                                                             // set EW_PED_SW to 1 
 }
} 

void INT2_ISR() 
{ 
 INTCON3bits.INT2IF=0;                                                          // Clear the interrupt flag                              
 FLASHING_REQUEST = 1;                                                          // Set FLASHING_REQUEST to 1  
}

void interrupt high_priority chkisr()                                           // a high priority interrupt ISR
{                                                                               // if statements are a priority network 
if (INTCONbits.INT0IF == 1) INT0_ISR();                                         // If INT0 Flag is set first proceed with INT0_ISR
if (INTCON3bits.INT1IF == 1) INT1_ISR();                                        // Then Checks if INT1IF is set, proceed with INT1_ISR
if (INTCON3bits.INT2IF == 1) INT2_ISR();                                        // Then Checks if INT2IF is set, proceed with INT2_ISR
} 

void Do_Flashing(){
    FLASHING = 1;                                                               // FlASHING is set to 1
    while(FLASHING == 1)                                                        // Checks if FLASHING is still 1 otherwise exits the while loop
    {
        if(FLASHING_REQUEST == 1)                                               // If FLASHING_REQUEST is pressed after interrupt has occurred 
        {
            FLASHING_REQUEST = 0;                                               // Clear the interrupt in INT0_ISR by setting FLASHING_REQUEST to 0 
            FLASHING = 0;                                                       // Exit while loop by setting FLASHING to 0 
        }
        else                                                                    // This part is to create the FLASHING effect if FLASHING_REQUEST = 0
        {
            Set_EW(RED);                                                        // SET EW RGB to RED
            Set_NS(RED);                                                        // SET NS RGB to RED
            Set_NSLT(RED);                                                      // SET NSLT RGB to RED
            Set_EWLT(RED);                                                      // SET EWLT RGB to RED
            Wait_One_Second();                                                  // Wait One Second
            Set_EW(OFF);                                                        // Turn OFF EW RGB
            Set_NS(OFF);                                                        // Turn OFF NS RGB
            Set_NSLT(OFF);                                                      // Turn OFF NSLT RGB
            Set_EWLT(OFF);                                                      // TURN OFF EWLT RGB
            Wait_One_Second();                                                  // Wait One Second
        }
    }
}

void main(void)
{
    Do_Init();                                                                  // Do_Init() contains all necessary requirements for Interrupts, I/O, ADC, OSSCON
    RBPU = 0;                                                                   // Enable pull up resistor internally in PORTB                                                          
    Initialize_Screen();                                                        // Initialize the TFT screen
  
    int nStep = get_full_ADC();                                                 // calculates the # of steps for analog conversion
    volt = nStep * 5 /1024.0;                                                   // gets the voltage in Volts, using 5V as reference s instead of 4, also divide by 1024 
    SW_MODE = volt < 2.5 ? 1:0;                                                 // Mode = 1, Day_mode, Mode = 0 Night_mode

    while(1)                                                                    // forever loop
    {   
        if (SW_MODE)                                                            // SW_MODE if 1
        {
            Day_Mode();                                                         // calls Day_Mode() function
        }
        else                                                                    // SW_MODE is 0 
        {
            Night_Mode();                                                       // calls Night_Mode() function
        }
        
        if(FLASHING_REQUEST == 1)                                               // FLASHING_REQUEST is 1 from INT2
        {                                               
            FLASHING_REQUEST = 0;                                               // Clear FLASHING_REQUEST to allow future interrupts 
            Do_Flashing();                                                      // call Do_Flashing() function 
        }
    } 
}

void init_IO(){
    TRISA = 0x21;                                                               // PORTA, RA0/AN0 - RA4 are inputs and RA5 is output
    TRISB = 0x07;                                                               // PORTB as input for first 3 bits(the 3 push buttons)
    TRISC = 0x07;                                                               // PORTC as input for first 2 bits (the switches)
    TRISD = 0x00;                                                               // PORTD all bits are output
    TRISE = 0x00;                                                               // PORTE all bits are output 
}

void Init_ADC(void)
{
 ADCON0 = 0 * 4 + 1;
 ADCON1=0x0E ;                                                                  // Select pins AN0 as analog signal and no voltage reference                                                               
 ADCON2=0xA9;                                                                   // right justify the result. Set the bit conversion time (TAD) and
                                                                                // acquisition time
}
void Do_Init(void)
{
    init_UART();                                                                // Initialize the uart 
    Init_ADC();                                                                 // Initialize the ADC  
    init_IO();                                                                  // Initialize all I/O pins
    OSCCON=0x70;                                                                // Set oscillator to 8 MHz  TRISB = 0x?? 

    INTCONbits.INT0IF = 0;                                                      // Clear INT0 flag
    INTCON3bits.INT1IF = 0;                                                     // Clear INT1 flag
    INTCON3bits.INT2IF = 0;                                                     // Clear INT2 flag
    
    INTCONbits.INT0IE = 1;                                                      // enable INT0 
    INTCON3bits.INT1IE = 1;                                                     // enable INT1
    INTCON3bits.INT2IE = 1;                                                     // enable INT2
    
    INTCON2bits.INTEDG0 = 0;                                                    // INT0 EDGE falling
    INTCON2bits.INTEDG1 = 0;                                                    // INT1 EDGE falling
    INTCON2bits.INTEDG2 = 0;                                                    // INT2 EDGE falling
    
    INTCONbits.GIE = 1;                                                         // Set Global Interrupt Enable 
}

unsigned int get_full_ADC()
{
unsigned int result;
   ADCON0bits.GO=1;                                                             // Start Conversion
   while(ADCON0bits.DONE==1);                                                   // wait for conversion to be completed
   result = (ADRESH * 0x100) + ADRESL;                                          // combine result of upper byte and
                                                                                // lower byte into result
   return result;                                                               // return the result.
}

void Set_NS(char color)
{
    direction = NS;                                                             // direction is NS
    update_LCD_color(direction, color);                                         // call update_LCD_color() function
    switch (color)
    {
        case OFF: NS_RED =0;NS_GREEN=0;break;                                   // Turns off the NS LED
        case RED: NS_RED =1;NS_GREEN=0;break;                                   // Sets NS LED RED
        case GREEN: NS_RED =0;NS_GREEN=1;break;                                 // sets NS LED GREEN
        case YELLOW: NS_RED =1;NS_GREEN=1;break;                                // sets NS LED YELLOW
    }
}

void Set_NSLT(char color)
{
    direction = NSLT;                                                           // direction is NSLT
    update_LCD_color(direction,color);                                          // call update_LCD_color() function        
    
    switch(color)
    {
        case OFF: NSLT_RED = 0; NSLT_GREEN = 0; break;                          // if case OFF, then NSLT_RED and NSLT_GREEN are 0 
        case RED: NSLT_RED = 1; NSLT_GREEN = 0; break;                          // if case RED, then NSLT_RED is 1 and NSLT_GREEN is 0
        case GREEN: NSLT_RED = 0; NSLT_GREEN = 1; break;                        // if case GREEN, then NSLT_RED is 0 and NSLT_GREEN is 1
        case YELLOW: NSLT_RED = 1; NSLT_GREEN = 1; break;                       // if case YELLOW, then NSLT_RED and NSLT_GREEN are both 1
    }
}

void Set_EW(char color)
{
    direction = EW;                                                             // direction is EW
    update_LCD_color(direction,color);                                          // call update_LCD_color() function
    switch(color)
    {
        case OFF: EW_RED = 0; EW_GREEN = 0; break;                              // if case OFF, then EW_RED and EW_GREEN are 0 
        case RED: EW_RED = 1; EW_GREEN = 0; break;                              // if case RED, then EW_RED is 1 and EW_GREEN is 0
        case GREEN: EW_RED = 0; EW_GREEN = 1; break;                            // if case GREEN, then EW_RED is 0 and EW_GREEN is 1
        case YELLOW: EW_RED = 1; EW_GREEN = 1; break;                           // if case YELLOW, then EW_RED and EW_GREEN are both 1
    }
}

void Set_EWLT(char color)
{
    direction = EWLT;                                                           // direction is EWLT
    update_LCD_color(direction,color);                                          // call update_LCD_color() function
    switch(color)
    {
        case OFF: EWLT_RED = 0; EWLT_GREEN = 0; break;                          // if case OFF, then EWLT_RED and EWLT_GREEN are 0 
        case RED: EWLT_RED = 1; EWLT_GREEN = 0; break;                          // if case RED, then EWLT_RED is 1 and EWLT_GREEN is 0
        case GREEN: EWLT_RED = 0; EWLT_GREEN = 1; break;                        // if case GREEN, then EWLT_RED is 0 and EWLT_GREEN is 1
        case YELLOW: EWLT_RED = 1; EWLT_GREEN = 1; break;                       // if case YELLOW, then EWLT_RED and EWLT_GREEN are both 1
    }
}

void Activate_Buzzer()
{
    PR2 = 0b11111001;
    T2CON = 0b00000101;
    CCPR2L = 0b01001010;
    CCP2CON = 0b00111100;
}

void Deactivate_Buzzer()
{
    CCP2CON = 0x0;
    PORTBbits.RB3 = 0; 
}

void PED_Control(char direction, char Num_Sec)
{ 
    for(char i = Num_Sec-1;i>0; i--)
    {
        update_LCD_PED_Count(direction, i);                                     // call update_LCD_PED_Count() function where count is i
        Wait_One_Second_With_Beep();                                            // call Wait_One_Second_With_Beep() function
    }
    update_LCD_PED_Count(direction, 0);                                         // call update_LCD_PED_Count() function where count is 0
    Wait_One_Second_With_Beep();                                                // call Wait_One_Second_With_Beep() function
}

void Day_Mode()
{
    MODE = 1;                                                                   // MODE is set to 1 meaning Daytime Mode on LCD 
    MODE_LED = 1;                                                               // turns on the MODE_LED
    Act_Mode_Txt[0] = 'D';                                                      // Show 'D' on LCD for the Mode
                                                            
    Set_NSLT(RED);                                                              // NSLT is RED
    Set_EW(RED);                                                                // EW is RED
    Set_EWLT(RED);                                                              // EWLT is RED
    Set_NS(GREEN);                                                              // NS is GREEN

    if(NS_PED_SW == 1)                                                          // if NS_PED is 1 
    {
        PED_Control(2,6);                                                       // PED_Control Direction is NS, and beeps and wait for 6 seconds
        NS_PED_SW = 0;                                                          // Clear NS_PED_SW once PED_Control is done 
    }
    Wait_N_Seconds(9);                                                          // Wait 9 Seconds
    Set_NS(YELLOW);                                                             // NS is now YELLOW
    Wait_N_Seconds(3);                                                          // Wait 3 Seconds
    Set_NS(RED);                                                                // NS is now RED
  
    
    if(EW_LT == 1)                                                              // if sensor detects EW LT
    {
        Set_EWLT(GREEN);                                                        // EWLT is GREEN
        Wait_N_Seconds(6);                                                      // Wait 6 Seconds
        Set_EWLT(YELLOW);                                                       // EWLT is YELLOW
        Wait_N_Seconds(3);                                                      // Wait 3 Seconds
        Set_EWLT(RED);                                                          // EWLT is RED
    }
    
    Set_EW(GREEN);                                                              // EW is GREEN
    if(EW_PED_SW == 1)                                                          // if pedestrian is crossing road
    {                                                        
        PED_Control(0,9);                                                       // PED_Control Direction is EW, and beeps and waits for 9 seconds
        EW_PED_SW = 0;                                                          // Clear EW_PED_SW once PED_Control is done
    }
    Wait_N_Seconds(7);                                                          // Wait 7 Seconds
    Set_EW(YELLOW);                                                             // EW is YELLOW
    Wait_N_Seconds(3);                                                          // Wait 3 Seconds
    Set_EW(RED);  
    
    if(NS_LT == 1)                                                              // if sensor detects NS left turn
    {
        Set_NSLT(GREEN);                                                        // NSLT is GREEN
        Wait_N_Seconds(6);                                                      // Wait 6 Seconds
        Set_NSLT(YELLOW);                                                       // NSLT is YELLOW
        Wait_N_Seconds(3);                                                      // Wait 3 Seconds
        Set_NSLT(RED);                                                          // NSLT is RED       
    }
}
void Night_Mode()
{ 
    MODE = 0;                                                                   // Mode is Night time 
    MODE_LED = 0;                                                               // turns off the MODE_LED
    Act_Mode_Txt[0] = 'N';                                                      // Mode on LCD is updated to 'N' meaning night time 
    EW_PED_SW = 0;                                                              // Clear EW_PED_SW to make sure if anything goes wrong EW_PED_SW will not be 1 
    NS_PED_SW = 0;                                                              // Clear NS_PED_SW to make sure if anything goes wrong NS_PED_SW will not be 1 
    
    Set_NSLT(RED);                                                              // NSLT is RED
    Set_EW(RED);                                                                // EW is RED
    Set_EWLT(RED);                                                              // EWLT is RED
    Set_NS(GREEN);                                                              // NS is GREEN
    Wait_N_Seconds(8);                                                          // Wait 8 Seconds
    Set_NS(YELLOW);                                                             // NS is YELLOW
    Wait_N_Seconds(3);                                                          // Wait 3 Seconds
    Set_NS(RED);                                                                // NS is RED
    
    if(EW_LT == 1)                                                              // if sensor detects EW left turn 
    {                                                             
        Set_EWLT(GREEN);                                                        // EWLT is GREEN
        Wait_N_Seconds(6);                                                      // Wait 6 Seconds
        Set_EWLT(YELLOW);                                                       // EWLT is YELLOW
        Wait_N_Seconds(3);                                                      // Wait 3 Seconds
        Set_EWLT(RED);                                                          // EWLT is RED
    }
    Set_EW(GREEN);                                                              // EW is GREEN
    Wait_N_Seconds(8);                                                          // Wait 8 Seconds
    Set_EW(YELLOW);                                                             // EW is YELLOW
    Wait_N_Seconds(3);                                                          // Wait 3 Seconds
    Set_EW(RED);                                                                // EW is RED
    
    if(NS_LT == 1)                                                              // if sensor detects NS left turn 
    {                                                             
        Set_NSLT(GREEN);                                                        // NSLT is GREEN
        Wait_N_Seconds(6);                                                      // Wait 6 Seconds
        Set_NSLT(YELLOW);                                                       // NSLT is YELLOW
        Wait_N_Seconds(3);                                                      // Wait 3 Seconds
        Set_NSLT(RED);                                                          // NSLT is RED
    }
}

void Old_Wait_One_Second()                                                      //creates one second delay and blinking asterisk
{
    SEC_LED = 1;                                                                // SEC_LED is ON
    Wait_Half_Second();                                                         // Wait for half second (or 500 msec)
    SEC_LED = 0;                                                                // SEC LED is OFF
    Wait_Half_Second();                                                         // Wait for half second (or 500 msec)

}

void Wait_One_Second()                                                          //creates one second delay and blinking asterisk
{
    SEC_LED = 1;                                                                // SEC_LED is ON
    strcpy(txt,"*");
    drawtext(120,10,txt,ST7735_WHITE,ST7735_BLACK,TS_1);
    Wait_Half_Second();                                                         // Wait for half second (or 500 msec)

    SEC_LED = 0;                                                                // SEC_LED is OFF
    strcpy(txt," ");
    drawtext(120,10,txt,ST7735_WHITE,ST7735_BLACK,TS_1);
    Wait_Half_Second();                                                         // Wait for half second (or 500 msec)
    update_LCD_misc();                                                          // call update_LCD_misc()
}

void Wait_One_Second_With_Beep()                                                // creates one second delay as well as sound buzzer
{
    SEC_LED = 1;                                                                // SEC_LED at PORTD7 is ON
    Activate_Buzzer();                                                          // Turn on beep noise on speaker
    Wait_Half_Second();                                                         // Wait for half second (or 500 msec)
    SEC_LED = 0;                                                                // SEC_LED at PORTD7 is OFF
    Deactivate_Buzzer();                                                        // Turn off beep noise on speaker
    Wait_Half_Second();                                                         // Wait for half second (or 500 msec)
}
    
void Wait_Half_Second()
{
    T0CON = 0x03;                                                               // Timer 0, 16-bit mode, prescaler 1:16
    TMR0L = 0xDB;                                                               // set the lower byte of TMR
    TMR0H = 0x0B;                                                               // set the upper byte of TMR
    INTCONbits.TMR0IF = 0;                                                      // clear the Timer 0 flag
    T0CONbits.TMR0ON = 1;                                                       // Turn on the Timer 0
    while (INTCONbits.TMR0IF == 0);                                             // wait for the Timer Flag to be 1 for done
    T0CONbits.TMR0ON = 0;                                                       // turn off the Timer 0
}

void Wait_N_Seconds (char seconds)
{
    char I;
    for (I = seconds; I> 0; I--)
    {
        update_LCD_count(direction, I);                                         // call update_LCD_count() where count is I
        Wait_One_Second();                                                      // calls Wait_One_Second for x number of times
    }
    update_LCD_count(direction, 0);                                             // call update_LCD_count() where count is 0
}

void update_LCD_color(char direction, char color)
{
    char Circle_Y;
    Circle_Y = EW_Cir_Y + direction * 30;    
    
    if (color == Color_Off)                                                     //if Color off make all circles black but leave outline
    {
        fillCircle(XRED, Circle_Y, Circle_Size, ST7735_BLACK);
        fillCircle(XYEL, Circle_Y, Circle_Size, ST7735_BLACK);
        fillCircle(XGRN, Circle_Y, Circle_Size, ST7735_BLACK); 
        drawCircle(XRED, Circle_Y, Circle_Size, ST7735_RED);            
        drawCircle(XYEL, Circle_Y, Circle_Size, ST7735_YELLOW);
        drawCircle(XGRN, Circle_Y, Circle_Size, ST7735_GREEN);                       
    }    
    
    if (color == Color_Red)                                                     //if the color is red only fill the red circle with red
    {
        fillCircle(XRED, Circle_Y, Circle_Size, ST7735_RED);
        fillCircle(XYEL, Circle_Y, Circle_Size, ST7735_BLACK);
        fillCircle(XGRN, Circle_Y, Circle_Size, ST7735_BLACK); 
        drawCircle(XRED, Circle_Y, Circle_Size, ST7735_RED);            
        drawCircle(XYEL, Circle_Y, Circle_Size, ST7735_YELLOW);
        drawCircle(XGRN, Circle_Y, Circle_Size, ST7735_GREEN);  
    }
          
    if (color == Color_Yellow)                                                  //if the color is red only fill the red circle with red
    {
        fillCircle(XRED, Circle_Y, Circle_Size, ST7735_BLACK);
        fillCircle(XYEL, Circle_Y, Circle_Size, ST7735_YELLOW);
        fillCircle(XGRN, Circle_Y, Circle_Size, ST7735_BLACK); 
        drawCircle(XRED, Circle_Y, Circle_Size, ST7735_RED);            
        drawCircle(XYEL, Circle_Y, Circle_Size, ST7735_YELLOW);
        drawCircle(XGRN, Circle_Y, Circle_Size, ST7735_GREEN);  
    }
    if (color == Color_Green)                                                   //if the color is red only fill the red circle with red
    {
        fillCircle(XRED, Circle_Y, Circle_Size, ST7735_BLACK);
        fillCircle(XYEL, Circle_Y, Circle_Size, ST7735_BLACK);
        fillCircle(XGRN, Circle_Y, Circle_Size, ST7735_GREEN); 
        drawCircle(XRED, Circle_Y, Circle_Size, ST7735_RED);            
        drawCircle(XYEL, Circle_Y, Circle_Size, ST7735_YELLOW);
        drawCircle(XGRN, Circle_Y, Circle_Size, ST7735_GREEN);  
    }
}

void update_LCD_count(char direction, char count)
{
   switch (direction)                                                           //update traffic light no ped time
   {
        case EW:        
        EW_Count[0] = count/10  + '0';
        EW_Count[1] = count%10  + '0';
        drawtext(XCNT, EW_Count_Y, EW_Count, EW_Color, ST7735_BLACK, TS_2);                
        break;
      
        case EWLT:        
        EWLT_Count[0] = count/10  + '0';
        EWLT_Count[1] = count%10  + '0';
        drawtext(XCNT, EWLT_Count_Y, EWLT_Count, EWLT_Color, ST7735_BLACK, TS_2);                
        break;
        
        case NS:        
        NS_Count[0] = count/10  + '0';
        NS_Count[1] = count%10  + '0';
        drawtext(XCNT, NS_Count_Y, NS_Count, NS_Color, ST7735_BLACK, TS_2);                
        break;
        
        case NSLT:        
        NSLT_Count[0] = count/10  + '0';
        NSLT_Count[1] = count%10  + '0';
        drawtext(XCNT, NSLT_Count_Y, NSLT_Count, NSLT_Color, ST7735_BLACK, TS_2);                
        break;
    }  
}

void update_LCD_PED_Count(char direction, char count)
{
   switch (direction)
   {
      case EW:       
        PED_EW_Count[0] = count/10  + '0';                                      // PED count upper digit
        PED_EW_Count[1] = count%10  + '0';                                      // PED Lower
        drawtext(PED_Count_X, PED_EW_Count_Y, PED_EW_Count, EW_Color, ST7735_BLACK, TS_2);        
        break;
        
      case NS:       
        PED_NS_Count[0] = count/10  + '0';                                      // PED count upper digit
        PED_NS_Count[1] = count%10  + '0';                                      // PED Lower
        drawtext(PED_Count_X, PED_NS_Count_Y, PED_NS_Count, NS_Color, ST7735_BLACK, TS_2);        
        break;
   } 
}

void update_LCD_misc()
{
    char ch = 0;
    ADCON0 = ch*4 + 1;
    int nStep = get_full_ADC();                                                 // calculates the # of steps for analog conversion
    volt = nStep * 5 /1024.0;                                                   // gets the voltage in Volts, using 5V as reference s instead of 4, also divide by 1024 
    SW_MODE = volt < 2.5 ? 1:0;                                                 // Mode = 1, Day_mode, Mode = 0 Night_mode
    
    SW_EWPED = EW_PED_SW;                                                       // create a variable SW_EWPED to store EW_PED_SW result
    SW_EWLT = EW_LT;                                                            // create a variable SW_EWLT to store EW_LT result
    SW_NSPED = NS_PED_SW;                                                       // create a variable SW_NSPED to store NS_PED_SW result
    SW_NSLT = NS_LT;                                                            // create a variable SW_NSLT to store NS_LT result
     
    if (SW_MODE == 0) SW_MODE_Txt[0] = 'N'; else SW_MODE_Txt[0] = 'D';          // Set Text at bottom of screen to switch states for MODE
    if (SW_EWPED == 0) SW_EWPED_Txt[0] = '0'; else SW_EWPED_Txt[0] = '1';       // Set Text at bottom of screen to switch states for EWPED
    if (SW_EWLT == 0) SW_EWLT_Txt[0] = '0'; else SW_EWLT_Txt[0] = '1';          // Set Text at bottom of screen to switch states for EWLT
    if (SW_NSPED == 0) SW_NSPED_Txt[0] = '0'; else SW_NSPED_Txt[0] = '1';       // Set Text at bottom of screen to switch states for NSPED
    if (SW_NSLT == 0) SW_NSLT_Txt[0] = '0'; else SW_NSLT_Txt[0] = '1';          // Set Text at bottom of screen to switch states for NSLT

    if (FLASHING_REQUEST == 0) FlashingR_Txt[0] = '0'; else FlashingR_Txt[0] = '1'; // Update FR on LCD on top of the screen
    if (FLASHING == 0) FlashingS_Txt[0] = '0'; else FlashingS_Txt[0] = '1';     // Update FS on LCD on top of the screen
    
    drawtext(35,10, Act_Mode_Txt, ST7735_WHITE, ST7735_BLACK, TS_1);            // Show switch and sensor states at bottom of the screen
    drawtext(6,   Switch_Txt_Y+9, SW_EWPED_Txt, ST7735_WHITE, ST7735_BLACK, TS_1);
    drawtext(32,  Switch_Txt_Y+9, SW_EWLT_Txt, ST7735_WHITE, ST7735_BLACK, TS_1);
    drawtext(58,  Switch_Txt_Y+9, SW_NSPED_Txt, ST7735_WHITE, ST7735_BLACK, TS_1);       
    drawtext(87,  Switch_Txt_Y+9, SW_NSLT_Txt, ST7735_WHITE, ST7735_BLACK, TS_1);
    drawtext(112, Switch_Txt_Y+9, SW_MODE_Txt, ST7735_WHITE, ST7735_BLACK, TS_1);  
    
    drawtext(70, 10, FlashingR_Txt, ST7735_WHITE, ST7735_BLACK, TS_1);          // draw FR and FS, the updated value 
    drawtext(100, 10, FlashingS_Txt, ST7735_WHITE, ST7735_BLACK, TS_1);
}



