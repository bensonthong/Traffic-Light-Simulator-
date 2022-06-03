#include <stdio.h>
#include <stdlib.h>
#include <xc.h>
#include <math.h>
#include <p18f4620.h>


#pragma config OSC = INTIO67
#pragma config WDT = OFF
#pragma config LVP = OFF
#pragma config BOREN = OFF
#pragma config CCP2MX = PORTBE


#define _XTAL_FREQ      8000000         // Set operation for 8 Mhz
#define TMR_CLOCK       _XTAL_FREQ/4    // Timer Clock 2 Mhz

#define TFT_DC      PORTDbits.RD0          		// Location of TFT D/C
#define TFT_CS      PORTDbits.RD1       		// Location of TFT Chip Select
#define TFT_RST     PORTDbits.RD2       		// Location of TFT Reset

#define SEC_LED     PORTDbits.RD7

#define NS_RED      PORTAbits.RA1 
#define NS_GREEN    PORTAbits.RA2 

#define NSLT_RED    PORTDbits.RD3
#define NSLT_GREEN  PORTDbits.RD4

#define EW_RED      PORTDbits.RD5 
#define EW_GREEN    PORTDbits.RD6 

#define EWLT_RED    PORTEbits.RE0
#define EWLT_GREEN  PORTEbits.RE1

#define NS_PED      PORTAbits.RA5
#define NS_LT       PORTCbits.RC1 

#define EW_PED      PORTCbits.RC0
#define EW_LT       PORTCbits.RC2

#define MODE_LED    PORTEbits.RE2

// colors
#define OFF 		0               // Defines OFF as decimal value 0
#define RED 		1               // Defines RED as decimal value 1
#define GREEN 		2               // Defines GREEN as decimal value 2
#define YELLOW 		3               // Defines YELLOW as decimal value 3

#define Circle_Size     7               // Size of Circle for Light
#define Circle_Offset   15              // Location of Circle
#define TS_1            1               // Size of Normal Text
#define TS_2            2               // Size of PED Text
#define Count_Offset    10              // Location of Count

#define XTXT            30              // X location of Title Text 
#define XRED            40              // X location of Red Circle
#define XYEL            60              // X location of Yellow Circle
#define XGRN            80              // X location of Green Circle
#define XCNT            100             // X location of Sec Count

#define EW              0               // Number definition of East/West
#define EWLT            1               // Number definition of East/West Left Turn
#define NS              2               // Number definition of North/South
#define NSLT            3               // Number definition of North/South Left Turn
 
#define Color_Off       0               // Number definition of Off Color
#define Color_Red       1               // Number definition of Red Color
#define Color_Green     2               // Number definition of Green Color
#define Color_Yellow    3               // Number definition of Yellow Color

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


char buffer[31];                        // general buffer for display purpose
char *nbr;                              // general pointer used for buffer
char *txt;

char EW_Count[]     = "00";             // text storage for EW Count
char EWLT_Count[]   = "00";             // text storage for EW Left Turn Count
char NS_Count[]     = "00";             // text storage for NS Count
char NSLT_Count[]   = "00";             // text storage for NS Left Turn Count

char PED_EW_Count[] = "00";             // text storage for EW Pedestrian Count
char PED_NS_Count[] = "00";             // text storage for NS Pedestrian Count

char SW_EWPED_Txt[] = "0";              // text storage for EW Pedestrian Switch
char SW_EWLT_Txt[]  = "0";              // text storage for EW Left Turn Switch
char SW_NSPED_Txt[] = "0";              // text storage for NS Pedestrian Switch
char SW_NSLT_Txt[]  = "0";              // text storage for NS Left Turn Switch

char SW_MODE_Txt[]  = "D";              // text storage for Mode Light Sensor
	
char Act_Mode_Txt[]  = "D";             // text storage for Actual Mode
char FlashingS_Txt[] = "0";             // text storage for Emergency Status
char FlashingR_Txt[] = "0";             // text storage for Flashing Request

char dir;
char Count;                             // RAM variable for Second Count
char PED_Count;                         // RAM variable for Second Pedestrian Count

char SW_EWPED;                          // RAM variable for EW Pedestrian Switch
char SW_EWLT;                           // RAM variable for EW Left Turn Switch
char SW_NSPED;                          // RAM variable for NS Pedestrian Switch
char SW_NSLT;                           // RAM variable for NS Left Turn Switch

char SW_MODE;                           // RAM variable for Mode Light Sensor
int MODE;
char direction;
float volt;
    
unsigned int get_full_ADC(void);
void Init_ADC(void);
void init_IO(void);


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

void update_LCD_color(char , char );
void update_LCD_PED_Count(char direction, char count);
void Initialize_Screen(void);
void update_LCD_misc(void);
void update_LCD_count(char, char);

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

void main(void)
{
    init_IO();
    Init_ADC();
    init_UART();
    OSCCON = 0x70; 
    RBPU = 0;    
    // set the system clock to be 1MHz 1/4 of the 4MHz
    Initialize_Screen();                        // Initialize the TFT screen


    int nStep = get_full_ADC();         // calculates the # of steps for analog conversion
    volt = nStep * 5 /1024.0;       // gets the voltage in Volts, using 5V as reference s instead of 4, also divide by 1024 
    SW_MODE = volt < 3.5 ? 1:0;        // Mode = 1, Day_mode, Mode = 0 Night_mode

    while(1)                                    // forever loop
    {
        if (SW_MODE)    
        {
            Day_Mode();                         // calls Day_Mode() function
        }
        else
        {
            Night_Mode();                       // calls Night_Mode() function
        }
     
    } 
}

void init_IO(){
    TRISA = 0x21;                                                               // PORTA, RA0/AN0 - RA4 are inputs and RA5 is output
    TRISB = 0x07;                                                               // PORTB as output
    TRISC = 0x07;                                                               // PORTC as output
    TRISD = 0x00;                                                               // PORTD as output
    TRISE = 0x00;                                                               // PORTE as output 
}

void Init_ADC(void)
{
 ADCON1=0x0E ;                                                                  // Select pins AN0 as analog signal and no voltage reference                                                               
 ADCON2=0xA9;                                                                   // right justify the result. Set the bit conversion time (TAD) and
                                                                                // acquisition time
}

unsigned int get_full_ADC()
{
unsigned int result;
   ADCON0bits.GO=1;                     // Start Conversion
   while(ADCON0bits.DONE==1);           // wait for conversion to be completed
   result = (ADRESH * 0x100) + ADRESL;  // combine result of upper byte and
                                        // lower byte into result
   return result;                       // return the result.
}

void Set_NS(char color)
{
    direction = NS;
    update_LCD_color(direction, color);
    
    switch (color)
    {
        case OFF: NS_RED =0;NS_GREEN=0;break;           // Turns off the NS LED
        case RED: NS_RED =1;NS_GREEN=0;break;           // Sets NS LED RED
        case GREEN: NS_RED =0;NS_GREEN=1;break;         // sets NS LED GREEN
        case YELLOW: NS_RED =1;NS_GREEN=1;break;        // sets NS LED YELLOW
    }
}

void Set_NSLT(char color)
{
    direction = NSLT;
    update_LCD_color(direction,color);
    
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
    direction = EW;
    update_LCD_color(direction,color);
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
    direction = EWLT;
    update_LCD_color(direction,color);
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

void PED_Control( char direction, char Num_Sec)
{ 
    for(char i = Num_Sec-1;i>0; i--)
    {
        update_LCD_PED_Count(direction, i);
        Wait_One_Second_With_Beep();            // hold the number on 7-Segment for 1 second
    }
    
    update_LCD_PED_Count(direction, 0);         //                            //
    Wait_One_Second_With_Beep();                // leaves the 7-Segment off for 1 second
}

void Day_Mode()
{
    MODE = 1;                                   // turns on the MODE_LED
    MODE_LED = 1;
    Act_Mode_Txt[0] = 'D';
    
    Set_NS(GREEN);                                                              // NS is GREEN
    Set_NSLT(RED);                                                              // NSLT is RED
    Set_EW(RED);                                                                // EW is RED
    Set_EWLT(RED);                                                              // EWLT is RED
    if(NS_PED == 1)                                                             // if NS_PED is 1 
    {
        PED_Control(0,6);                                                       // PED_Control Direction is NS, and beeps and wait for 6 seconds
        Wait_N_Seconds(9);                                                      // Wait 9 Seconds
        Set_NS(YELLOW);                                                         // NS is now YELLOW
        Wait_N_Seconds(3);                                                      // Wait 3 Seconds
        Set_NS(RED);                                                            // NS is now RED
    }
    else                                                                        // If no pedestrians are crossing road
    {
        Wait_N_Seconds(9);                                                      // Wait 9 Seconds
        Set_NS(YELLOW);                                                         // NS is YELLOW
        Wait_N_Seconds(3);                                                      // Wait 3 Seconds
        Set_NS(RED);                                                            // NS is RED
    }
    
    if(EW_LT == 1)                                                              // if sensor detects EW LT
    {
        Set_EWLT(GREEN);                                                        // EWLT is GREEN
        Wait_N_Seconds(6);                                                      // Wait 6 Seconds
        Set_EWLT(YELLOW);                                                       // EWLT is YELLOW
        Wait_N_Seconds(3);                                                      // Wait 3 Seconds
        Set_EWLT(RED);                                                          // EWLT is RED
        
        Set_EW(GREEN);                                                          // EW is GREEN
        if(EW_PED == 1)                                                         // if pedestrian is crossing road
        {                                                        
            PED_Control(1,9);                                                   // PED_Control Direction is EW, and beeps and waits for 9 seconds
            Wait_N_Seconds(7);                                                  // Wait 7 Seconds
            Set_EW(YELLOW);                                                     // EW is YELLOW
            Wait_N_Seconds(3);                                                  // Wait 3 Seconds
            Set_EW(RED);                                                        // EW is RED
            
        }
        else                                                                    // if no pedestrian crossing road
        {
            Wait_N_Seconds(7);                                                  // Wait 7 Seconds
            Set_EW(YELLOW);                                                     // EW is YELLOW
            Wait_N_Seconds(3);                                                  // Wait 3 Seconds
            Set_EW(RED);                                                        // EW is RED
        }
    }
    else                                                                        // if there is no sensor detecting left turn
    {
        Set_EW(GREEN);                                                          // EW is GREEN
        if(EW_PED == 1)                                                         // if pedestrian is crossing road
        {                                                        
            PED_Control(1,9);                                                   // PED_Control Direction is EW, and beeps and waits for 9 seconds
            Wait_N_Seconds(7);                                                  // Wait 7 Seconds
            Set_EW(YELLOW);                                                     // EW is YELLOW
            Wait_N_Seconds(3);                                                  // Wait 3 Seconds
            Set_EW(RED);                                                        // EW is RED
            
        }
        else                                                                    // if no pedestrian crossing road
        {
            Wait_N_Seconds(7);                                                  // Wait 7 Seconds
            Set_EW(YELLOW);                                                     // EW is YELLOW
            Wait_N_Seconds(3);                                                  // Wait 3 Seconds
            Set_EW(RED);                                                        // EW is RED
        }
    }
    
    if(NS_LT == 1)                                                              // if sensor detects NS left turn
    {
      Set_NSLT(GREEN);                                                          // NSLT is GREEN
      Wait_N_Seconds(6);                                                        // Wait 6 Seconds
      Set_NSLT(YELLOW);                                                         // NSLT is YELLOW
      Wait_N_Seconds(3);                                                        // Wait 3 Seconds
      Set_NSLT(RED);                                                            // NSLT is RED
    }
}
void Night_Mode()
{ 
    MODE = 0;                                   // turns on the MODE_LED
    MODE_LED = 0;
    Act_Mode_Txt[0] = 'N';
    
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
        Set_EW(GREEN);                                                          // EW is GREEN
        Wait_N_Seconds(8);                                                      // Wait 8 Seconds
        Set_EW(YELLOW);                                                         // EW is YELLOW
        Wait_N_Seconds(3);                                                      // Wait 3 Seconds
        Set_EW(RED);                                                            // EW is RED
    }
    else
    {
        Set_EW(GREEN);                                                          // EW is GREEN
        Wait_N_Seconds(8);                                                      // Wait 8 Seconds
        Set_EW(YELLOW);                                                         // EW is YELLOW
        Wait_N_Seconds(3);                                                      // Wait 3 Seconds
        Set_EW(RED);                                                            // EW is RED
    }
    
    if(NS_LT == 1)                                                              // if sensor detects NS left turn 
    {                                                             
        Set_NSLT(GREEN);                                                        // NSLT is GREEN
        Wait_N_Seconds(6);                                                      // Wait 6 Seconds
        Set_NSLT(YELLOW);                                                       // NSLT is YELLOW
        Wait_N_Seconds(3);                                                      // Wait 3 Seconds
        Set_NSLT(RED);                                                          // NSLT is RED
    }

}

void Old_Wait_One_Second()                      //creates one second delay and blinking asterisk
{
    SEC_LED = 1;
    Wait_Half_Second();                         // Wait for half second (or 500 msec)
    SEC_LED = 0;
    Wait_Half_Second();                         // Wait for half second (or 500 msec)

}

void Wait_One_Second()							//creates one second delay and blinking asterisk
{
    SEC_LED = 1;
    strcpy(txt,"*");
    drawtext(120,10,txt,ST7735_WHITE,ST7735_BLACK,TS_1);
    Wait_Half_Second();                         // Wait for half second (or 500 msec)

    SEC_LED = 0;
    strcpy(txt," ");
    drawtext(120,10,txt,ST7735_WHITE,ST7735_BLACK,TS_1);
    Wait_Half_Second();                         // Wait for half second (or 500 msec)
    update_LCD_misc();
}

void Wait_One_Second_With_Beep()				// creates one second delay as well as sound buzzer
{
    SEC_LED = 1;                                                                // SEC_LED at PORTD7 is ON
    Activate_Buzzer();                                                            // Turn on beep noise on speaker
    Wait_Half_Second();                                                         // Wait for half second (or 500 msec)
    SEC_LED = 0;                                                                // SEC_LED at PORTD7 is OFF
    Deactivate_Buzzer();                                                          // Turn off beep noise on speaker
    Wait_Half_Second();                                                         // Wait for half second (or 500 msec)
}
    
void Wait_Half_Second()
{
    T0CON = 0x03;                               // Timer 0, 16-bit mode, prescaler 1:16
    TMR0L = 0xDB;                               // set the lower byte of TMR
    TMR0H = 0x0B;                               // set the upper byte of TMR
    INTCONbits.TMR0IF = 0;                      // clear the Timer 0 flag
    T0CONbits.TMR0ON = 1;                       // Turn on the Timer 0
    while (INTCONbits.TMR0IF == 0);             // wait for the Timer Flag to be 1 for done
    T0CONbits.TMR0ON = 0;                       // turn off the Timer 0
}

void Wait_N_Seconds (char seconds)
{
    char I;
    for (I = seconds; I> 0; I--)
    {
        Wait_One_Second();                      // calls Wait_One_Second for x number of times
        update_LCD_count(direction, I);        
    }
    update_LCD_count(direction, 0);   
}

void update_LCD_color(char direction, char color)
{
    char Circle_Y;
    Circle_Y = EW_Cir_Y + direction * 30;    
    
    if (color == Color_Off)     //if Color off make all circles black but leave outline
    {
        fillCircle(XRED, Circle_Y, Circle_Size, ST7735_BLACK);
        fillCircle(XYEL, Circle_Y, Circle_Size, ST7735_BLACK);
        fillCircle(XGRN, Circle_Y, Circle_Size, ST7735_BLACK); 
        drawCircle(XRED, Circle_Y, Circle_Size, ST7735_RED);            
        drawCircle(XYEL, Circle_Y, Circle_Size, ST7735_YELLOW);
        drawCircle(XGRN, Circle_Y, Circle_Size, ST7735_GREEN);                       
    }    
    
    if (color == Color_Red)     //if the color is red only fill the red circle with red
    {
        fillCircle(XRED, Circle_Y, Circle_Size, ST7735_RED);
        fillCircle(XYEL, Circle_Y, Circle_Size, ST7735_BLACK);
        fillCircle(XGRN, Circle_Y, Circle_Size, ST7735_BLACK); 
        drawCircle(XRED, Circle_Y, Circle_Size, ST7735_RED);            
        drawCircle(XYEL, Circle_Y, Circle_Size, ST7735_YELLOW);
        drawCircle(XGRN, Circle_Y, Circle_Size, ST7735_GREEN);  
    }
          
    // put code here
    if (color == Color_Yellow)     //if the color is red only fill the red circle with red
    {
        fillCircle(XRED, Circle_Y, Circle_Size, ST7735_BLACK);
        fillCircle(XYEL, Circle_Y, Circle_Size, ST7735_YELLOW);
        fillCircle(XGRN, Circle_Y, Circle_Size, ST7735_BLACK); 
        drawCircle(XRED, Circle_Y, Circle_Size, ST7735_RED);            
        drawCircle(XYEL, Circle_Y, Circle_Size, ST7735_YELLOW);
        drawCircle(XGRN, Circle_Y, Circle_Size, ST7735_GREEN);  
    }
    if (color == Color_Green)     //if the color is red only fill the red circle with red
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
   switch (direction)                   //update traffic light no ped time
   {
      case EW:        
        EW_Count[0] = count/10  + '0';
        EW_Count[1] = count%10  + '0';
        drawtext(XCNT, EW_Count_Y, EW_Count, EW_Color, ST7735_BLACK, TS_2);                
        break;
      
    // put code here
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
        PED_EW_Count[0] = count/10  + '0';          // PED count upper digit
        PED_EW_Count[1] = count%10  + '0';          // PED Lower
        drawtext(PED_Count_X, PED_EW_Count_Y, PED_EW_Count, EW_Color, ST7735_BLACK, TS_2);        
        break;
      // put code here  
       case NS:       
        PED_NS_Count[0] = count/10  + '0';          // PED count upper digit
        PED_NS_Count[1] = count%10  + '0';          // PED Lower
        drawtext(PED_Count_X, PED_NS_Count_Y, PED_NS_Count, NS_Color, ST7735_BLACK, TS_2);        
        break;
   }
   
}

void update_LCD_misc()
{
    char ch = 0;
    ADCON0 = ch*4 + 1;
    int nStep = get_full_ADC();         // calculates the # of steps for analog conversion
    volt = nStep * 5 /1024.0;           // gets the voltage in Volts, using 5V as reference s instead of 4, also divide by 1024 
    
    SW_MODE = volt < 3.3 ? 1:0;         // Mode = 1, Day_mode, Mode = 0 Night_mode
    SW_EWPED = EW_PED;
    SW_EWLT = EW_LT;    
    SW_NSPED = NS_PED;
    SW_NSLT = NS_LT;
    
       // put code here  
    if(SW_MODE == 0) SW_MODE_Txt[0] = 'N'; else SW_MODE_Txt[0] = 'D';
    if (SW_EWPED == 0) SW_EWPED_Txt[0] = '0'; else SW_EWPED_Txt[0] = '1';      // Set Text at bottom of screen to switch states
    if (SW_EWLT == 0) SW_EWLT_Txt[0] = '0'; else SW_EWLT_Txt[0] = '1';      // Set Text at bottom of screen to switch states
    if (SW_NSPED == 0) SW_NSPED_Txt[0] = '0'; else SW_NSPED_Txt[0] = '1';      // Set Text at bottom of screen to switch states
    if (SW_NSLT == 0) SW_NSLT_Txt[0] = '0'; else SW_NSLT_Txt[0] = '1';      // Set Text at bottom of screen to switch states
    
    drawtext(35,10, Act_Mode_Txt, ST7735_WHITE, ST7735_BLACK, TS_1);    
    drawtext(6,   Switch_Txt_Y+9, SW_EWPED_Txt, ST7735_WHITE, ST7735_BLACK, TS_1);
    drawtext(32,  Switch_Txt_Y+9, SW_EWLT_Txt, ST7735_WHITE, ST7735_BLACK, TS_1);
    drawtext(58,  Switch_Txt_Y+9, SW_NSPED_Txt, ST7735_WHITE, ST7735_BLACK, TS_1);       // Show switch and sensor states at bottom of the screen
    drawtext(87,  Switch_Txt_Y+9, SW_NSLT_Txt, ST7735_WHITE, ST7735_BLACK, TS_1);

    drawtext(112,  Switch_Txt_Y+9, SW_MODE_Txt, ST7735_WHITE, ST7735_BLACK, TS_1);
                  
}



