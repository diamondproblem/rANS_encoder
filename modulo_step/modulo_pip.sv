parameter integer W = 64; 

function logic[W-1:0] modulo_step (input logic[W-1:0] x_in, input logic[W-1:0] z_in, input logic[(W-1)*2:0] fi, output logic[W-1:0] x_out, output logic[W-1:0] z_out);

$display("z_in = %0d, x_in =%0d, fi=%0d", z_in, x_in, fi);
if (x_in >= fi) x_out = x_in - fi;
else x_out = x_in;
z_out = z_in;
$display("z_out = %0d, x_out =%0d", z_out, x_out);


endfunction


module modulo_rtl (
input logic clock, 
input logic reset,  
input logic [W/2-1:0] x_high, 
input logic [W/2-1:0] x_low, 
input logic [W/2-1:0] z_high,
input logic [W/2-1:0] z_low,
output logic valid_out,
output logic [W/2-1:0] x_mod_z_high,
output logic [W/2-1:0] x_mod_z_low);

logic [(W*2)-1:0] x_cur;
logic [(W*2)-1:0] z_cur;

assign x_cur[(W*2)-1:32] = x_high;
assign x_cur[31:0] = x_low;
assign z_cur[(W*2)-1:32] = z_high;
assign z_cur[31:0] = z_low;

logic run_pip = 1;

logic special_z_0 = 0;
logic special_z_1 = 0;
logic special_z_2_0 = 0;
logic special_z_2_1 = 0;

logic [7:0] start_out_reg = 255;
logic [7:0] start_reg_first = 255;
logic [(W*2) - 1:0] fi_calc_0;
logic [(W*2) - 1:0] fi_calc_8;
logic [(W*2) - 1:0] fi_calc_16;
logic [(W*2) - 1:0] fi_calc_24;
logic [(W*2) - 1:0] fi_calc_32;
logic [(W*2) - 1:0] fi_calc_40;
logic [(W*2) - 1:0] fi_calc_48;
logic [(W*2) - 1:0] fi_calc_56;

integer i_num = 0;
integer pipe_stop = 0;
integer pipe_start = 1;

logic [0:7][7:0] start_num = {8'd255, 8'd255, 8'd255, 8'd255, 8'd255, 8'd255, 8'd255, 8'd255};

integer j_counter_0 = 8;
integer j_counter_8 = 16;
integer j_counter_16 = 24;
integer j_counter_24 = 32;
integer j_counter_32 = 40;
integer j_counter_40 = 48;
integer j_counter_48 = 56;
integer j_counter_56 = 64;

logic [7:0] min_start = 0;

logic [W-1:0][W-1:0] x_tab;
logic [W-1:0][W-1:0] z_tab;

logic [0:W-2][W-1:0] pow_two_tab = {
64'b1,
64'b10,
64'b100,
64'b1000,
64'b10000,
64'b100000,
64'b1000000,
64'b10000000,
64'b100000000,
64'b1000000000,
64'b10000000000,
64'b100000000000,
64'b1000000000000,
64'b10000000000000,
64'b100000000000000,
64'b1000000000000000,
64'b10000000000000000,
64'b100000000000000000,
64'b1000000000000000000,
64'b10000000000000000000,
64'b100000000000000000000,
64'b1000000000000000000000,
64'b10000000000000000000000,
64'b100000000000000000000000,
64'b1000000000000000000000000,
64'b10000000000000000000000000,
64'b100000000000000000000000000,
64'b1000000000000000000000000000,
64'b10000000000000000000000000000,
64'b100000000000000000000000000000,
64'b1000000000000000000000000000000,
64'b10000000000000000000000000000000,
64'b100000000000000000000000000000000,
64'b1000000000000000000000000000000000,
64'b10000000000000000000000000000000000,
64'b100000000000000000000000000000000000,
64'b1000000000000000000000000000000000000,
64'b10000000000000000000000000000000000000,
64'b100000000000000000000000000000000000000,
64'b1000000000000000000000000000000000000000,
64'b10000000000000000000000000000000000000000,
64'b100000000000000000000000000000000000000000,
64'b1000000000000000000000000000000000000000000,
64'b10000000000000000000000000000000000000000000,
64'b100000000000000000000000000000000000000000000,
64'b1000000000000000000000000000000000000000000000,
64'b10000000000000000000000000000000000000000000000,
64'b100000000000000000000000000000000000000000000000,
64'b1000000000000000000000000000000000000000000000000,
64'b10000000000000000000000000000000000000000000000000,
64'b100000000000000000000000000000000000000000000000000,
64'b1000000000000000000000000000000000000000000000000000,
64'b10000000000000000000000000000000000000000000000000000,
64'b100000000000000000000000000000000000000000000000000000,
64'b1000000000000000000000000000000000000000000000000000000,
64'b10000000000000000000000000000000000000000000000000000000,
64'b100000000000000000000000000000000000000000000000000000000,
64'b1000000000000000000000000000000000000000000000000000000000,
64'b10000000000000000000000000000000000000000000000000000000000,
64'b100000000000000000000000000000000000000000000000000000000000,
64'b1000000000000000000000000000000000000000000000000000000000000,
64'b10000000000000000000000000000000000000000000000000000000000000,
64'b100000000000000000000000000000000000000000000000000000000000000
};

logic [0:2][W-1:0] x_z_tab;

always@(posedge clock)
begin
if (reset == 1'b1) 
begin
pipe_stop <= 0;
run_pip <= 1;
end
end

//Search ranges to find minimal start step from every range
always@(posedge clock)
begin
if (reset != 1'b1 && run_pip == 1 )
begin
    if (j_counter_0 >= 0 && start_num[0][7:0] == 255)
    begin
        if (x_cur >= pow_two_tab[j_counter_0][W-1:0]*z_cur) start_num[0][7:0] <= j_counter_0;
        else j_counter_0 <= j_counter_0 - 1;
    end
end
end

always@(posedge clock)
begin
if (reset != 1'b1 && run_pip == 1 )
begin
    if (j_counter_8 > 8 && start_num[1][7:0] == 255)
    begin
        if (x_cur >= pow_two_tab[j_counter_8][W-1:0]*z_cur) start_num[1][7:0] <= j_counter_8;
        else j_counter_8 <= j_counter_8 - 1;
    end
end
end

always@(posedge clock)
begin
if (reset != 1'b1 && run_pip == 1 )
begin
    if (j_counter_16 > 16 && start_num[2][7:0] == 255)
    begin
        if (x_cur >= pow_two_tab[j_counter_16][W-1:0]*z_cur) start_num[2][7:0] <= j_counter_16;
        else j_counter_16 <= j_counter_16 - 1;
    end
end
end

always@(posedge clock)
begin
if (reset != 1'b1 && run_pip == 1 )
begin
    if (j_counter_24 > 24 && start_num[3][7:0] == 255)
    begin
        if (x_cur >= pow_two_tab[j_counter_24][W-1:0]*z_cur) start_num[3][7:0] <= j_counter_24;
        else j_counter_24 <= j_counter_24 - 1;
    end
end
end

always@(posedge clock)
begin
if (reset != 1'b1 && run_pip == 1 )
begin
    if (j_counter_32 > 32 && start_num[4][7:0] == 255)
    begin
        if (x_cur >= pow_two_tab[j_counter_32][W-1:0]*z_cur) start_num[4][7:0] <= j_counter_32;
        else j_counter_32 <= j_counter_32 - 1;
    end
end
end

always@(posedge clock)
begin
if (reset != 1'b1 && run_pip == 1 )
begin
    if (j_counter_40 > 40 && start_num[5][7:0] == 255)
    begin
        if (x_cur >= pow_two_tab[j_counter_40][W-1:0]*z_cur) start_num[5][7:0] <= j_counter_40;
        else j_counter_40 <= j_counter_40 - 1;
    end
end
end

always@(posedge clock)
begin
if (reset != 1'b1 && run_pip == 1 )
begin
    if (j_counter_48 > 48 && start_num[6][7:0] == 255)
    begin
        if (x_cur >= pow_two_tab[j_counter_48][W-1:0]*z_cur) start_num[6][7:0] <= j_counter_48;
        else j_counter_48 <= j_counter_48 - 1;
    end
end
end

always@(posedge clock)
begin
if (reset != 1'b1 && run_pip == 1 )
begin
    if (j_counter_56 > 56 && start_num[7][7:0] == 255)
    begin
        if (x_cur >= pow_two_tab[j_counter_56][W-1:0]*z_cur) start_num[7][7:0] <= j_counter_56;
        else j_counter_56 <= j_counter_56 - 1;
    end
end
end

// Find the biggest possible starting index
always@(posedge clock)
begin

if(reset != 1'b1 && i_num < 8 && run_pip == 1 && pipe_stop != 1)
begin
// Spend 8 bars on checking the ranges
    if ((j_counter_0 == 0 || start_num[0][7:0] != 8'd255) &&
    (j_counter_8 == 8 || start_num[1][7:0] != 8'd255) &&
    (j_counter_16 == 16 || start_num[2][7:0] != 8'd255) &&
    (j_counter_24 == 24 || start_num[3][7:0] != 8'd255) &&
    (j_counter_32 == 32 || start_num[4][7:0] != 8'd255) &&
    (j_counter_40 == 40 || start_num[5][7:0] != 8'd255) &&
    (j_counter_48 == 48 || start_num[6][7:0] != 8'd255) &&
    (j_counter_56 == 56 || start_num[7][7:0] != 8'd255))
    begin
        if (start_num[i_num][7:0] >= min_start && start_num[i_num] != 255) min_start <= start_num[i_num][7:0];
        else if (start_num[i_num] != 255) min_start <= min_start;
        i_num <= i_num + 1;
    end
end

if (reset != 1'b1 && i_num >= 8 && run_pip == 1 && pipe_stop != 1)
    begin
        if (min_start < 8'd255) start_reg_first <= min_start;
        else start_reg_first <= 8'd255;
    end

if (reset != 1'b1 && run_pip == 1)
begin
    // If-else istructions for special cases
    start_out_reg <= start_reg_first;
    if (x_cur < z_cur || z_cur == 0) 
    begin
        special_z_0 <= 0;
        x_tab[0] <= x_cur;
        pipe_stop <= 1;
        run_pip <= 0;
    end
    else if (z_cur == 1 || z_cur == x_cur)
    begin
        special_z_1 <= 1;
        x_tab[0] <= 0;
        pipe_stop <= 1;
        run_pip <= 0;
    end
    else if (z_cur == 2) 
    begin
        special_z_2_0 <= 1;
        if(x_cur != (x_cur/2)*2)
        begin
            x_tab[0] <= 1;
            pipe_stop <= 1;
            run_pip <= 0;
        end
        else
        begin
            x_tab[0] <= 0;
            pipe_stop <= 1;
            run_pip <= 0;
        end
    end
    // Pipeline implementation
    else if (start_out_reg > 0 && start_out_reg != 255 && pipe_stop != 1 && (special_z_0 == 0 &&
        special_z_1 == 0 && special_z_2_0 == 0 && special_z_2_1 == 0))
    begin
            if (pipe_start == 1)
            begin
                x_tab[start_out_reg] = x_cur;
                z_tab[start_out_reg] = z_cur;
                pipe_start = 0;
            end
            pipe_stop <= 0;
            modulo_step (x_tab[start_out_reg][W-1:0], z_tab[start_out_reg][W-1:0], pow_two_tab[start_out_reg][W-1:0]*z_cur,  x_tab[start_out_reg - 1][W-1:0], z_tab[start_out_reg - 1][W-1:0]);
            start_out_reg <= start_out_reg - 1;
    end
    else if (start_out_reg == 0 && pipe_stop != 1 && (special_z_0 == 0 &&
        special_z_1 == 0 && special_z_2_0 == 0 && special_z_2_1 == 0))
    begin
            if (pipe_start == 1)
            begin
                x_tab[start_out_reg] = x_cur;
                z_tab[start_out_reg] = z_cur;
                pipe_start = 0;
            end
            modulo_step (x_tab[start_out_reg], z_tab[start_out_reg], pow_two_tab[start_out_reg]*z_cur,  x_tab[0], z_tab[0]);
            pipe_stop <= 1;
            run_pip <= 0;
    end
    
end
// Restore default values
if ((reset != 1'b1 && pipe_stop == 1))
        begin
            start_num <= {8'd255, 8'd255, 8'd255, 8'd255, 8'd255, 8'd255, 8'd255, 8'd255};
            j_counter_0 <= 8;
            j_counter_8 <= 16;
            j_counter_16 <= 24;
            j_counter_24 <= 32;
            j_counter_32 <= 40;
            j_counter_40 <= 48;
            j_counter_48 <= 56;
            j_counter_56 <= 64;
            special_z_0 <= 0;
            special_z_1 <= 0;
            special_z_2_0 <= 0;
            special_z_2_1 <= 0;
            special_z_2_1 <= 0;
            min_start <= 0;
            start_out_reg <= 255;
            start_reg_first <= 255;
            pipe_start <= 1;
            i_num <= 0;       
end

end

assign x_mod_z_high = (pipe_stop == 1)?x_tab[0][W-1:32]:64'd42;
assign x_mod_z_low = (pipe_stop == 1)?x_tab[0][W/2-1:0]:64'd42;
assign valid_out = (pipe_stop == 1 && reset != 1)? 1'b1 : 1'b0;

endmodule