library ieee; 
use ieee.std_logic_1164.all; 
use ieee.numeric_std.all; 
use std.textio.all;

entity tb_top_reciproc_freq_meas is 
end entity;


architecture sim of tb_top_reciproc_freq_meas is
  constant FREF_HZ  : integer := 12_000_000;
  constant CLK_PER  : time := 83.333 ns;
  constant STABILIZATION_TIME : time := 100 us;

  signal clk_ref   : std_logic := '0';
  signal rst_n     : std_logic := '0';
  signal sig_in    : std_logic := '0';
  signal start     : std_logic := '0';
  signal ready     : std_logic;
  signal done      : std_logic;
  signal f_calc    : unsigned(31 downto 0);

  signal sck       : std_logic := '0';
  signal mosi      : std_logic := '0';
  signal cs_n      : std_logic := '1';
  signal LED0      : std_logic;
  signal NCO_OUT   : std_logic;

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
  
begin

  ------------------------------------------------------------------------
  -- Horloge de référence 12 MHz
  ------------------------------------------------------------------------
  clk_ref <= not clk_ref after CLK_PER / 2;

  ------------------------------------------------------------------------
  -- Instance du design principal
  ------------------------------------------------------------------------
  uut: entity work.top_reciproc_freq_meas
    generic map (FREF_HZ => FREF_HZ)
    port map (
      clk_master    => clk_ref,
      reset_n       => rst_n,
      sck           => sck,
      mosi          => mosi,
      cs_n          => cs_n,
      hf_freq_in    => sig_in,
      start         => start,
      ready         => ready,
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
  -- Génération du signal à mesurer
  ------------------------------------------------------------------------
  sig_gen: process
    variable per_sig : time;
  begin
    wait for STABILIZATION_TIME;
    loop
      if f_sig > 0.0 then
        per_sig := 1 sec / f_sig;
        sig_in <= '1';
        wait for per_sig / 2;
        sig_in <= '0';
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

    for k in 0 to 5 loop
      f_sig := real(TEST_FREQS(k));
      per_sig := 1 sec / f_sig;

      report "===== Test signal " & integer'image(TEST_FREQS(k)) & " Hz =====" severity note;

      wait for 200 * per_sig;

      -- Début de la mesure
      start <= '1';
      wait for CLK_PER;
      start <= '0';

      wait until done = '1';
      
      -- Temps écoulé en secondes à partir des ticks
      if end_tick > start_tick then
        dt := real(to_integer(end_tick - start_tick)) / real(FREF_HZ);
      else
        dt := 0.0;
      end if;
      
      -- Calcul des fréquences à partir du freq_counter / frac_interp
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

      -- Affichage regroupé
      report
      "------------------------------" severity note;
      report
      "Méthode       | Fréquence (Hz)     | Précision (Hz)" severity note;
      report
      "f_direct      | " & real'image(f_est_d) & " | " & real'image(abs(f_est_d - f_sig)) severity note;
      --report
      --"f_interp      | " & real'image(f_est_i) & " | " & real'image(abs(f_est_i - f_sig)) severity note;
      --report
      --"f_calc        | " & real'image(f_est_c) & " | " & real'image(abs(f_est_c - f_sig)) severity note;
      report
      "Temps mesure  : " & real'image(dt) & " s" severity note;
      --report
      --"------------------------------" severity note;
      -- exemple d'affichage regroupé
      write(l, string'("------------------------------"));
      writeline(result_file, l);
    
      write(l, string'("Méthode       | Fréquence (Hz) | Précision (Hz) | t_measure"));
      writeline(result_file, l);
    
      -- mesurer f_direct
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

    report "end testbench";
  end process;

end architecture;
