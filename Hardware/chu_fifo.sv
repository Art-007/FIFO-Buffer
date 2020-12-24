`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 10/29/2020 03:43:55 PM
// Design Name: 
// Module Name: chu_fifo
// Project Name: 
// Target Devices: 
// Tool Versions: 
// Description: 
// 
// Dependencies: 
// 
// Revision:
// Revision 0.01 - File Created
// Additional Comments:
// 
//////////////////////////////////////////////////////////////////////////////////

// 
//  Reg map (each port uses 4 address space)
//    * 0: read data and status
//    * 1: write data 
//    * 2: read enable 

module chu_fifo
#( parameter DATA_WIDTH=8, // parameter to defin the number 8 bits = byte 
             ADDR_WIDTH=2  // number of 4 rows of 1 byte each, 2^2 =4-1 = [0-3] and number of address in the fifo ctrl   
   )
   (
    input  logic clk,
    input  logic reset,
    // slot interface
    input  logic cs,
    input  logic read,
    input  logic write,
    input  logic [4:0] addr,
    input  logic [31:0] wr_data,
    output logic [31:0] rd_data    
   );

   // signal declaration
   logic wr, rd;
   logic full, empty;
   logic [7:0] r_data;   

   // body
   // instantiate uart fifo connecting all the signals require 
   fifo #(.DATA_WIDTH(DATA_WIDTH), .ADDR_WIDTH( ADDR_WIDTH))  FIFO_unit     
   (.*, .rd(rd), .wr(wr), .w_data(wr_data[7:0]),.empty(empty),.full(full),.r_data(r_data[7:0])); // we write and read a byte at a time 
       
       
   /////// wrapping circuit////////////       
   // decoding logic                                 // 0 is for the status of the fifo
   assign wr = (write && cs && (addr[1:0]==2'b01));  // address 1 enable write 
   assign rd= (read && cs && (addr[1:0]==2'b10));   //  address 2 read enable 
   
   // slot read concatenation to read 
   assign rd_data = {22'h000000, full,  empty, r_data}; // 10 bit the full flag   we'll use masking to unpack it in vitis 
                                                        // 9 bit the empty flag
                                                        // [7:0] the read data  
endmodule

