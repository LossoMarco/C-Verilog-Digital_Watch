
module my_timer(
clk         , // Clock Input
address     , // Address Input
readdata        , // Data output
writedata        , // Data input
chipselect          , // Chip Select
write          , // Write Enable/Read Enable
reset_n				// Reset
);

parameter DATA_WIDTH = 32 ;
parameter ADDR_WIDTH = 2 ;
parameter SUBSAMP = 50000 ;


//--------------Input/Output Ports----------------------- 
input                  clk         ;
input [ADDR_WIDTH-1:0] address     ;
input                  chipselect          ;
input                  write          ;
input                  reset_n          ; 
input [DATA_WIDTH-1:0]  writedata       ;
output reg [DATA_WIDTH-1:0]  readdata       ;


reg [DATA_WIDTH-1:0] count;

always @ (posedge clk or negedge reset_n)
	begin
		if (reset_n==0)
			count <= 0;
		else if (count<SUBSAMP)
			count <= count + 1;
		else 
		   count <=0;
	end


// Memory Write Block 

always @ (posedge clk or negedge reset_n)
if (reset_n==0 ) readdata <= 0;
else  if (write) readdata <= writedata;
else if(count==1)
		readdata <= readdata + 1;
		
endmodule