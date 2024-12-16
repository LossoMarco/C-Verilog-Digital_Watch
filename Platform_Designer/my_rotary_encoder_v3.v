
module my_rotary_encoder(
clk         , // Clock Input
address     , // Address Input
readdata        , // Data output
writedata        , // Data input
chipselect          , // Chip Select
write          , // Write Enable/Read Enable
reset_n		,		// Reset
data1,
data2,
data1_reg,
data2_reg
);

parameter DATA_WIDTH = 32 ;
parameter ADDR_WIDTH = 4 ;
parameter SUBSAMP = 12 ;


//--------------Input/Output Ports----------------------- 
input                  clk         ;
input [ADDR_WIDTH-1:0] address     ;
input                  chipselect          ;
input                  write          ;
input                  reset_n          ; 
input [DATA_WIDTH-1:0]  writedata       ;
input							data1;
input							data2;
output reg data1_reg       ;
output reg data2_reg       ;
output reg [DATA_WIDTH-1:0]  readdata       ;


reg data1_delayed, data2_delayed;
reg [SUBSAMP-1:0] count;


always @(posedge clk) count=count+1;
always @(posedge clk) if (count==0) data1_reg <= data1; 
always @(posedge clk) if (count==0) data2_reg <= data2; 
always @(posedge clk)  data1_delayed<=data1_reg; 
always @(posedge clk)  data2_delayed<=data2_reg; 

wire readdata_enable = data1_delayed ^ data1_reg ^ data2_delayed ^ data2_reg;
wire readdata_direction = data1_reg ^ data2_delayed;

// Memory Write Block 

always @ (posedge clk or negedge reset_n)
if (reset_n==0 ) readdata <= 0;
else  if (write) readdata <= writedata;
else if(readdata_enable)
		readdata <= readdata + (readdata_direction ? 1 : -1);
		
		
		

endmodule