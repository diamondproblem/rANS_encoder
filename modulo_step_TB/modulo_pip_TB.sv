module modulo_pip_rtl_TB;
logic clock, reset;
logic [31:0] x_mod_z_low;
logic [31:0] x_mod_z_high;
logic [31:0] x_high = 32'd6144;
logic [31:0] x_low = 32'd0;
logic [31:0] z_high = 32'd0;
logic [31:0] z_low = 32'd3;
logic valid_out;


modulo_rtl modulo_rtl_inst ( clock, reset, x_high, x_low, z_high, z_low, valid_out, x_mod_z_high, x_mod_z_low);

initial
begin
 reset <= 1'b1;
 #10 reset <= 1'b0;
 #900 reset <= 1'b1;
 x_high = 32'd6144;
 x_low = 32'd0;
 z_high = 32'd0;
 z_low = 32'd3;
 #50 reset <= 1'b0;
end

initial
begin
 clock <= 1'b0;


end
always
 #5 clock <= ~clock;

endmodule