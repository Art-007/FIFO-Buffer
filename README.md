FIFO Buffer
## This project involved the use of a FIFO buffer, similar to the Uart FIFO. The FIFO can store up to 4 bytes. The input volatge from the Xadc (potentiometer) determines what bytes will be push, after a button is press. To read from the FIFO, we find the sum of all the stored bytes. Finally, we display the empty and full flags using two leds.  
### *This project includes:*

1. - [x] The Hardware, with all the modify code from vivado to permform this lab. 
    * The chu_fifo, has the instantiation of the Uart FIFO and our wrapping circuit. 
    * mmio_sys_sampler, has the connection of our parametrized FIFO to the mmio system, we kept the same name S4_User for simplicity 

2. - [x] The driver.
    * The fifo_core.h, has the needed fucntion such as, a empty, full, write and read fucntions. 
    * The fifo_core.cpp, has how we implementing and driving  the fucntions.

3. - [x] Test Application.
    * The main_sampler_test.cpp, has all the C++ code to implement the write, read and the FIFO flags of empty and full
      * It has the require testing such as to push to the FIFO using the potentiometer, read the sum, and display the empty and full. and do flashing when trying to push, but full and when trying to read, but empty.  
      * It has also non-require fucntions such as using the tri color lights, a rest button just reading without having the sum, and pushing bytes using the switches [7:0], or pushing bytes in a combination of switches and the potentiometer.

4. - [x] Vivado Plots.
    * Resgister Map
    * FIFO simulation testing just to see if it was writing  and reading.

5. - [x] Video implemetation of project on the FPGA board. The first 5 minutes it covers the needed material, and the rest of it, the other non-require functionalities that I added just to see and test what we learned so far. **[Click here for project video demo ](https://www.youtube.com/watch?v=ElIRNtSaTC4)**
