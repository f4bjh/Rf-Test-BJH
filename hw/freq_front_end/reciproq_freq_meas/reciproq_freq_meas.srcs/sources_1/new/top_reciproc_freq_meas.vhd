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
    LED0          : out std_logic;
    NCO_OUT       : out std_logic
  );
end entity;

architecture rtl of top_reciproc_freq_meas is
  constant USE_FRAC_INTERP : boolean := false; -- mettre true pour activer
  constant USE_CALC_FREQ : boolean := false; -- mettre true pour activer
  
  -- SPI interface
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

  --freq_counter
  signal start_meas          : std_logic;
  signal meas_done      : std_logic;                            -- measurement (capture of N periods) done (pulse)
  signal start_tick    : unsigned(63 downto 0);
  signal end_tick      : unsigned(63 downto 0);
  signal N_counted     : unsigned(31 downto 0);
  signal ready         : std_logic;
 
  
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
    nco_freq_valid=> nco_freq_valid,
    
    start_meas => start_meas,
    meas_done  => meas_done,
    start_tick  => start_tick,
    end_tick    => end_tick,
    N_counted  => N_counted 

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
      start      => start_meas,
      ready      => ready,
      meas_done  => meas_done,
      status     => open,
      start_tick => start_tick,
      end_tick   => end_tick,
      N_counted  => N_counted
    );


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
