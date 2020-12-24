// fifo_core.cpp

#include "fifo_core.h"  // Including our created .h file

fifo_core::fifo_core(uint32_t core_base_addr) {       // core address
    base_addr = core_base_addr;
}

fifo_core::~fifo_core() {
}

// Find if the FIFO is empty or not
int fifo_core::fifo_empty() {
	uint32_t rd_word;  // we read the 32 bit
	int empty;

	rd_word = io_read(base_addr, STATUS_DATA_REG);  // read the 32 bit, which we concatenated on vivado
	empty = (int) (rd_word & EMPTY_FIELD) >> 8;     // bit manipulation, unpacking the empty because its on the 9 bit, just after [7:0] read data
                                                    // then we shift by 8

	return (empty); // return 1 or 0, meaning is empty or not
}

// Find if the FIFO is full or not
int fifo_core::fifo_full() {
	uint32_t rd_word; // we read a 32 bit
	int full;

	rd_word = io_read(base_addr, STATUS_DATA_REG);  // read the 32 bit, which we concatenated on vivado
	full = (int) (rd_word & FULL_FIELD) >> 9;      //bit manipulation, unpacking the full, because it on the 10 bit, just after the empty bit
                                                   // then we shift by 9

	return (full);   // return 1 or 0,  meaning is full or not
}

// writing a byte to the FIFO
void fifo_core::write_byte(uint8_t byte) { // it takes a byte

	if (!fifo_full())   // If the FIFO is not full, then write.
	{
	  wr_data = (uint32_t)byte;
	   io_write(base_addr, WR_DATA_REG, wr_data );  // doing a io write, based on our map, where WR_DATA_REG = 1
	}
}

// reading form the FIFO
int fifo_core::read_byte() {
	uint32_t read_data;   // read the 32 bit

	if (!fifo_empty())    // If the FIFO is not empty, then read, else return 0.
	{
	      read_data = io_read(base_addr, RD_DATA_REG) &  DATA_FIELD;   // doing a io read, based on our map, where  RD_DATA_REG= 2
	                                                                   // then we mask the 8 bits or the byte

	      return ((int)  read_data);    // we return the (int) of the read data
	   }
	else
	{
		return 0;  // if not empty return 0.
	}
}
