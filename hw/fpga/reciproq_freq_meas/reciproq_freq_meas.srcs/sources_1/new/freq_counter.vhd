-- freq_counter.vhdl
-- Capture N périodes d'un signal et enregistre start/end ticks (référence).
library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;

entity freq_counter is
  generic (
    FREF_HZ     : natural := 12_000_000;  -- documentation only (used to compute timeout)
    REF_CNT_W   : natural := 64;          -- width of reference tick counter
    MAX_N       : natural := 200_000;     -- maximum allowed N
    TIMEOUT_MS  : natural := 2000         -- timeout in ms (safety)
  );
  port (
    clk_ref     : in  std_logic;
    rst_n       : in  std_logic;
    sig_in      : in  std_logic;                            -- async signal to measure (rising edge)
    cfg_N       : in  unsigned(31 downto 0);                -- requested N
    start       : in  std_logic;                            -- pulse to start a measurement
    ready       : out std_logic;                            -- module ready (idle)
    meas_done   : out std_logic;                            -- measurement (capture of N periods) done (pulse)
    status      : out std_logic_vector(3 downto 0);         -- [0]=OK, [1]=TIMEOUT, [2]=OVERFLOW, [3]=BAD_TRIGGER
    start_tick  : out unsigned(REF_CNT_W-1 downto 0);       -- captured ref tick at first edge
    end_tick    : out unsigned(REF_CNT_W-1 downto 0);       -- captured ref tick at N-th edge
    N_counted   : out unsigned(31 downto 0)                 -- number of input edges counted (should equal cfg_N)
  );
end entity;

architecture rtl of freq_counter is
  type state_t is (IDLE, ARMED, MEASURING, COMPLETE, ERROR);
  signal state       : state_t := IDLE;

  -- synchronizer for sig_in
  signal s0, s1      : std_logic := '0';
  signal edge_rise   : std_logic := '0';

  -- reference tick counter
  signal ref_cnt     : unsigned(REF_CNT_W-1 downto 0) := (others => '0');

  -- capture registers
  signal start_tick_r: unsigned(REF_CNT_W-1 downto 0) := (others => '0');
  signal end_tick_r  : unsigned(REF_CNT_W-1 downto 0) := (others => '0');

  -- counters
  signal n_counter   : unsigned(31 downto 0) := (others => '0');
  signal req_N       : unsigned(31 downto 0) := (others => '0');

  -- timeout counter (in reference ticks)
  constant TIMEOUT_TICKS : unsigned(REF_CNT_W-1 downto 0) :=
    resize(to_unsigned(FREF_HZ, REF_CNT_W) *
         to_unsigned(TIMEOUT_MS, REF_CNT_W) /
         to_unsigned(1000, REF_CNT_W),
         REF_CNT_W);
  signal timeout_cnt : unsigned(REF_CNT_W-1 downto 0) := (others => '0');

  -- flags
  signal flag_ok      : std_logic := '0';
  signal flag_timeout : std_logic := '0';
  signal flag_overflow: std_logic := '0';
  signal flag_badtrig : std_logic := '0';

begin

  -- outputs
  ready     <= '1' when state = IDLE else '0';
  meas_done <= '1' when (state = COMPLETE or state = ERROR) else '0';
  status    <= flag_ok & flag_timeout & flag_overflow & flag_badtrig;
  start_tick <= start_tick_r;
  end_tick   <= end_tick_r-2;
  N_counted  <= n_counter-1;

  ----------------------------------------------------------------------------
  -- Synchronize sig_in and detect rising edge (synchronous to clk_ref)
  ----------------------------------------------------------------------------
  process(clk_ref)
  begin
    if rising_edge(clk_ref) then
      if rst_n = '0' then
        s0 <= '0';
        s1 <= '0';
        edge_rise <= '0';
      else
        s0 <= sig_in;
        s1 <= s0;
        edge_rise <= s0 and not s1; -- one-cycle pulse when a rising edge observed
      end if;
    end if;
  end process;

  ----------------------------------------------------------------------------
  -- Reference tick counter (free running)
  ----------------------------------------------------------------------------
  process(clk_ref)
  begin
    if rising_edge(clk_ref) then
      if rst_n = '0' then
        ref_cnt <= (others => '0');
      else
        ref_cnt <= ref_cnt + 1;
      end if;
    end if;
  end process;

  ----------------------------------------------------------------------------
  -- Main state machine: IDLE -> ARMED -> MEASURING -> COMPLETE / ERROR
  ----------------------------------------------------------------------------
  process(clk_ref)
    variable next_n : unsigned(31 downto 0);
  begin
    if rising_edge(clk_ref) then
      if rst_n = '0' then
        state <= IDLE;
        req_N <= (others => '0');
        n_counter <= (others => '0');
        start_tick_r <= (others => '0');
        end_tick_r <= (others => '0');
        timeout_cnt <= (others => '0');
        flag_ok <= '0';
        flag_timeout <= '0';
        flag_overflow <= '0';
        flag_badtrig <= '0';
      else
        -- default: clear transient OK flag (set later on COMPLETE)
        flag_ok <= '0';

        case state is
          when IDLE =>
            -- wait for a start pulse (start must be pulsed)
            if start = '1' then
              -- latch requested N and sanity check
              req_N <= cfg_N;
              if cfg_N = 0 or to_integer(cfg_N) > to_integer(to_unsigned(MAX_N, 32)) then
                flag_badtrig <= '1';
                state <= ERROR;
              else
                -- arm for measurement
                n_counter <= (others => '0');
                timeout_cnt <= (others => '0');
                flag_timeout <= '0';
                flag_overflow <= '0';
                flag_badtrig <= '0';
                state <= ARMED;
              end if;
            end if;

          when ARMED =>
            -- wait for first rising edge of input
            timeout_cnt <= timeout_cnt + 1;
            if edge_rise = '1' then
              start_tick_r <= ref_cnt;
              n_counter <= to_unsigned(1, 32);
              timeout_cnt <= (others => '0');
              state <= MEASURING;
            elsif timeout_cnt > TIMEOUT_TICKS then
              flag_timeout <= '1';
              state <= ERROR;
            end if;

          when MEASURING =>
            -- count edges until requested number
            timeout_cnt <= timeout_cnt + 1;
            if edge_rise = '1' then
              next_n := n_counter + 1;
              n_counter <= next_n;
              if next_n = (req_N+1) then
                end_tick_r <= ref_cnt;
                flag_ok <= '1';
                state <= COMPLETE;
              end if;
            end if;
            if timeout_cnt > TIMEOUT_TICKS then
              flag_timeout <= '1';
              state <= ERROR;
            end if;

          when COMPLETE =>
            -- produce a one-cycle meas_done (done is driven from state)
            -- go back to IDLE ready for next measurement
            state <= ARMED;

          when ERROR =>
            -- remain in ERROR until start is released (simple ack)
            if start = '0' then
              -- clear flags (they are preserved until start low)
              state <= IDLE;
              flag_timeout <= flag_timeout;
              flag_overflow <= flag_overflow;
              flag_badtrig <= flag_badtrig;
            end if;

          when others =>
            state <= IDLE;
        end case;
      end if;
    end if;
  end process;

end architecture;
