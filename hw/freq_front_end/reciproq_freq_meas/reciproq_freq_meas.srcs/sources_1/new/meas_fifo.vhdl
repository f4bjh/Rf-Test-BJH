library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;

entity meas_fifo is
  generic (
    FIFO_DEPTH_PER_VAR : integer := 8;
    FIFO_DEPTH         : integer := 32
  );
  port (
    clk   : in  std_logic;
    reset_n : in std_logic;

    -- write
    fifo_wr_en        : in  std_logic;
    delta_tick   : in  unsigned(63 downto 0);
    N_counted    : in  unsigned(31 downto 0);
    wr_counter   : out std_logic_vector(7 downto 0);

    -- read
    fifo_rd_en        : in  std_logic;
    fifo_rd_data      : out std_logic_vector(31 downto 0);
    fifo_data_type_req : in  std_logic_vector(7 downto 0);
    
    -- status
    fifo_full    : out std_logic
  );
end entity;

architecture rtl of meas_fifo is

constant FPGA_COUNTER_PTR_START : integer := 0;
constant DELTA_TICK_PTR_START   : integer := 8;
constant N_COUNTED_PTR_START    : integer := 24;

type fifo_array is array (0 to FIFO_DEPTH-1) of std_logic_vector(31 downto 0);

signal fifo_mem : fifo_array := (others => (others => '0'));
signal fifo_count : integer range 0 to FIFO_DEPTH_PER_VAR := 0;
signal fifo_full_r : std_logic := '0';

signal wr_cnt : integer range 0 to FIFO_DEPTH_PER_VAR := 0;
signal rd_cnt : integer range 0 to FIFO_DEPTH_PER_VAR := 0;

signal ptr_wr_fpga : integer range 0 to FIFO_DEPTH_PER_VAR-1 := 0;
signal ptr_wr_dt   : integer range 0 to FIFO_DEPTH_PER_VAR-1 := 0;
signal ptr_wr_n    : integer range 0 to FIFO_DEPTH_PER_VAR-1 := 0;

signal ptr_rd_fpga : integer range 0 to FIFO_DEPTH_PER_VAR-1 := 0;
signal ptr_rd_dt   : integer range 0 to (2*FIFO_DEPTH_PER_VAR)-1 := 0;
signal ptr_rd_n    : integer range 0 to FIFO_DEPTH_PER_VAR-1 := 0;
signal fifo_rd_ptr        : integer range 0 to FIFO_DEPTH-1 := 0;

begin

process(clk)
begin
  if rising_edge(clk) then
--    if reset_n = '0' then
--    else
 
      if fifo_wr_en = '1' and fifo_full_r = '0' then
        fifo_mem(FPGA_COUNTER_PTR_START + ptr_wr_fpga) <= std_logic_vector(to_unsigned(wr_cnt,32));
        fifo_mem(DELTA_TICK_PTR_START + 2*ptr_wr_dt)     <= std_logic_vector(delta_tick(31 downto 0));
        fifo_mem(DELTA_TICK_PTR_START + 2*ptr_wr_dt + 1) <= std_logic_vector(delta_tick(63 downto 32));
        fifo_mem(N_COUNTED_PTR_START + ptr_wr_n)       <= std_logic_vector(N_counted);

        wr_cnt <= (wr_cnt + 1) mod FIFO_DEPTH_PER_VAR;

        ptr_wr_fpga <= (ptr_wr_fpga + 1) mod FIFO_DEPTH_PER_VAR;
        ptr_wr_dt   <= (ptr_wr_dt + 1) mod FIFO_DEPTH_PER_VAR;
        ptr_wr_n    <= (ptr_wr_n + 1) mod FIFO_DEPTH_PER_VAR;

      end if;
      
      if fifo_rd_en = '1' then
 
            case fifo_data_type_req is 
                when  x"00" =>
                        fifo_rd_ptr <= 0 +  ptr_rd_fpga; 
                        ptr_rd_fpga <= (ptr_rd_fpga + 1) mod (FIFO_DEPTH_PER_VAR+1);                        
                     
                when  x"01" =>
                        fifo_rd_ptr <= 8 +  ptr_rd_dt; 
                        ptr_rd_dt <= (ptr_rd_dt + 1) mod (2*FIFO_DEPTH_PER_VAR+1);                        
                
                when  x"02" =>
                        fifo_rd_ptr <= 24 +  ptr_rd_n; 
                        ptr_rd_n <= (ptr_rd_n + 1) mod (FIFO_DEPTH_PER_VAR+1);                        
                    
                when others =>
                     null;
            end case;
          
        end if;
     
        if (fifo_wr_en = '1' and fifo_full_r = '0') and
            not (fifo_rd_en = '1' and fifo_count > 0) then
            fifo_count <= fifo_count + 1;

        elsif (fifo_rd_en = '1' and fifo_count > 0) and
            not (fifo_wr_en = '1' and fifo_full_r = '0') then
            fifo_count <= fifo_count - 1;

        end if;
    
    end if;
  --end if;
end process;

--fifo_count <= fifo_count_i;
fifo_full_r <= '1' when fifo_count = FIFO_DEPTH_PER_VAR else '0';
fifo_full <= fifo_full_r;
wr_counter <= std_logic_vector(to_unsigned(wr_cnt,8));
fifo_rd_data <= fifo_mem(fifo_rd_ptr) when (fifo_rd_ptr < 32) else (others => '0');

end architecture;