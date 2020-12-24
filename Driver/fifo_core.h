//fifo_core.h


#ifndef _FIFO_CORE_H_INCLUDED
#define _FIFO_CORE_H_INCLUDED

#include "chu_init.h"
#include "chu_io_rw.h"
#include "chu_io_map.h"


class fifo_core {
public:
    /**
     * register map
     *
     */

    enum {

    	  STATUS_DATA_REG = 0, // data/status register
		  WR_DATA_REG = 1,     // wr data register
	      RD_DATA_REG = 2      // rd read data
    };

    /**
      * mask fields
      *
      */
      enum {
         FULL_FIELD = 0x00000200,     // bit 10 of the rd_data_reg on viviado, mask for the full bit
         EMPTY_FIELD = 0x00000100,    //  bit 9 of the rd_data_reg  on viviado, mask for the  empty bit
         DATA_FIELD = 0x000000ff      //  bits [7:0] of the rd_data_reg  on viviado, so then read data
      };


    fifo_core(uint32_t core_base_addr);

    ~fifo_core();                  // not used

     /**
     * check the empty flag in the fifo
     * return 1,  if empty or 0  if not empty
     */

    int fifo_empty();

    /**
     * check the Full flag in the fifo
     * return 1, if  Full or 0 if not Full
     */

    int fifo_full();

     /**
     * //  A function to write a byte into the buffer
     *
     */

    void write_byte(uint8_t byte);

    /**
     *  A function to read a byte from the buffer
     *
     */

    int read_byte();

private:      // Access specifier
    uint32_t base_addr;    // Attributes
    uint32_t wr_data;
};

#endif
