library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;

entity nco is
    port(
        clk_in      : in  std_logic;
        reset_n     : in  std_logic;
        freq_word   : in  unsigned(23 downto 0); -- 24 bits venant du SPI
        freq_valid  : in  std_logic;             -- met à jour PHASE_INC
        clk_out     : out std_logic
    );
end entity;

architecture rtl of nco is
    constant PHASE_BITS : integer := 32;
    constant CLK_MASTER  : integer := 100_000_000;  -- fréquence en Hz

    signal phase_acc : unsigned(PHASE_BITS-1 downto 0) := (others => '0');
    signal PHASE_INC : unsigned(PHASE_BITS-1 downto 0) := (others => '0');
    signal out_reg   : std_logic := '0';
begin
    process(clk_in, reset_n)
        variable tmp : unsigned(55 downto 0); -- 24 bits * 32 bits -> 56 bits
    begin
        if reset_n = '0' then
            phase_acc <= (others => '0');
            PHASE_INC <= (others => '0');
            out_reg   <= '0';
        elsif rising_edge(clk_in) then
            -- mise à jour dynamique de PHASE_INC
            if freq_valid = '1' then
                tmp := resize(freq_word, 56) sll 32;       -- freq_word * 2^32
                tmp := tmp / CLK_MASTER;                   -- division entière
                PHASE_INC <= resize(tmp, PHASE_BITS);      -- tronquer à 32 bits
            end if;

            -- accumulateur de phase
            phase_acc <= phase_acc + PHASE_INC;

            -- sortie MSB
            out_reg <= phase_acc(PHASE_BITS-1);
        end if;
    end process;

    clk_out <= out_reg;
end architecture;
