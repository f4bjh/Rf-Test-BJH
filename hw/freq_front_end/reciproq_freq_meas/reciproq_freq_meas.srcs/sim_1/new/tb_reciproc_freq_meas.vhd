library ieee; 
use ieee.std_logic_1164.all; 
use ieee.numeric_std.all; 
use std.textio.all;

entity tb_top_reciproc_freq_meas is 
end entity;

architecture sim of tb_top_reciproc_freq_meas is

  type t_word32_array is array (natural range <>) of std_logic_vector(31 downto 0);

  constant FREF_HZ  : integer := 100_000_000;
  constant CLK_PER  : time := 10.000 ns;
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
    variable delta_t_ns : real;
    variable dt : real;
    variable l : line;  -- ligne temporaire
    variable delta_t_var : unsigned(63 downto 0);
    variable N_counted_var : unsigned(31 downto 0);
    variable status : std_logic_vector(31 downto 0);
    variable nb : integer;
    variable cmd1, cmd2 : std_logic_vector(31 downto 0);
    variable f_int_part  : integer;
    variable f_frac_part : integer;
    variable err_abs  : real;
    variable err_rel  : real;

  begin
    rst_n <= '0';
    wait for 10 us;
    rst_n <= '1';
    wait for STABILIZATION_TIME;
    
    
    -- led on
    spi_device_transmit(cs_n, sck, mosi, miso, x"02010000", rx1);
    wait for 10 ms;

    -- 100kHz init
    report "===== Test signal NCO 100kHz =====" severity note;
    spi_device_transmit(cs_n, sck, mosi, miso, x"040186A0", rx1);
    wait for 10 ms;

    use_nco_out <= '1';
    wait until rising_edge(NCO_OUT);
    t_start := now;
    
    for i in 0 to 99 loop
        wait until rising_edge(NCO_OUT);
    end loop;
    
    t_end := now;
    
    if t_end > t_start then
        delta_t_ns := real((t_end - t_start) / 1 ns);
        f_sig := 100.0e9 / delta_t_ns;
    else 
        report "Erreur: delta_t nul !" severity error;
    end if;


    spi_device_transmit(cs_n, sck, mosi, miso, x"03000001", rx1);

    wait for 200 * per_sig;

    spi_device_transmit(cs_n, sck, mosi, miso,x"01000000", rx1);
    report "READ_STATUS = 0x" & slv_to_hex(rx1(0));
    
    spi_device_transmit(cs_n, sck, mosi, miso,x"02040000", rx1);
    report "START MEASURMENT";
    spi_device_transmit(cs_n, sck, mosi, miso,x"01000000", rx1);
    report "READ_STATUS = 0x" & slv_to_hex(rx1(0));

    for nb_meas in 0 to 10 loop

        report "Polling - Measure nr " & integer'image(nb_meas) ;
        
        wait for 250 ms;
        
        spi_device_transmit(cs_n, sck, mosi, miso,x"01000000", rx1);
        report "READ_STATUS = 0x" & slv_to_hex(rx1(0));
        nb := to_integer(unsigned(rx1(0)(15 downto 8)));
        status := rx1(0);

        if nb > 0 or status(FIFO_FULL_BIT) = '1' then

            cmd1 := x"10010000" or std_logic_vector(to_unsigned(nb, 32));
            cmd2 := x"10020000" or std_logic_vector(to_unsigned(nb, 32));

            spi_device_transmit(cs_n, sck, mosi, miso, cmd1, dt16);
            spi_device_transmit(cs_n, sck, mosi, miso, cmd2, N_counted8);

            for i in 0 to nb-1 loop   -- ⚠️ souvent nb mesures → 0 à nb-1
    
                delta_t_var(63 downto 32) := unsigned(dt16(2*i + 1));
                delta_t_var(31 downto 0)  := unsigned(dt16(2*i));
                N_counted_var := unsigned(N_counted8(i));
    
                dt := real(to_integer(delta_t_var(31 downto 0))) +
                      real(to_integer(delta_t_var(63 downto 32))) * 2.0**32;
                dt := dt / real(FREF_HZ);
 
                f_est_d := FREF_HZ *
                           real(to_integer(N_counted_var)) /
                           real(to_integer(delta_t_var));
                f_int_part  := integer(f_est_d);
                f_frac_part := integer((f_est_d - real(f_int_part)) * 1.0e9);                           

                err_abs := abs(f_est_d - f_sig);
                err_rel := 100 * err_abs / f_sig;

                report integer'image(i) & " - " &
                       "delta_t_var=0x" & slv_to_hex(dt16(2*i+1)) & slv_to_hex(dt16(2*i)) &
                       " | N_counted_var=0x" & slv_to_hex(N_counted8(i)) &  
                       " | f_direct=" & integer'image(f_int_part) & "." & integer'image(f_frac_part) &
                       " | dt=" & real'image(dt) &
                       " | N_counted_var=" & integer'image(to_integer(N_counted_var)) severity note;

        
                write(l, string'("------------------------------"));
                writeline(result_file, l);
                write(l, string'("Méthode     | Fréquence (Hz)   | Précision (Hz) | Précision relative % | t_measure"));
                writeline(result_file, l);
                write(l, string'("f_direct    | "));
                write(l, f_int_part);
                write(l, string'("."));
                write(l, f_frac_part);
                write(l, string'(" | "));
                write(l, real'image(err_abs));
                write(l, string'("   | "));
                write(l, real'image(err_rel));
                write(l, string'("         | "));
                write(l, real'image(dt));
                write(l, string'(" s"));
                writeline(result_file, l);
    
    
            end loop;
        end if;

    end loop; 
    
    report "end testbench";
    
  end process;

end architecture;
