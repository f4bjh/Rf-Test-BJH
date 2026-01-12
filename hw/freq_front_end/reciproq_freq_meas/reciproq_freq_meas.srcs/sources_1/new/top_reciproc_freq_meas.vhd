-- top_reciproc_freq_meas.vhd
library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;

entity top_reciproc_freq_meas is
  generic (
    FREF_HZ : natural := 12_000_000
  );
  port(
    clk_master    : in  std_logic;
    reset_n       : in  std_logic;
    sclk          : in  std_logic;
    mosi          : in  std_logic;
    miso          : out  std_logic;
    cs_n          : in  std_logic;
    hf_freq_in    : in  std_logic;
    start         : in  std_logic;
    ready         : out std_logic;
    done          : out std_logic;
    f_calc        : out unsigned(31 downto 0);
    LED0          : out std_logic;
    NCO_OUT       : out std_logic;

    -- Pour testbench
    start_tick    : out unsigned(63 downto 0);
    end_tick      : out unsigned(63 downto 0);
    N_counted     : out unsigned(31 downto 0);
    interp_period : out unsigned(63 downto 0);
    interp_valid  : out std_logic
  );
end entity;

architecture rtl of top_reciproc_freq_meas is
  constant USE_FRAC_INTERP : boolean := false; -- mettre true pour activer
  constant USE_CALC_FREQ : boolean := false; -- mettre true pour activer
  
  -- SPI interface
  signal spi_word       : std_logic_vector(31 downto 0);
  signal spi_data_valid : std_logic;
  signal     rx_word    : std_logic_vector(31 downto 0);
  signal      rx_valid  : std_logic;
  signal      tx_word   : std_logic_vector(31 downto 0);
  signal      tx_load   : std_logic;
  signal      tx_busy   : std_logic;
  
  -- LED
  signal led_state      : std_logic := '0';
  signal led_on, led_off, led_toggle : std_logic;

  -- NCO
  signal nco_freq_word      : unsigned(23 downto 0);
  signal nco_freq_valid     : std_logic;

  signal meas_done      : std_logic;                            -- measurement (capture of N periods) done (pulse)
  signal calc_done : std_logic;

begin

  ------------------------------------------------------------------------
  -- SPI Slave
  ------------------------------------------------------------------------
  
  spi_slave_inst : entity work.spi_slave
 port map (
    clk     => clk_master,
    reset_n => reset_n,
    sclk    => sclk,
    cs_n    => cs_n,
    mosi    => mosi,
    miso    => miso,

    rx_word  => rx_word,
    rx_valid => rx_valid,

    tx_word  => tx_word,
    tx_load  => tx_load,
    tx_busy  => tx_busy
);

  ------------------------------------------------------------------------
  -- SPI Decode
  ------------------------------------------------------------------------
  
spi_decode_inst : entity work.spi_decode
port map (
    clk     => clk_master,
    reset_n => reset_n,

    rx_word  => rx_word,
    rx_valid => rx_valid,

    tx_word  => tx_word,
    tx_load  => tx_load,
    tx_busy  => tx_busy,

    led_on        => led_on,
    led_off       => led_off,
    led_toggle    => led_toggle,
    nco_freq_word => nco_freq_word,
    nco_freq_valid=> nco_freq_valid
);

  ------------------------------------------------------------------------
  -- NCO
  ------------------------------------------------------------------------
  nco_inst : entity work.nco
    port map (
      clk_in     => clk_master,
      reset_n    => reset_n,
      freq_word  => nco_freq_word,
      freq_valid => nco_freq_valid,
      clk_out    => NCO_OUT
    );

  ------------------------------------------------------------------------
  -- Frequency counter
  ------------------------------------------------------------------------
  u_freq_counter: entity work.freq_counter
    generic map (FREF_HZ => FREF_HZ)
    port map (
      clk_ref    => clk_master,
      rst_n      => reset_n,
      sig_in     => hf_freq_in,
      cfg_N      => to_unsigned(1000, 32),
      start      => start,
      ready      => ready,
      meas_done  => meas_done,
      status     => open,
      start_tick => start_tick,
      end_tick   => end_tick,
      N_counted  => N_counted
    );

  ------------------------------------------------------------------------
  -- Fractional interpolation
  ------------------------------------------------------------------------
  frac_interp_block: if USE_FRAC_INTERP generate
      u_frac_interp: entity work.frac_interp
        port map (
          clk        => clk_master,
          rst_n      => reset_n,
          valid_in   => meas_done,
          raw_period => end_tick - start_tick,
          interp_out => interp_period,
          valid_out  => interp_valid
        );
  end generate frac_interp_block;
  
  ------------------------------------------------------------------------
  -- Complete frequency calculation using calc_freq
  ------------------------------------------------------------------------
  calc_freq_block: if USE_CALC_FREQ generate
      u_calc_freq: entity work.calc_freq
        generic map (FREF_HZ => FREF_HZ)
        port map (
          clk          => clk_master,
          rst_n        => reset_n,
          start        => interp_valid,
          interp_period => interp_period,
          N_counted    => N_counted,
          done         => calc_done,
          f_est_int    => f_calc,
          f_est_frac   => open,
          df_q16_16    => open
        );
  end generate calc_freq_block;

-- assignation à meas_done
 done <= calc_done when USE_CALC_FREQ else meas_done;

    
  ------------------------------------------------------------------------
  -- LED management
  ------------------------------------------------------------------------
  process(clk_master, reset_n)
  begin
    if reset_n = '0' then
      led_state <= '0';
    elsif rising_edge(clk_master) then
      if led_on = '1' then
        led_state <= '1';
      elsif led_off = '1' then
        led_state <= '0';
      elsif led_toggle = '1' then
        led_state <= not led_state;
      end if;
    end if;
  end process;

  LED0 <= led_state;

end architecture;
