



/**************************************************************************************************************************
 ******** Name: Keyi Liu
 ******** Student ID: s1703084
 ******** Lab Group: Wednesdays
 **************************************************************************************************************************/



/**************************************************************************************************************************
 * Basic Calculator
 **************************************************************************************************************************
 * functionalities achieved under this project :
 *
 * (Requirements)
 * 1. build a 8-bit calculator including 2 inputs
 * 2. 4 push button are used to select different arithmetic operations which includes(+,-,x,/)
 * 3. the display of calculated results are demonstrated through 7-segment display and LEDs
 *
 * (extra features on basic Calculator)
 * 1. every input Value can be demonstrated through 7-segment display and LEDs
 * 2. input values can be corrected at any time before applying any arithmetic operations
 * 3. corrected values can be updated and demonstrated through 7-segment display and LEDs immediately
 * when pressing related buttons
 * 4. If the output is negative, it can also be demonstrated through 7-segment display and LEDs
 *
 *
 **************************************************************************************************************************
 * Instruction manual
 **************************************************************************************************************************
 *
 * 1.Press BTNL(left button)>>>>>> Starting the calculator
 *
 * -----------------------------------------after calculator start---------------------------------------------------------------
 * 2.Press BTND(down button)>>>>>>>> check the decimal number of input-1(switchValue1)
 *   press BTNU(up button)>>>>>> check the decimal number of input-2(switchValue2)
 *
 *   !!!!!!!!!!!!! Note!!!!!!!!!
 *   a. the two inputs can be corrected by pushing slide switches at any time before applying arithmetic operations)
 *   b. corrected values can be updated and demonstrated through 7-segment display and LEDs immediately
 * when pressing UP or DOWN button again)
 *
 * 3.press BTNR(right button)>>>>>go the the computation mode
 *   #press BTNR again>>>>>doing the addition
 *   #press BTNL >>>>>>>>>>doing the division
 *   #press BTNU >>>>>>>>>>doing the subtraction
 *   #press BTND >>>>>>>>>>doing the multiplication
 *
 *   !!!!!!!!!!! Note!!!!!!!!!!
 *   c.once applied one of four arithmetic operations, the finalResult will be hold forever before pressing BTNC(reset)
 *   d.press BTNC(reset) and press BTNL(start) to restart the calculation go for other values or arithmetic operations
 *
 **************************************************************************************************************************
 */


/*
 * This application configures UART 16550 to baud rate 9600.
 * PS7 UART (Zynq) is not initialized by this application, since
 * bootrom/bsp configures it to baud rate 115200
 *
 * ------------------------------------------------
 * | UART TYPE   BAUD RATE                        |
 * ------------------------------------------------
 *   uartns550   9600
 *   uartlite    Configurable only in HW design
 *   ps7_uart    115200 (configured by bootrom/bsp)
 */

/**************************************************************************************************************************
 *                                                Include Files
 **************************************************************************************************************************/

#include <stdio.h>
#include "platform.h"     // contains function init_platform() and cleanup_platform
#include "xil_types.h"    // for integer type definition
#include "xparameters.h"  // for finding device ID
#include "seg7_display.h" // display decimal digits on 7-segment display
#include "gpio_init.h"    // the device I have used from BASYS3


int main()
{
    init_platform(); // function for enable MicroBalze caches and Initialise UART

/**************************************************************************************************************************
*                                         declaration of all the Arithmetic Operations
**************************************************************************************************************************/

    s32 adder(s32 num1,s32 num2);
    s32 subtractor(s32 minuend, s32 subtrahend );
    s32 multiplicator(s32 multiplicand, s32 multiplier);
    s32 divider(s32 dividend, s32 divisor);

/**************************************************************************************************************************
 *                                    define and Initialise all the variables will be used
 **************************************************************************************************************************/
    s32 slideSwitchIn = 0;  // 16-bit binary number
    u16 finalResult = 0;    // the final result of any arithmetic operations
    u16 switchValue1;       // the most right 8-bit binary number
    u16 switchValue2;       // the most left 8-bit binary number
    u16 input = 0;          // the input represent either Value1 or Value2
    u16 BTNL_P = 0;         // initialise left button (Aim to execute addition operation)
    u16 BTNR_P = 0;         // initialise right button (Aim to execute subtraction operation)
    u16 BTNU_P = 0;         // initialise up button (Aim to execute multiplication operation)
    u16 BTND_P = 0;         // initialise down button (Aim to execute division operation)
    int leftHold = 0;       // variable for while loop to start the calculator
    int status;             // declare the status (Aim to check the initialisation)


/**************************************************************************************************************************
*                                 check the initialisation of GPIOs and interrupt System
**************************************************************************************************************************/
    status = initGpio();// initialise the GPIOs

/***Check if the general-purpose IOs in the hardware are successful initialised. If not, the program is terminated.******/
    if (status != XST_SUCCESS) {
    	print("GPIOs initialisation failed!\n\r");
    	cleanup_platform();
    	return 0;
    }

    status = setUpInterruptSystem(); // Setup the Interrupt System

/***Check if the interrupt system has been set up. If not, the program is terminated.******/
    if (status != XST_SUCCESS) {
    	print("Interrupt system setup failed!\n\r");
    	cleanup_platform();
    	return 0;
    }

/**************************************************************************************************************************
*                             display the decimal number of input value
**************************************************************************************************************************/

while(1){

    while(leftHold == 0)
    {
        BTNL_P = XGpio_DiscreteRead(&P_BTN_LEFT, 1);
        // when the left button is 1, keep left button 1 until we release the button
        if(BTNL_P == 1){
                while(BTNL_P == 1)
                {
                     BTNL_P=XGpio_DiscreteRead(&P_BTN_LEFT, 1);
                 }
                leftHold=1;//assign leftHold to 1 when releasing  the left button
         }
    }

    while(leftHold == 1){  // start the calculator
         displayNumber(input);// display either value1 or value2
         XGpio_DiscreteWrite(&LED_OUT,1,input);
    	 slideSwitchIn = XGpio_DiscreteRead(&SLIDE_SWITCHES,1);  // read the swithValue input
    	 switchValue1 = slideSwitchIn&0x00ff;  // assign 8 right switches to the Value1
    	 switchValue2 = slideSwitchIn>>8;      // assign 8 left switches to value2
    	 BTNU_P = XGpio_DiscreteRead(&P_BTN_UP,1);
    	 BTND_P = XGpio_DiscreteRead(&P_BTN_DOWN,1);
    	 BTNR_P = XGpio_DiscreteRead(&P_BTN_RIGHT,1);

/**************************************************************************************************************************
*            This part is aim to check the input switchValue
*
*            # input values can be corrected at any time before applying any arithmetic operations
*            # corrected values can be updated and demonstrated through 7-segment display and LEDs immediately
*             when pressing related buttons
*
************************************************************************************************************************/

                // when DOWN button is pressed, show switchValue1 on display
    	          if(BTND_P == 1)
    	     	{
    	     	while(BTND_P == 1)
    	     	{
    	     	   BTND_P = XGpio_DiscreteRead(&P_BTN_DOWN, 1);
    	     	}
    	     	    input = switchValue1;
    	     	}

    	     	// when UP button is pressed, show switchValue2 on display
    	         else if(BTNU_P == 1)
    	     	{

    	     	while(BTNU_P == 1)
    	     	{
    	     	   BTNU_P = XGpio_DiscreteRead(&P_BTN_UP, 1);
    	     	}
    	     	   input = switchValue2;
    	     	}



/**************************************************************************************************************************
*                           Apply the arithmetic operations and display the finalResult
**************************************************************************************************************************/
    	          if(XGpio_DiscreteRead(&P_BTN_RIGHT,1) == 1){ // going to the arithmetic mode when pressing RIGHT button

    	                	while(1){
    	                		// show the initial value before any operations applied
    	                		 displayNumber(finalResult);
    	                		 XGpio_DiscreteWrite(&LED_OUT,1,finalResult);

    	                		//after releasing the right button, and choose one operation among four operations
    	                		if(XGpio_DiscreteRead(&P_BTN_RIGHT,1) == 0){
                                  while(1){

    	                		  //doing the addition, press RIGHT button again
    	                		  if(XGpio_DiscreteRead(&P_BTN_RIGHT,1) == 1 ){
    	                			  while(1){
    	                				  finalResult = adder(switchValue1,switchValue2);
		                                  displayNumber(finalResult);
    	                				  XGpio_DiscreteWrite(&LED_OUT,1,finalResult);
    	                			  }
    	                		  }
    	                          // doing the subtraction, press UP button
    	                 		 if(XGpio_DiscreteRead(&P_BTN_UP,1) == 1){
    	                 			while(1){
    	                 				 finalResult =subtractor(switchValue1, switchValue2);
                                        	 displayNumber(finalResult);
                                        	 XGpio_DiscreteWrite(&LED_OUT,1,finalResult);

    	                 			 }

    	                 		 }
    	                 		 // doing the division, press LEFT button
    	                 		else if(XGpio_DiscreteRead(&P_BTN_LEFT,1) == 1){
    	                 		    while(1){
    	                 		          finalResult =divider(switchValue1, switchValue2);
    	                 		          displayNumber(finalResult);
    	                 		          XGpio_DiscreteWrite(&LED_OUT,1,finalResult);
    	                 		     }

    	                 		 }
    	                 		 // doing the multiplication, press DOWN button
    	                 		 else if(XGpio_DiscreteRead(&P_BTN_DOWN,1) == 1){
    	                 			while(1){
    	                 			      finalResult = multiplicator(switchValue1, switchValue2);
    	                 			      displayNumber(finalResult);
    	                 			      XGpio_DiscreteWrite(&LED_OUT,1,finalResult);
    	                 			   }

    	                 		 }
    	                	}

    	                }
    	           }

    	      }


    }
    }
    cleanup_platform();
    return 0;
}


