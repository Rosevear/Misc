/NOTE: This code has been modified from its original form
//Original code by John Loomis
//Taken from: http://www.johnloomis.org/digitallab/audio/audio3/audio3.html 
//and :http://www.johnloomis.org/digitallab/ps2lab1/ps2lab1.html#top
//and :http://www.johnloomis.org/digitallab/lcdlab/lcdlab3/lcdlab3.html
//and :

module finalProject(
  // Clock Input (50 MHz)
  input CLOCK_50, // 50 MHz
  input CLOCK_27, // 27 MHz
  //  Push Buttons
  input  [3:0]  KEY,
  //  DPDT Switches 
  input  [17:0]  SW,
  //  7-SEG Displays
  output  [6:0]  HEX0, HEX1, HEX2, HEX3, HEX4, HEX5, HEX6, HEX7,
  //  LEDs
  output  [8:0]  LEDG,  //  LED Green[8:0]
  output  [17:0]  LEDR, //  LED Red[17:0]
  // TV Decoder
  output TD_RESET, // TV Decoder Reset
  // I2C
  inout  I2C_SDAT, // I2C Data
  output I2C_SCLK, // I2C Clock
  // Audio CODEC
  output/*inout*/ AUD_ADCLRCK, // Audio CODEC ADC LR Clock
  input	 AUD_ADCDAT,  // Audio CODEC ADC Data
  output /*inout*/  AUD_DACLRCK, // Audio CODEC DAC LR Clock
  output AUD_DACDAT,  // Audio CODEC DAC Data
  inout	 AUD_BCLK,    // Audio CODEC Bit-Stream Clock
  output AUD_XCK,     // Audio CODEC Chip Clock
  //  GPIO Connections
  inout  [35:0]  GPIO_0, GPIO_1,
  // Keyboard Inputs
  input	PS2_DAT,
  input	PS2_CLK,
  //	LCD Module 16X2
  output LCD_ON,	// LCD Power ON/OFF
  output LCD_BLON,	// LCD Back Light ON/OFF
  output LCD_RW,	// LCD Read/Write Select, 0 = Write, 1 = Read
  output LCD_EN,	// LCD Enable
  output LCD_RS,	// LCD Command/Data Select, 0 = Command, 1 = Data
  inout [7:0] LCD_DATA	// LCD Data bus 8 bits
  
  //connect switches to the green LED's
  //assign 

);

wire [6:0] myclock;
wire RST;
assign RST = KEY[0];

// reset delay gives some time for peripherals to initialize
wire DLY_RST;
Reset_Delay r0(	.iCLK(CLOCK_50),.oRESET(DLY_RST) );
assign	LCD_ON		=	1'b1;
assign	LCD_BLON	=	1'b1;



//	All inout port turn to tri-state
assign	GPIO_0		=	36'hzzzzzzzzz;
assign	GPIO_1		=	36'hzzzzzzzzz;


//assign RST = KEY[0];

// reset delay gives some time for peripherals to initialize


// Send switches to red leds 
assign LEDR = SW;

// Turn off green leds
assign LEDG = 0;

// Turn off 7-segment displays
parameter BLANK = 7'h7f;


assign	TD_RESET = 1'b1;  // Enable 27 MHz

VGA_Audio_PLL 	p1 (	
	.areset(~DLY_RST),
	.inclk0(CLOCK_27),
	.c0(VGA_CTRL_CLK),
	.c1(AUD_CTRL_CLK),
	.c2(VGA_CLK)
);

I2C_AV_Config u3(	
//	Host Side
  .iCLK(CLOCK_50),
  .iRST_N(KEY[0]),
//	I2C Side
  .I2C_SCLK(I2C_SCLK),
  .I2C_SDAT(I2C_SDAT)	
);

assign	AUD_ADCLRCK	=	AUD_DACLRCK;
assign	AUD_XCK		=	AUD_CTRL_CLK;

audio_clock u4(	
//	Audio Side
   .oAUD_BCK(AUD_BCLK),
   .oAUD_LRCK(AUD_DACLRCK),
//	Control Signals
  .iCLK_18_4(AUD_CTRL_CLK),
   .iRST_N(DLY_RST)	
);

audio_converter u5(
	// Audio side
	.AUD_BCK(AUD_BCLK),       // Audio bit clock
	.AUD_LRCK(AUD_DACLRCK), // left-right clock
	.AUD_ADCDAT(AUD_ADCDAT),
	.AUD_DATA(AUD_DACDAT),
	// Controller side
	.iRST_N(DLY_RST),  // reset
	.AUD_outL(audio_outL),
	.AUD_outR(audio_outR),
	.AUD_inL(audio_inL),
	.AUD_inR(audio_inR)
);

wire [15:0] audio_inL, audio_inR;
wire [15:0] audio_outL, audio_outR;
wire [15:0] signal;

//Retrieve the scan code of the key being pressed
wire reset = 1'b0;
wire [7:0] scan_code;

reg [7:0] history[1:4];

//reg [7:0] history[1:4];
wire read, scan_ready;

oneshot pulser(
   .pulse_out(read), //output
   .trigger_in(scan_ready), //input
   .clk(CLOCK_50) //input
);

keyboard kbd(
  .keyboard_clk(PS2_CLK), //input
  .keyboard_data(PS2_DAT), //input
  .clock50(CLOCK_50), //input
  .reset(reset), //input
  .read(read), //input
  .scan_ready(scan_ready), //output
  .scan_code(scan_code)    //output
);

/*
hex_7seg dsp0(history[1],HEX1, HEX0);
hex_7seg dsp1(history[2],HEX2, HEX3);
hex_7seg dsp6(history[3],HEX4, HEX5);
hex_7seg dsp7(history[4],HEX6, HEX7);

*/

always @(posedge scan_ready)
begin
    history[4] <= history[3];
    history[3] <= history[2];
    history[2] <= history[1];
    history[1] <= scan_code;
end

//GLOBAL STUFF
//Registers storing the frequency to set to audio-out
wire [31:0]freq_m; 
wire [31:0]freq_h;

//Convert scan_code into a ordered numerical code
wire [5:0]freq_code;
freq_encoder(scan_code, freq_code);

//Convert scan code into harmony interval code
wire [5:0]harmony_interval;

interval_encoder(SW[13:1], harmony_interval);

//Harmonize the original frequency
wire [5:0]freq_code_h;
harmonizer(freq_code, harmony_interval, freq_code_h); 

//Harmonizer on/off
wire [5:0]freq_code_h_final;
harmonizer_on_off(SW[0], freq_code_h, freq_code_h_final);

//convert freq_code to char_code for LCD
wire [3:0] hex0, hex1;
char_encoder(freq_code, hex0, hex1);
LCD_Display u1(
// Host Side
   .iCLK_50MHZ(CLOCK_50),
   .iRST_N(DLY_RST),
   .hex0(hex0),
   .hex1(hex1),
// LCD Side
   .DATA_BUS(LCD_DATA),
   .LCD_RW(LCD_RW),
   .LCD_E(LCD_EN),
   .LCD_RS(LCD_RS)
);

// blank unused 7-segment digits
assign HEX0 = 7'b111_1111;
assign HEX1 = 7'b111_1111;
assign HEX2 = 7'b111_1111;
assign HEX3 = 7'b111_1111;
assign HEX4 = 7'b111_1111;
assign HEX5 = 7'b111_1111;
assign HEX6 = 7'b111_1111;
assign HEX7 = 7'b111_1111;


//Convert the manually derived freq code into a frequency
code_to_freq(freq_code, freq_m);

//Convert the harmonized freq code into a frequency
code_to_freq(freq_code_h_final, freq_h);
//Pass the final manually derived frequencies to the audio converter
//set up DDS frequency
//Use switches to set freq

wire [31:0] dds_incr;
wire [31:0] freq_r = freq_m[3:0]+10*freq_m[7:4]+100*freq_m[11:8]+1000*freq_m[15:12]+10000*freq_m[17:16];
assign dds_incr = freq_r * 91626 ; //91626 = 2^32/46875 so SW is in Hz

reg [31:0] dds_phase;

always @(negedge AUD_DACLRCK or negedge DLY_RST)
	if (!DLY_RST) dds_phase <= 0;
	else dds_phase <= dds_phase + dds_incr;

wire [7:0] index = dds_phase[31:24];

 
sine_table sig1(
	.index(index),
	.signal(audio_outR)
);

//audio_outR <= audio_inR;

//always @(posedge AUD_DACLRCK)

//Pass the final harmonized frequencies to the audio converter 

wire [31:0] dds_incr_h;
wire [31:0] freq_l = freq_h[3:0]+10*freq_h[7:4]+100*freq_h[11:8]+1000*freq_h[15:12]+10000*freq_h[17:16];
assign dds_incr_h = freq_l * 91626;

reg [31:0] dds_phase_h;

always @(negedge AUD_DACLRCK or negedge DLY_RST)
	if (!DLY_RST) dds_phase_h <= 0;
	else dds_phase_h <= dds_phase_h + dds_incr_h;

wire [7:0] index_h = dds_phase_h[31:24];

sine_table sig2(
	.index(index_h),
	.signal(audio_outL)
);

//assign audio_outL = 15'h0000;
endmodule

module freq_encoder(scan_code, freq_code);
//Port declarations
input [7:0]scan_code;
output reg [5:0]freq_code; //determine bit width for this if it matters

//Encoding process
always @ (scan_code)
case (scan_code)
//Row 1
8'h0e: freq_code =6'd0;
8'h16: freq_code =6'd1;
8'h1e: freq_code =6'd2;
8'h26: freq_code =6'd3;
8'h25: freq_code =6'd4;
8'h2e: freq_code =6'd5;
8'h36: freq_code =6'd6;
8'h3d: freq_code =6'd7;
8'h3e: freq_code =6'd8;
8'h46: freq_code =6'd9;
8'h45: freq_code =6'd10;
8'h4e: freq_code =6'd11;
//Row 2
8'h0d: freq_code =6'd12;
8'h15: freq_code =6'd13;
8'h1d: freq_code =6'd14;
8'h24: freq_code =6'd15;
8'h2d: freq_code =6'd16;
8'h2c: freq_code =6'd17;
8'h35: freq_code =6'd18;
8'h3c: freq_code =6'd19;
8'h43: freq_code =6'd20;
8'h44: freq_code =6'd21;
8'h4d: freq_code =6'd22;
8'h54: freq_code =6'd23;
//Row 3
8'h58: freq_code =6'd24;
8'h1c: freq_code =6'd25;
8'h1b: freq_code =6'd26;
8'h23: freq_code =6'd27;
8'h2b: freq_code =6'd28;
8'h34: freq_code =6'd29;
8'h33: freq_code =6'd30;
8'h3b: freq_code =6'd31;
8'h42: freq_code =6'd32;
8'h4b: freq_code =6'd33;
8'h4c: freq_code =6'd34;
8'h52: freq_code =6'd35;
//Row 4
8'h12: freq_code =6'd36;
8'h1a: freq_code =6'd37;
8'h22: freq_code =6'd38;
8'h21: freq_code =6'd39;
8'h2a: freq_code =6'd40;
8'h32: freq_code =6'd41;
8'h31: freq_code =6'd42;
8'h3a: freq_code =6'd43;
8'h41: freq_code =6'd44;
8'h49: freq_code =6'd45;
8'h4a: freq_code =6'd46;
8'h59: freq_code =6'd47;
default: freq_code = 6'd48; //If the key that is pressed is not a note, default to sending the frequency code which corresponds to the silent frequency.
endcase
endmodule

module code_to_freq(this_freq_code, freq);
//Port declarations
input [5:0]this_freq_code;
output reg [17:0]freq;

//Conversion process
always @ (this_freq_code)
case (this_freq_code)
6'd0: freq = 18'd55; //b1101110; //A, 1
6'd1: freq = 18'd58; //b1110101; //A#, 2
6'd2: freq = 18'd62;//b1111011; //B, 3
6'd3: freq = 18'd65; //b10000011;//C 4
6'd4: freq = 18'd69; //b10001011;//C# 5
6'd5: freq = 18'd73; //b10010011; //D 6
6'd6: freq = 18'd78;//b10011100; //D# 7
6'd7: freq = 18'd82; //E 8
6'd8: freq = 18'd87; //F 9
6'd9: freq = 18'd92; //F# 0
6'd10: freq = 18'd98; //G -
6'd11: freq = 18'd104; //G# 
//2nd
6'd12: freq = 18'd110; //A q
6'd13: freq = 18'd117; //A# w
6'd14: freq = 18'd123; //B e
6'd15: freq = 18'd131; //C r
6'd16: freq = 18'd139;//C# t
6'd17: freq = 18'd147;// D  y
6'd18: freq = 18'd156; //D# u
6'd19: freq = 18'd165; //E i
6'd20: freq = 18'd175;//F o
6'd21: freq = 18'd185;//F# p
6'd22: freq = 18'd196;//G{
6'd23: freq = 18'd208;//G#}
//3rd
6'd24: freq = 18'd220; //Aa
6'd25: freq = 18'd233;//A# s
6'd26: freq = 18'd245;//Bd
6'd27: freq = 18'd262;//C f
6'd28: freq = 18'd277;//C# g
6'd29: freq = 18'd249;//D k
6'd30: freq = 18'd311;//D# l
6'd31: freq = 18'd330;//E ;
6'd32: freq = 18'd349;//F"
6'd33: freq = 18'd370;//F#
6'd34: freq = 18'd392;//G
6'd35: freq = 18'd415;//G#
//4th
6'd36: freq = 18'd440;
6'd37: freq = 18'd466; //This is the silence value, which sets the frequency to 0.
6'd38: freq = 18'd494; //Aa
6'd39: freq = 18'd523;//A# s
6'd40: freq = 18'd554;//Bd
6'd41: freq = 18'd587;//C f
6'd42: freq = 18'd622;//C# g
6'd43: freq = 18'd659;//D k
6'd44: freq = 18'd698;//D# l
6'd45: freq = 18'd740;//E ;
6'd46: freq = 18'd784;//F"
6'd47: freq = 18'd831;//F#
default : freq = 18'd0;




endcase
endmodule

module interval_encoder(sw, harmony_interval);
//Port declarations
input [12:0] sw;
output reg [5:0]harmony_interval;

//Determine interval size
always @ (sw)
case (sw)
13'd1: harmony_interval = 6'd0;
13'd2: harmony_interval = 6'd1;
13'd4: harmony_interval = 6'd2;
13'd8: harmony_interval = 6'd3;
13'd16: harmony_interval = 6'd4;
13'd32: harmony_interval = 6'd5;
13'd64: harmony_interval = 6'd6;
13'd128: harmony_interval = 6'd7;
13'd256: harmony_interval = 6'd8;
13'd512: harmony_interval = 6'd9;
13'd1024: harmony_interval = 6'd10;
13'd2048: harmony_interval = 6'd11;
13'd4096: harmony_interval = 6'd12;

//need to fill in the upper range of values for harmonization
endcase
endmodule

module harmonizer(freq_code, harmony_interval, harmonized_code);
//Port declarations
input [5:0]freq_code;
input [5:0]harmony_interval;
output [5:0]harmonized_code;
assign harmonized_code = freq_code + harmony_interval;
endmodule	

module harmonizer_on_off(switch, freq_code_h, freq_code_h_final);
//Port declarations
input switch;
input [5:0]freq_code_h;
output reg [5:0]freq_code_h_final;

//Selection process
always @ (switch)
    if (!switch)
	    freq_code_h_final = 6'd36;
	 else
		 freq_code_h_final = freq_code_h;
endmodule
module char_encoder(freq_code, hex0, hex1);
	input [5:0]freq_code;
	output reg [3:0] hex0, hex1;

	always @ (freq_code)
	case(freq_code)
	6'd0: hex0 = 4'h1; //b1101110; //A, 1
	6'd1: hex0 = 4'h1; //b1110101; //A#, 2
	6'd2: hex0 = 4'h2;//b1111011; //B, 3
	6'd3: hex0 = 4'h3; //b10000011;//C 4
	6'd4: hex0 = 4'h3; //b10001011;//C# 5
	6'd5: hex0 = 4'h4; //b10010011; //D 6
	6'd6: hex0 = 4'h4;//b10011100; //D# 7
	6'd7: hex0 = 4'h5; //E 8
	6'd8: hex0 = 4'h6; //F 9
	6'd9: hex0 = 4'h6; //F# 0
	6'd10: hex0 = 4'h7; //G -
	6'd11: hex0 = 4'h7; //G# 
	//2nd
	6'd12: hex0 = 4'h1; //b1101110; //A, 1
	6'd13: hex0 = 4'h1; //b1110101; //A#, 2
	6'd14: hex0 = 4'h2;//b1111011; //B, 3
	6'd15: hex0 = 4'h3; //b10000011;//C 4
	6'd16: hex0 = 4'h3; //b10001011;//C# 5
	6'd17: hex0 = 4'h4; //b10010011; //D 6
	6'd18: hex0 = 4'h4;//b10011100; //D# 7
	6'd19: hex0 = 4'h5; //E 8
	6'd20: hex0 = 4'h6; //F 9
	6'd21: hex0 = 4'h6; //F# 0
	6'd22: hex0 = 4'h7; //G -
	6'd23: hex0 = 4'h7; //G# 
	//3rd
	6'd24: hex0 = 4'h1; //b1101110; //A, 1
	6'd25: hex0 = 4'h1; //b1110101; //A#, 2
	6'd26: hex0 = 4'h2;//b1111011; //B, 3
	6'd27: hex0 = 4'h3; //b10000011;//C 4
	6'd28: hex0 = 4'h3; //b10001011;//C# 5
	6'd29: hex0 = 4'h4; //b10010011; //D 6
	6'd30: hex0 = 4'h4;//b10011100; //D# 7
	6'd31: hex0 = 4'h5; //E 8
	6'd32: hex0 = 4'h6; //F 9
	6'd33: hex0 = 4'h6; //F# 0
	6'd34: hex0 = 4'h7; //G -
	6'd35: hex0 = 4'h7; //G# 
	//4th
	6'd36: hex0 = 4'h1; //b1101110; //A, 1
	6'd37: hex0 = 4'h1; //b1110101; //A#, 2
	6'd38: hex0 = 4'h2;//b1111011; //B, 3
	6'd39: hex0 = 4'h3; //b10000011;//C 4
	6'd40: hex0 = 4'h3; //b10001011;//C# 5
	6'd41: hex0 = 4'h4; //b10010011; //D 6
	6'd42: hex0 = 4'h4;//b10011100; //D# 7
	6'd43: hex0 = 4'h5; //E 8
	6'd44: hex0 = 4'h6; //F 9
	6'd45: hex0 = 4'h6; //F# 0
	6'd46: hex0 = 4'h7; //G -
	6'd47: hex0 = 4'h7; //G# 
	default : hex0 = 4'h0;
	endcase
	always @(freq_code)
	case(freq_code)
	6'd1: hex1 = 4'h3; //b1110101; //A#, 2
	6'd4: hex1 = 4'h3; //b10001011;//C# 5
	6'd6: hex1 = 4'h3;//b10011100; //D# 7
	6'd9: hex1 = 4'h3; //F# 0
	6'd11: hex1 = 4'h3; //G# 
	//2nd
	6'd13: hex1 = 4'h3; //b1110101; //A#, 2
	6'd16: hex1 = 4'h3; //b10001011;//C# 5
	6'd18: hex1 = 4'h3;//b10011100; //D# 7
	6'd21: hex1 = 4'h3; //F# 0
	6'd23: hex1 = 4'h3; //G# 
	//3rd
	6'd25: hex1 = 4'h3; //b1110101; //A#, 2
	6'd28: hex1 = 4'h3; //b10001011;//C# 5
	6'd30: hex1 = 4'h3;//b10011100; //D# 7
	6'd33: hex1 = 4'h3; //F# 0
	6'd35: hex1 = 4'h3; //G# 
	//4th
	6'd37: hex1 = 4'h3; //b1110101; //A#, 2
	6'd40: hex1 = 4'h3; //b10001011;//C# 5
	6'd42: hex1 = 4'h3;//b10011100; //D# 7
	6'd45: hex1 = 4'h3; //F# 0
	6'd47: hex1 = 4'h3; //G# 
	default : hex1 = 4'h0;
	endcase
endmodule
