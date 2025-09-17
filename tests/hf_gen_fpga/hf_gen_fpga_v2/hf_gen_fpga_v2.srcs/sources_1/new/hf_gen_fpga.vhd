library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;

entity oscillator2 is
    port(
        clk_in           : in  std_logic;
        reset_n          : in  std_logic;
        decr_push_button : in  std_logic;
        clk_out          : out std_logic
    );
end entity oscillator2;

architecture rtl of oscillator2 is
    constant MAX : natural := 31; -- valeur maximale de DIV

    signal DIV        : natural range 1 to MAX := MAX; -- valeur courante du diviseur
    signal counter    : natural := 0;
    signal clk_reg    : std_logic := '0';

    signal high_count : natural := MAX/2;
    signal low_count  : natural := MAX - (MAX/2);

    -- Anti-rebond bouton (compteur de stabilité)
    constant DEBOUNCE_MAX : natural := 50000; -- nombre de cycles à attendre (à ajuster selon clk_in)
    signal debounce_cnt   : natural := 0;
    signal button_sync    : std_logic := '0';
    signal button_stable  : std_logic := '0';
    signal button_last    : std_logic := '0';

begin

    -- Synchronisation + anti-rebond du bouton
    process(clk_in, reset_n)
    begin
        if reset_n = '0' then
            button_sync   <= '0';
            button_stable <= '0';
            debounce_cnt  <= 0;
        elsif rising_edge(clk_in) then
            -- synchronisation
            button_sync <= decr_push_button;

            -- anti-rebond : si état stable pendant DEBOUNCE_MAX cycles, on valide
            if button_sync = button_stable then
                debounce_cnt <= 0;
            else
                if debounce_cnt = DEBOUNCE_MAX then
                    button_stable <= button_sync;
                    debounce_cnt  <= 0;
                else
                    debounce_cnt <= debounce_cnt + 1;
                end if;
            end if;
        end if;
    end process;

    -- Gestion du diviseur
    process(clk_in, reset_n)
    begin
        if reset_n = '0' then
            DIV        <= MAX;
            counter    <= 0;
            high_count <= MAX/2;
            low_count  <= MAX - (MAX/2);
            button_last <= '0';

        elsif rising_edge(clk_in) then
            -- Détection front montant (anti-rebondé)
            if (button_last = '0' and button_stable = '1') then
                if DIV = 1 then
                    DIV <= MAX;
                else
                    DIV <= DIV - 1;
                end if;
                -- recalcul high/low
                high_count <= DIV/2;
                low_count  <= DIV - (DIV/2);
                counter    <= 0;
            end if;
            button_last <= button_stable;
         end if;
    end process;
    
    --gestion de la clock de sortie
    process(clk_in, reset_n, counter)
    begin
        if reset_n = '0' then
            clk_reg    <= '0';
        else
            -- Division avec duty-cycle pair/impair
            counter <= counter + 1;
            if (clk_reg = '0' and counter = high_count-1) then
                counter <= 0;
                clk_reg <= '1';
            elsif (clk_reg = '1' and counter = low_count-1) then
                counter <= 0;
                clk_reg <= '0';
            end if;
        end if;
    end process;

    clk_out <= clk_reg;

end architecture rtl;
