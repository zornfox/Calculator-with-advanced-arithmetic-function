



/**************************************************************************************************************************
 ******** Name: Keyi Liu
 ******** Student ID: s1703084
 ******** Lab Group: Wednesdays
 **************************************************************************************************************************/



/**************************************************************************************************************************
 * Advanced Calculator
 **************************************************************************************************************************
 * functionalities achieved under this project :
 *
 *
 * (extra features)
 * 1. every input Value can be demonstrated through 7-segment display and LEDs
 * 2. input values can be corrected at any time before applying any arithmetic operations
 * 3. corrected values can be updated and demonstrated through 7-segment display and LEDs immediately
 * when pressing related buttons
 * 4. applying complex arithmetic operations which are modulator, squareRoot, power, logarithm.
 * 5. round off will be applied to the decimal result
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
 * 3.press BTNR(right button)>>>>>go the the complex arithmetic mood
 *   #press BTNR again>>>>>doing the modulator
 *   #press BTNL >>>>>>>>>>doing the power
 *   #press BTNU >>>>>>>>>>doing the squareRoot
 *   #press BTND >>>>>>>>>>doing the logarithm
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
#include "math.h"         // for doing complex arithmetic operations


int main()
{
    init_platform(); // function for enable MicroBalze caches and Initialise UART

/**************************************************************************************************************************
*                                         declaration of all the Arithmetic Operations
**************************************************************************************************************************/

    // all the arithmetic operations' definitions are in arith_operations.c
    u32 modulator(u32 dividend, u32 divisor);
    float squareRoot(float one_8_number);
    s32 power(s32 base, s32 exponent );
    float logarithm(float exponent, float base);

/**************************************************************************************************************************
 *                                    define and Initialise all the variables will be used
 **************************************************************************************************************************/
    s32 slideSwitchIn = 0;  // 16-bit binary number
    float finalResult = 0;    // the final result of any arithmetic operations
    float switchValue1;       // the most right 8-bit binary number
    float switchValue2;       // the most left 8-bit binary number
    float input = 0;          // the input aim to show the either Value1 or Value2
    u16 BTNL_P = 0;         // initialise left button (Aim to execute addition operation)
    u16 BTNR_P = 0;         // initialise right button (Aim to execute subtraction operation)
    u16 BTNU_P = 0;         // initialise up button (Aim to execute multiplication operation)
    u16 BTND_P = 0;         // initialise down button (Aim to execute division operation)
    int leftHold = 0;       // variable for while loop to display inputs on 7-segment display
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

while(1){  // start the calculator

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

    while(leftHold == 1){  // keep the calculator working
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

    	                		  //doing the modulation, press RIGHT button again
    	                		  if(XGpio_DiscreteRead(&P_BTN_RIGHT,1) == 1 ){
    	                			  while(1){
    	                				  finalResult = modulator(switchValue1,switchValue2);
		                                  displayNumber(finalResult);
    	                				  XGpio_DiscreteWrite(&LED_OUT,1,finalResult);
    	                			  }
    	                		  }
    	                          // doing the squareRoot, press UP button
    	                		  // get the square root of an 8-bit value
    	                 		 if(XGpio_DiscreteRead(&P_BTN_UP,1) == 1){
    	                 			while(1){
    	                 				 finalResult =squareRoot(switchValue1);;
    	                 				if(finalResult-(int)finalResult > 0.5){
    	                 				   displayNumber(finalResult+1);
    	                 				   XGpio_DiscreteWrite(&LED_OUT,1,finalResult+1);
    	                 				   }else{
    	                 				   displayNumber(finalResult);
    	                 				   XGpio_DiscreteWrite(&LED_OUT,1,finalResult);
    	                 				   }
    	                 			 }

    	                 		 }
    	                 		 // doing the power, press LEFT button
    	                 		 // switchValue1 as base and switchValue2 as exponent(index)
    	                 		else if(XGpio_DiscreteRead(&P_BTN_LEFT,1) == 1){
    	                 		    while(1){
    	                 		          finalResult =power(switchValue1, switchValue2);
    	                 		          displayNumber(finalResult);
    	                 		          XGpio_DiscreteWrite(&LED_OUT,1,finalResult);
    	                 		     }

    	                 		 }
    	                 		 // doing the logarithm, press DOWN button
    	                 		// switchValue1 as exponent and switchValue2 as base
    	                 		 else if(XGpio_DiscreteRead(&P_BTN_DOWN,1) == 1){
    	                 			while(1){
    	                 			      finalResult = logarithm(switchValue1, switchValue2);
    	                 			      if(finalResult-(int)finalResult > 0.5){
    	                 			      displayNumber(finalResult+1);
    	                 			      XGpio_DiscreteWrite(&LED_OUT,1,finalResult+1);
    	                 			      }else{
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
    }
    cleanup_platform();
    return 0;
}


