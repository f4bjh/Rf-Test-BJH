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
  constant SPI_CLK  : integer := 1_000_000;
  constant SPI_CLK_PER : time := 1 us;
  constant STABILIZATION_TIME : time := 100 us;
  constant FIFO_FULL_BIT   : integer := 6;
  
  signal clk_ref   : std_logic := '0';
  signal rst_n     : std_logic := '0';
  signal sig_in    : std_logic := '0';
  signal sig_gen_out : std_logic := '0';
  signal use_nco_out : std_logic := '0';
  signal meas_ready : std_logic;

  signal sck       : std_logic := '0';
  signal mosi      : std_logic := '0';
  signal miso      : std_logic := '0';
  signal cs_n      : std_logic := '1';
  signal LED0      : std_logic;
  signal NCO_OUT   : std_logic;
  signal rx1 : t_word32_array(0 to 0);
  signal rx2 : t_word32_array(0 to 1);
  signal rx4 : t_word32_array(0 to 3);
  signal rx8 : t_word32_array(0 to 7);
  signal dt16 : t_word32_array(0 to 15);
  signal N_counted8 : t_word32_array(0 to 7);
  
  signal delta_t : unsigned(63 downto 0);
  signal N_counted            : unsigned(31 downto 0);
 
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
      LED0          => LED0,
      NCO_OUT       => NCO_OUT
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
    variable delta_t_var : unsigned(63 downto 0);
    variable N_counted_var : unsigned(31 downto 0);
    variable status : std_logic_vector(31 downto 0);
    
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
    f_sig := real(100_001);

    report "===== Test signal NCO 100kHz =====" severity note;

    wait for 200 * per_sig;

    spi_device_transmit(cs_n, sck, mosi, miso,x"01000000", rx1);
    report "READ_STATUS = 0x" & slv_to_hex(rx1(0));
    
    spi_device_transmit(cs_n, sck, mosi, miso,x"02040000", rx1);
    report "START MEASURMENT";
    spi_device_transmit(cs_n, sck, mosi, miso,x"01000000", rx1);
    report "READ_STATUS = 0x" & slv_to_hex(rx1(0));



    for nb_meas in 0 to 5 loop
        report "Measure nr " & integer'image(nb_meas) ;
        -- polling FIFO FULL
        for i in 0 to 1000 loop
            spi_device_transmit(cs_n, sck, mosi, miso, x"01000000", rx1);
    
            status := rx1(0);
            exit when status(FIFO_FULL_BIT) = '1';
    
            wait for 1 ms;
        end loop;
        spi_device_transmit(cs_n, sck, mosi, miso,x"01000000", rx1);
        report "READ_STATUS = 0x" & slv_to_hex(rx1(0));
        if status(FIFO_FULL_BIT) = '0' then
            report "Timeout FIFO FULL" severity failure;
            wait;  -- bloque le process
        end if;
        
        spi_device_transmit(cs_n, sck, mosi, miso,x"10010008", dt16);
        spi_device_transmit(cs_n, sck, mosi, miso,x"10020008", N_counted8);
        for i in 0 to 7 loop
            report
            "------------------------------" severity note;
            report "dt(" & integer'image(i) & ") = 0x" & slv_to_hex(dt16(2*i+1)) & slv_to_hex(dt16(2*i));
            report "N_counted(" & integer'image(i) & ") = 0x" & slv_to_hex(N_counted8(i));
            
            delta_t_var(63 downto 32) := unsigned(dt16(2*i + 1));
            delta_t_var(31 downto 0)  := unsigned(dt16(2*i));
            N_counted_var := unsigned(N_counted8(i));
        
            dt := real(to_integer(delta_t_var(31 downto 0))) + real(to_integer(delta_t_var(63 downto 32))) * 2.0**32;
            dt := dt / real(FREF_HZ);      
            report "dt=" & real'image(dt) & " N_counted_var=" & integer'image(to_integer(N_counted_var));
            f_est_d := FREF_HZ * real(to_integer(N_counted_var)) / real(to_integer(delta_t_var));
        
     
    
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
      
        end loop;
    end loop;    
    
    
    report "end testbench";
  end process;

end architecture;
