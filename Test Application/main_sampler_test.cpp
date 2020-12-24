/*****************************************************************//**
 * @file main_sampler_test.cpp
 *
 * @brief Basic test of nexys4 ddr mmio cores
 *
 * @author p chu
 * @version v1.0: initial release
 *********************************************************************/
// Including all the needed .h files
#define _DEBUG
#include "chu_init.h"
#include "gpio_cores.h"
#include "xadc_core.h"
#include "sseg_core.h"
#include "fifo_core.h"

//////////////////////////////////////////////////UARTS DISPLAYS ////////////////////////////////////////////////////////////////

// Checking the voltage, we took it from our previous labs, is useful.
void uart_checkvoltage(double reading) {

   static double initial_voltage = 0;  // static to keep the value
   double diffence;     // Variable to find the difference

   diffence= (initial_voltage >= reading) ? initial_voltage-reading : reading-initial_voltage; // if and else condition, to decide what difference to take

   if(diffence > .001)//if voltage changes show new voltage
   {
	   uart.disp("Current voltage, times 1000: ");  // uart.disp to show a message, along the values
	   uart.disp((int) reading);
	   uart.disp("\n\r");
	   initial_voltage = reading;
   }
}
// check what is been push to the FIFO
void uart_check_push(int write) {
	 static double ini_value = 0;  // static to keep the values

	   if(ini_value != write  || ini_value == write) // Show the byte push to the FIFO, when is a different number or the the same.
	   {
		   uart.disp("Byte push to the FIFO: ");  // uart.disp to show a message, along the values
		   uart.disp(write);
		   uart.disp("\n\r");
	   	   ini_value = write;
	   }
}
// Display the Sum in the Uart
void uart_check_sum(int sum)
{
  static double ini_value = 0;  // static to keep the values

     if(ini_value != sum  || ini_value == sum) // Show the sum of the FIFO
		 {
		    uart.disp("The FIFO sum =  ");  // uart.disp to show a message, along the values
			uart.disp(sum);
			uart.disp("\n\r");
		   	ini_value = sum;
		   }
}

//  Trying to eliminate Adc noise, we took it from our previous labs, which very useful. Basically doing something similar like in the Uartcheck
double noise_correction(double reading)
{
	static double initial_voltage = 0;   // Static to keep the value
	double diffence;   // Variable to find the difference

	diffence = (initial_voltage >= reading) ? initial_voltage-reading : reading-initial_voltage; // if and else condition, to decide what difference to take

	if(diffence > 0.002)//if the voltage is less than .002 update new value, else return the initial voltage
	{
		initial_voltage = reading;  // setting the initial voltage, what we reading
		return reading;      // return reading
	}
	else
		return initial_voltage;  //  return initial volt
}

////////////////////////////////////////////////////////SEVEN-SEGMENTS DISPLAYS ////////////////////////////////////////////////////////////////

// Display the adc values to the segment segment
void seg_write( SsegCore *sseg_p, int write) {
	// variables
	 int i;

	 // making the seven segment to only display the 0 to 255 values
       //turn off led
      for (i = 0; i < 4; i++) {
         sseg_p->write_1ptn(0xff, i);
      }

         //turn off all decimal points
         sseg_p->set_dp(0x00);

         // set 1st digit ones
      sseg_p->write_1ptn(sseg_p->h2s(write %10), 0);

     if (write >= 10){   // show only the first 2 digt when is greater or equal to 10
        // set 2nd digit tens
	  sseg_p->write_1ptn(sseg_p->h2s(write %10), 0);
      sseg_p->write_1ptn(sseg_p->h2s((write/10)%10), 1);
     }

     if (write >= 100){   // show only the first 3 digt when is greater or equal to 100
       // set 3 dgit
	  sseg_p->write_1ptn(sseg_p->h2s(write %10), 0);
	  sseg_p->write_1ptn(sseg_p->h2s((write/10)%10), 1);
      sseg_p->write_1ptn(sseg_p->h2s((write/100)%10), 2);
     }

}

//  Display the reading sum form the FIFO into 4 left digits
void seg_read(SsegCore *sseg_p, int sum ) {

	// declaring variables
	int i;
	int write =  sum;

	 //turn off led
	   for (i = 4; i < 8; i++) {
	       sseg_p->write_1ptn(0xff, i);
	      }

      //turn off all decimal points
      sseg_p->set_dp(0x00);

         // set 5st digit ones
      sseg_p->write_1ptn(sseg_p->h2s(write %10), 4);

      if (write >= 10){
        // set 6th digit tens
      sseg_p->write_1ptn(sseg_p->h2s(write %10), 4);
      sseg_p->write_1ptn(sseg_p->h2s((write/10)%10), 5);
      }

      if(write >=100){
       // set 7th digit
      sseg_p->write_1ptn(sseg_p->h2s(write %10), 4);
      sseg_p->write_1ptn(sseg_p->h2s((write/10)%10), 5);
      sseg_p->write_1ptn(sseg_p->h2s((write/100)%10), 6);
      }

      if (write>=1000){
      // set 8th digit
    	sseg_p->write_1ptn(sseg_p->h2s(write %10), 4);
    	sseg_p->write_1ptn(sseg_p->h2s((write/10)%10), 5);
    	sseg_p->write_1ptn(sseg_p->h2s((write/100)%10), 6);
        sseg_p->write_1ptn(sseg_p->h2s(write/1000), 7);
      }

}

//////////////////////////////////////////////////////////EXTRA ADDDED, NONE-REQUIRED FUNCTONS//////////////////////////////////////////////////

// Trying to make max value coming form the adc to be 0-255, no more than that
int min_max_value(int write)
{
	if(write < 256)//if write value is less then 256 return the value
	{
		return write;      // return Adc vlaue
	}
	else
	{
		return write = 255;  //  If Adc is beyond 255 which is the max, make the write value = 255
	}
}

// When the button for rest is press do this to make the FIFO empty
void reset (fifo_core *user_p, GpoCore *led_p,  DebounceCore *btn_p)
{
	// declaring variables
	int reset;

	reset = btn_p-> read_db(4);  // read the debounce up button for reset

	if (reset){  // if reset is press, empty all the FIFO and resting all the flags full to o and empty to 1

		 sleep_ms(200);            // sleep to prevent multiple inputs
	   for (int i = 16; i > 0; i--) {  // indicate is a reset by doing a simple chase led from left to right
			 led_p->write(1,i);
			 sleep_ms(50);
			 led_p->write(0,i);
			 sleep_ms(50);
		 	 }
	   for(int i = 0; i < 4; i++){  // reading all the bytes in the FIFO to empty, which is reseting without taking the sum
	       user_p->read_byte();
	   }
	 }

}

// This displays on the 4th digit E= empty, F=full and A = None
void F_E_digit4 (fifo_core *user_p, SsegCore *sseg_p, int empty, int full)
{
   if (empty) {
	   sseg_p->write_1ptn(sseg_p->h2s(14), 3);   // make the 4 digit on the right display the letter E
   }
   if(full){
	   sseg_p->write_1ptn(sseg_p->h2s(15), 3);   // make the 4 digit on the right display the letter F
   }
   if(!full && !empty)
   {
	   sseg_p->write_1ptn(sseg_p->h2s(10), 3);   // make the 4 digit on the right display the letter A meaning is NOT empty or full
   }
}
// uses the Tri-color lights to show When is full or empty
void lights_full_empty(PwmCore *pwm_p, int color)
{
	// declaring variables, I used the same values form the given labs
    int i;
	double bright, duty;
	const double P20 = 1.2589; // P20=100^(1/20); i.e., P20^20=100

	    // blue lights to let the user know the FIFO full and can't push
	    // or is empty and we can't add because the FIFO is empty
	bright = 1.0;
	 for (i = 0; i < 15; i++){
			bright = bright * P20;
			duty = bright / 100.0;

		   if (color == 0) {
			 pwm_p->set_duty(duty, 0 );   // Blue lights on both tri-color lights, when pushing when is full
			 pwm_p->set_duty(duty, 3);
			 sleep_ms(50);
			}
		   else{
			 pwm_p->set_duty(duty, 2);   // Red lights on both tri-color lights, when adding when is empty
			 pwm_p->set_duty(duty, 5);
			 sleep_ms(50);
			 }
		}

		sleep_ms(50); // turn them off after is done
		for (i = 0; i < 6; i++) {
			 pwm_p->set_duty(0.0, i);
		}

}
// flash LEDs when pushing a item when is full and when adding and its empty
void flash_leds (GpoCore *led_p)
{
	 for (int i = 0; i < 3; i++)
	         {
			   led_p->write(0xffff);
			   sleep_ms(200);
			   led_p->write(0x0000);
			   sleep_ms(200);
			 }
}

// Isolating the switches 0-7
int unpack_swtiches(GpiCore *sw_p) {
   int switches;

   switches = sw_p->read();//read switch inputs

   return switches = (uint8_t) (switches & 0xff); //isolate switches 0-7
}

// If the user wants he can also use the [7:0] switches to write to the FIFO, so the user can even push a byte with the Adc, then one from the switches etc.
void write_FIFO_Switches(fifo_core *user_p, GpiCore *sw_p, DebounceCore *btn_p,  PwmCore *pwm_p, GpoCore *led_p,  int full)
{
	// declaring the variables
	   int sw_button;
	   int blue = 0;

	   sw_button = btn_p->read_db(1);   // reading the button to push a byte to the FIFO

	   if (sw_button)     // button to write input to the FIFO, using the switches
	   {
		   sleep_ms(200);    // sleep to prevent multiple inputs
			   user_p->write_byte(unpack_swtiches(sw_p));  // take the input form the switches and write it to the FIFO
			   uart_check_push(unpack_swtiches(sw_p));     // Uart_check to see what is been push to it

	       if (full )   // If full then flash the user, to indicate it the FIFO is full
		  	{
	    	   lights_full_empty(pwm_p, blue);   // also light up the tri-colr to indicate is full

	    	   flash_leds (led_p);  // flash the leds when pushing but is full
	        }
	   }
}

//////////////////////////////////////////////////MAIN READING AND WRITING TO THE FIFO/////////////////////////////////////////////////////
// Find the sum of the FIFO bytes
int find_sum (fifo_core *user_p, GpoCore *led_p, DebounceCore *btn_p, PwmCore *pwm_p, SsegCore *sseg_p, int empty)
{
	// Declaring variables
	int i, find_sum;
	static int sum = 0;     // static to keep the sum
	int red = 1;

	find_sum=  btn_p->read_db(2);   // read button on position 2, (down button)

	// Find the sum of the FIFO bytes
	 if (find_sum)     // debouncing button to write adc input to the FIFO
	 {
		 sleep_ms(200);    // sleep to prevent multiple inputs
		  sum = 0;
		 for (i = 0; i < 4; i++)    // read four times to get the sum of the FIFO
		 {
		   sum += user_p->read_byte();
		 }
		 uart_check_sum(sum); // show the sum in the Uart when the sum button is press

		 if (empty )   // If full then flash the user, to indicate it the FIFO is full
		{
		  lights_full_empty(pwm_p, red);   // also light up the tri-colr to indicate is full

		  flash_leds (led_p);  // flash the leds when trying to add but the FIFO is empty
		}
	 }
	 return sum;
}

// Write to the FIFO when pressing the button
void write_fifo (fifo_core *user_p, GpoCore *led_p, DebounceCore *btn_p, PwmCore *pwm_p,  int write, int full)
{
	// declaring the variables
   int push_button;
   int blue = 0;

   push_button = btn_p->read_db(0);   // reading the button to push a byte to the FIFO

   if (push_button)     // debouncing button to write adc input to the FIFO
   {
	   sleep_ms(200);    // sleep to prevent multiple inputs

		   user_p->write_byte(write);  // max is the Adc (int) value
		   uart_check_push(write);     // Uart_check to see what is been push to it

       if (full )   // If full then flash the user, to indicate it the FIFO is full
	  	{
    	   lights_full_empty(pwm_p, blue);   // also light up the tri-colr to indicate is full

    	   flash_leds (led_p);  // flash the leds when pushing but is full
      }
   }
}

/////////////////////////////////////////////////////MAIN FUCNTION///////////////////////////////////////////////////////////////////////////////////

// Main FIFO push and read from the fifo
void fifo_main(fifo_core *user_p, GpoCore *led_p, DebounceCore *btn_p, XadcCore *adc_p, SsegCore *sseg_p, PwmCore *pwm_p, GpiCore *sw_p)
{
	// declaring variables
	int empty, full;         // flags
	double reading, integer; // read Adc
	int max, write;
	static int sum;          // find the sum

	// Check the flags and indicated the status using a led
  empty = user_p->fifo_empty();
  full = user_p->fifo_full();

  // show the led empty and full flags
  led_p->write(empty, 0);
  led_p->write(full, 1);

   // It shows the Letter E or F when is Full or Empty on the 4th digit on the right, and letter A when is NOT full or empty
   F_E_digit4 (user_p, sseg_p, empty, full);

	//  read adc input, reading the 8 bits
   reading = noise_correction(adc_p->read_adc_in(0));

	// To get good values we could multiply the adc time 10000 so if 0.006 = 6 and then just take the int of it
   integer = reading * 1000.0;
   write = (int)integer;

    // Function to prevent taking byte numbers form than 255
	max = min_max_value(write); //  get the Adc value

	// display the voltage, which is the input to the FIFO
	uart_checkvoltage(max);

	// put the (int) value in the write segment display 0-255
	 seg_write(sseg_p, max);

	// Show the read sum to the left seven-segments
	 seg_read(sseg_p, sum);

    // write to the FIFO when pressing the button
	 write_fifo (user_p, led_p, btn_p, pwm_p, max, full);

	 // If the User wants he can also use the switches to write into the FIFO, this not require, but I just wanted to try it with the switches
	 // to push to the FIFO using the swtches use the left button on position 1
	 write_FIFO_Switches(user_p, sw_p, btn_p, pwm_p, led_p, full);

   // Getting the sum form the int function
	 sum = find_sum (user_p, led_p, btn_p, pwm_p, sseg_p, empty);

   // Do a reset FIFO from Vitis, without using the CPU reset, the reset button is the middle one
	   reset (user_p,led_p,  btn_p);

}

GpoCore led(get_slot_addr(BRIDGE_BASE, S2_LED));
GpiCore sw(get_slot_addr(BRIDGE_BASE, S3_SW));
XadcCore adc(get_slot_addr(BRIDGE_BASE, S5_XDAC));
DebounceCore btn(get_slot_addr(BRIDGE_BASE, S7_BTN));
SsegCore sseg(get_slot_addr(BRIDGE_BASE, S8_SSEG));
fifo_core user(get_slot_addr(BRIDGE_BASE, S4_USER));
PwmCore pwm(get_slot_addr(BRIDGE_BASE, S6_PWM));

int main() {

   while (1) {
	  fifo_main(&user, &led, &btn, &adc, &sseg, &pwm, &sw);   // main FIFO function, I made use of all we have learned so far

   } //while
} //main

