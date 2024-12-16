
module my_led7seg (clk, address, readdata, writedata, read, write, reset_n, hexa,hexb,hexc,hexd,hexe,hexf);


input clk;
input address;
output [31:0] readdata;
input [31:0] writedata;
input read,write;
input reset_n;
output [6:0] hexa,hexb,hexc,hexd,hexe,hexf;


reg [31:0] reg_data;

assign readdata = (read) ? reg_data : {31{1'bz}};

always @(posedge clk)
  if (write)
    reg_data = writedata;
	 
	 
	 
SEG7_LUT	u0	(clk,hexa,reg_data[3:0]);
SEG7_LUT	u1	(clk,hexb,reg_data[7:4]);
SEG7_LUT	u2	(clk,hexc,reg_data[11:8]);
SEG7_LUT	u3	(clk,hexd,reg_data[15:12]);
SEG7_LUT	u4	(clk,hexe,reg_data[19:16]);
SEG7_LUT	u5	(clk,hexf,reg_data[23:20]);


endmodule

module SEG7_LUT	(clk,oSEG,iDIG);

input clk;
input	[3:0]	iDIG;
output	reg [6:0]	oSEG;

always @(posedge clk)
begin
	case(iDIG)
		4'h1: oSEG = 7'b1111001;	// ---t----
		4'h2: oSEG = 7'b0100100; 	// |	  |
		4'h3: oSEG = 7'b0110000; 	// lt	 rt
		4'h4: oSEG = 7'b0011001; 	// |	  |
		4'h5: oSEG = 7'b0010010; 	// ---m----
		4'h6: oSEG = 7'b0000010; 	// |	  |
		4'h7: oSEG = 7'b1111000; 	// lb	 rb
		4'h8: oSEG = 7'b0000000; 	// |	  |
		4'h9: oSEG = 7'b0011000; 	// ---b----
		4'ha: oSEG = 7'b0001000;
		4'hb: oSEG = 7'b0000011;
		4'hc: oSEG = 7'b1000110;
		4'hd: oSEG = 7'b0100001;
		4'he: oSEG = 7'b0000110;
		4'hf: oSEG = 7'b0001110;
		4'h0: oSEG = 7'b1000000;
		endcase
end
endmodule

