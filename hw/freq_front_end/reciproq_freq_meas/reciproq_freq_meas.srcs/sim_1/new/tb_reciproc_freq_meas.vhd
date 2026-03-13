library ieee; 
use ieee.std_logic_1164.all; 
use ieee.numeric_std.all; 
use std.textio.all;

entity tb_top_reciproc_freq_meas is 
end entity;

architecture sim of tb_top_reciproc_freq_meas is

  type t_word32_array is array (natural range <>) of std_logic_vector(31 downto 0);

  constant FREF_HZ  : integer := 12_000_000;
  constant CLK_PER  : time := 83.333 ns;
  constant SPI_CLK  : integer := 250_000;
  constant SPI_CLK_PER : time := 4 us;
  constant STABILIZATION_TIME : time := 100 us;

  signal clk_ref   : std_logic := '0';
  signal rst_n     : std_logic := '0';
  signal sig_in    : std_logic := '0';
  signal sig_gen_out : std_logic := '0';
  signal use_nco_out : std_logic := '0';
  signal start     : std_logic := '0';
  signal done      : std_logic;
  signal f_calc    : unsigned(31 downto 0);

  signal sck       : std_logic := '0';
  signal mosi      : std_logic := '0';
  signal miso      : std_logic := '0';
  signal cs_n      : std_logic := '1';
  signal LED0      : std_logic;
  signal NCO_OUT   : std_logic;
  signal rx1 : t_word32_array(0 to 0);
  signal rx4 : t_word32_array(0 to 3);
  signal rx8 : t_word32_array(0 to 7);
  
  -- Signaux exposés par le top
  signal start_tick, end_tick : unsigned(63 downto 0);
  signal N_counted            : unsigned(31 downto 0);
  signal interp_period        : unsigned(63 downto 0);
  signal interp_valid         : std_logic;

  type freq_array is array (0 to 5) of integer;
  constant TEST_FREQS : freq_array := (100_000, 123_456, 150_000,100_001,1_000_000, 10_000_000);

  shared variable f_sig : real := 0.0;
  signal f_direct, f_interp : real := 0.0;

  file result_file : text open write_mode is "resultats.txt";


function slv_to_hex(slv : std_logic_vector) return string is
    constant hex_chars : string := "0123456789ABCDEF";
    variable result    : string(1 to slv'length / 4);
    variable nibble    : std_logic_vector(3 downto 0);
begin
    for i in 0 to (slv'length / 4) - 1 loop
        nibble := slv(slv'left - i*4 downto slv'left - i*4 - 3);
        result(i+1) := hex_chars(to_integer(unsigned(nibble)) + 1);
    end loop;
    return result;
end function;



procedure spi_device_transmit (
    signal cs_n     : out std_logic;
    signal sck      : out std_logic;
    signal mosi     : out std_logic;
    signal miso     : in  std_logic;
    constant tx_word : in std_logic_vector(31 downto 0);
    signal rx_words  : out t_word32_array
) is
begin
    cs_n <= '0';
    wait for 10 * CLK_PER;

    -- Transmission du mot TX (32 bits)
    for i in 31 downto 0 loop
        mosi <= tx_word(i);
        wait for SPI_CLK_PER/2;
        sck <= '1';
        wait for SPI_CLK_PER/2;
        sck <= '0';
    end loop;

    -- Réception de N mots de 32 bits
    for w in rx_words'range loop
        for i in 31 downto 0 loop
            mosi <= '0';
            wait for SPI_CLK_PER/2;
            sck <= '1'; 
            rx_words(w)(i) <= miso;
            wait for SPI_CLK_PER/2;
            sck <= '0';
        end loop;
    end loop;

    wait for 10 * CLK_PER;
    cs_n <= '1';
end procedure;


begin

  ------------------------------------------------------------------------
  -- Horloge de référence 12 MHz
  ------------------------------------------------------------------------
  clk_ref <= not clk_ref after CLK_PER / 2;

  ------------------------------------------------------------------------
  -- Sélection dynamique de la source du signal à mesurer
  ------------------------------------------------------------------------
  sig_in <= NCO_OUT when use_nco_out = '1' else sig_gen_out;

  ------------------------------------------------------------------------
  -- Instance du design principal
  ------------------------------------------------------------------------
  uut: entity work.top_reciproc_freq_meas
    generic map (FREF_HZ => FREF_HZ)
    port map (
      clk_master    => clk_ref,
      reset_n       => rst_n,
      sclk           => sck,
      mosi          => mosi,
      miso          => miso,
      cs_n          => cs_n,
      hf_freq_in    => sig_in,
      start         => start,
      done          => done,
      f_calc        => f_calc,
      LED0          => LED0,
      NCO_OUT       => NCO_OUT,
      start_tick    => start_tick,
      end_tick      => end_tick,
      N_counted     => N_counted,
      interp_period => interp_period,
      interp_valid  => interp_valid
    );

  ------------------------------------------------------------------------
  -- Génération du signal externe
  ------------------------------------------------------------------------
  sig_gen: process
    variable per_sig : time;
  begin
    wait for STABILIZATION_TIME;
    loop
      if f_sig > 0.0 then
        per_sig := 1 sec / f_sig;
        sig_gen_out <= '1';
        wait for per_sig / 2;
        sig_gen_out <= '0';
        wait for per_sig / 2;
      else
        wait for 1 us;
      end if;
    end loop;
  end process;

  ------------------------------------------------------------------------
  -- Stimuli et calculs de fréquences
  ------------------------------------------------------------------------
  stim_proc: process
    variable per_sig : time;
    variable f_est_d, f_est_i, f_est_c : real;
    variable t_start, t_end : time;
    variable dt : real;
    variable l : line;  -- ligne temporaire
  begin
    rst_n <= '0';
    wait for 10 us;
    rst_n <= '1';
    wait for STABILIZATION_TIME;
    
    
    -- led on
    spi_device_transmit(cs_n, sck, mosi, miso, x"02010000", rx1);
    wait for 10 ms;

    -- 100kHz init
    spi_device_transmit(cs_n, sck, mosi, miso, x"040186A0", rx1);
    wait for 10 ms;

    use_nco_out <= '1';
    f_sig := real(100_000);

    report "===== Test signal NCO 100kHz =====" severity note;

    wait for 200 * per_sig;

    start <= '1';
    wait for CLK_PER;
    start <= '0';
    wait until done = '1';

    -- READ_STATUS
    spi_device_transmit(cs_n, sck, mosi, miso,x"01000000", rx1);
    report "READ_STATUS = 0x" & slv_to_hex(rx1(0));

    -- READ_DATA
    spi_device_transmit(cs_n, sck, mosi, miso,x"10000000", rx8);
    for i in rx8'range loop
        report "READ_DATA(" & integer'image(i) & ") = 0x" &
               slv_to_hex(rx8(i));
    end loop;
           
      if end_tick > start_tick then
        dt := real(to_integer(end_tick - start_tick)) / real(FREF_HZ);
      else
        dt := 0.0;
      end if;
      
      if end_tick > start_tick and N_counted /= 0 then
        f_est_d := FREF_HZ * real(to_integer(N_counted)) / real(to_integer(end_tick - start_tick));
      else
        f_est_d := 0.0;
      end if;

      if interp_valid = '1' and interp_period /= 0 then
        f_est_i := FREF_HZ * real(to_integer(N_counted)) / real(to_integer(interp_period));
      else
        f_est_i := 0.0;
      end if;

      f_est_c := real(to_integer(f_calc));

      report
      "------------------------------" severity note;
      report
      "Méthode       | Fréquence (Hz)     | Précision (Hz)" severity note;
      report
      "f_direct      | " & real'image(f_est_d) & " | " & real'image(abs(f_est_d - f_sig)) severity note;
      report
      "Temps mesure  : " & real'image(dt) & " s" severity note;

      write(l, string'("------------------------------"));
      writeline(result_file, l);
      
      write(l, string'("NCO 100kHz"));
      writeline(result_file, l);
      
      write(l, string'("Méthode       | Fréquence (Hz) | Précision (Hz) | t_measure"));
      writeline(result_file, l);
    
      write(l, string'("f_direct      | "));
      write(l, real'image(f_est_d));
      write(l, string'("   | "));
      write(l, real'image(abs(f_est_d - f_sig)));
      write(l, string'("   | "));
      write(l, real'image(dt));
      write(l, string'(" s"));
      writeline(result_file, l);
    
      wait for 200 us;
  

    report "end testbench";
  end process;

end architecture;
