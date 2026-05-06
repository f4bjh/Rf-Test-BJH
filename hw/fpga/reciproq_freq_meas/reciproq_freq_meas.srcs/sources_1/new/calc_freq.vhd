-- calc_freq.vhd
-- Calculate frequency estimate and quantization error from captured ticks
library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;

entity calc_freq is
  generic (
    FREF_HZ     : natural := 12_000_000;
    REF_CNT_W   : natural := 64;
    FRAC_BITS   : natural := 32;         -- fractional bits in f_est (Q.F)
    FQ_INT_BITS : natural := 32          -- integer bits for f_est output (Hz)
  );
  port (
    clk         : in  std_logic;
    rst_n       : in  std_logic;
    start       : in  std_logic;         -- pulse to trigger calculation
    interp_period : in unsigned(REF_CNT_W-1 downto 0); -- filtered period from frac_interp
    N_counted   : in  unsigned(31 downto 0);
    done        : out std_logic;         -- pulse when calculation completed
    f_est_int   : out unsigned(FQ_INT_BITS-1 downto 0);    -- integer part (Hz)
    f_est_frac  : out unsigned(FRAC_BITS-1 downto 0);     -- fractional part (Q.FRAC_BITS)
    df_q16_16   : out unsigned(31 downto 0)               -- quantization error in Q16.16
  );
end entity;

architecture rtl of calc_freq is
  constant FQ_WIDTH : natural := FQ_INT_BITS + FRAC_BITS;
begin
  process(clk)
    variable num_big : unsigned(255 downto 0);
    variable f_q_big : unsigned(FQ_WIDTH-1 downto 0);
    variable f_q_sq  : unsigned(2*FQ_WIDTH-1 downto 0);
    variable denom_df: unsigned(255 downto 0);
    variable df_tmp  : unsigned(255 downto 0);
    variable shifted : unsigned(255 downto 0);
  begin
    if rising_edge(clk) then
      if rst_n = '0' then
        done       <= '0';
        f_est_int  <= (others => '0');
        f_est_frac <= (others => '0');
        df_q16_16  <= (others => '0');
      else
        done <= '0';
        if start = '1' then
          if interp_period = 0 or N_counted = 0 then
            f_est_int  <= (others => '0');
            f_est_frac <= (others => '0');
            df_q16_16  <= (others => '0');
            done <= '1';
          else
            -- numerator = FREF_HZ * N_counted << FRAC_BITS
            num_big := resize(N_counted * to_unsigned(FREF_HZ, N_counted'length), num_big'length);
            num_big := num_big sll FRAC_BITS;

            -- divide by interp_period to get f_q (Q.FRAC_BITS)
            f_q_big := resize(num_big / resize(interp_period, 256), f_q_big'length);

            -- output integer & fractional parts
            f_est_int  <= f_q_big(FRAC_BITS + FQ_INT_BITS - 1 downto FRAC_BITS);
            f_est_frac <= f_q_big(FRAC_BITS-1 downto 0);

            -- compute quantization error df_q16_16
            f_q_sq := resize(resize(f_q_big, f_q_sq'length) * resize(f_q_big, f_q_sq'length), f_q_sq'length);
            denom_df := resize(to_unsigned(FREF_HZ, 256) * resize(N_counted, 256), denom_df'length);
            if denom_df = 0 then
              df_q16_16 <= (others => '0');
            else
              df_tmp := resize(f_q_sq / denom_df, df_tmp'length);
              if (2*FRAC_BITS) > 16 then
                shifted := df_tmp srl (2*FRAC_BITS - 16);
              else
                shifted := df_tmp sll (16 - 2*FRAC_BITS);
              end if;
              df_q16_16 <= resize(shifted(31 downto 0), 32);
            end if;

            done <= '1';
          end if;
        end if;
      end if;
    end if;
  end process;
end architecture;
