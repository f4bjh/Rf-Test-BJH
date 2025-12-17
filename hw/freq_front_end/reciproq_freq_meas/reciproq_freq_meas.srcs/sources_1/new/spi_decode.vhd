library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;

entity spi_slave is
    port (
        clk         : in  std_logic;
        reset_n     : in  std_logic;

        -- SPI
        sclk        : in  std_logic;
        cs_n        : in  std_logic;
        mosi        : in  std_logic;
        miso        : out std_logic;

        -- LED
        led_on          : out std_logic;
        led_toggle      : out std_logic;
        led_off         : out std_logic;
 
        -- NCO
        nco_freq_word  : out unsigned(23 downto 0);
        nco_freq_valid : out std_logic
    );
end entity;

architecture rtl of spi_slave  is

    signal sclk_d      : std_logic:='0';
    signal sclk_rise   : std_logic:='0';

    signal bit_cnt     : integer range 0 to 7 := 0;
    signal byte_cnt    : integer range 0 to 3 := 0;

    signal rx_shift    : std_logic_vector(31 downto 0):= (others => '0');
    signal tx_shift    : std_logic_vector(31 downto 0):= (others => '0');
    signal tx_load     : std_logic:='0';
    
    signal rx_word     : std_logic_vector(31 downto 0):= (others => '0');
    signal data_valid  : std_logic:='0';
    signal data_out     : std_logic_vector(31 downto 0) := (others => '0');

    signal opcode      : unsigned(7 downto 0):= (others => '0');
    signal subcmd      : unsigned(7 downto 0):= (others => '0');
    signal payload     : unsigned(15 downto 0):= (others => '0');

    signal status_reg  : std_logic_vector(31 downto 0):= (others => '0');
    signal error_flag  : std_logic:='0';
    signal busy_flag   : std_logic :='0';
      
    
    
begin
    
    process(clk)
    begin
        if rising_edge(clk) then
            sclk_d <= sclk;
        end if;
    end process;
    
    sclk_rise <= sclk and not sclk_d;
    
    process(clk)
    begin
        if rising_edge(clk) then

            if cs_n = '1' then
                bit_cnt    <= 0;
                byte_cnt   <= 0;
                data_valid <= '0';

            elsif sclk_rise = '1' then

                rx_shift <= rx_shift(30 downto 0) & mosi;
                bit_cnt <= bit_cnt + 1;

                if bit_cnt = 7 then
                    bit_cnt <= 0;
                    byte_cnt <= byte_cnt + 1;

                    if byte_cnt = 3 then
                        rx_word    <= rx_shift(30 downto 0) & mosi;
                        data_valid <= '1';
                        byte_cnt   <= 0;
                    end if;
                end if;
            end if;
        end if;
    end process;

    process(clk, reset_n)
    begin
        if reset_n = '0' then

            led_on        <= '0';
            led_off       <= '0';
            led_toggle    <= '0';

            nco_freq_word   <= (others => '0');
            nco_freq_valid  <= '0';
            error_flag      <= '0';

        elsif rising_edge(clk) then

            nco_freq_valid <= '0';

            if data_valid = '1' then

                opcode  <= unsigned(rx_word(31 downto 24));
                subcmd  <= unsigned(rx_word(23 downto 16));
                payload <= unsigned(rx_word(15 downto 0));

                case rx_word(31 downto 24) is

                    -- READ_STATUS
                    when x"01" =>
                        data_out<=status_reg;
               

                    -- LED                    
                    when x"02" =>
                        case rx_word(23 downto 16) is
                            when x"01" => led_on     <= '1';
                            when x"02" => led_off    <= '1';
                            when x"03" => led_toggle <= '1';
                            when others => error_flag <= '1';
                        end case;

                    -- SYSTEM
                    when x"03" =>
                        null;


                    -- NCO
                    when x"04" =>
                        nco_freq_word  <= unsigned(rx_word(23 downto 0));
                        nco_freq_valid <= '1';
                   


                    when others =>
                        error_flag <= '1';
                        data_out <= (others => '0');

                end case;
            end if;
        end if;
    end process;


    process(clk, reset_n)
    begin
        if reset_n = '0' then
            status_reg <= (others => '0');
        elsif rising_edge(clk) then
            status_reg(31 downto 24) <= x"01"; -- VERSION
            status_reg(7) <= error_flag;
            status_reg(6) <= busy_flag;
            status_reg(5) <= '0';              -- IRQ (futur)
            status_reg(4) <= nco_freq_valid;
            status_reg(3) <= led_on;
            status_reg(2) <= led_off;
            status_reg(1) <= led_toggle;
            status_reg(0) <= '0';
        end if;
    end process;


    process(clk)
    begin
        if rising_edge(clk) then
    
            -- Fin de transaction SPI
            if cs_n = '1' then
                tx_load <= '1';  -- autorise un nouveau chargement
    
            -- Front montant SCLK (mode 0)
            elsif sclk_rise = '1' then
    
                -- PREMIER BIT : on charge le mot à transmettre
                if tx_load = '1' then
                    tx_shift <= data_out;  -- <-- CHARGEMENT
                    tx_load  <= '0';
    
                -- BITS SUIVANTS : on décale
                else
                    tx_shift <= tx_shift(30 downto 0) & '0';  -- <-- DÉCALAGE
                end if;
    
            end if;
        end if;
    end process;
    
    miso <= tx_shift(31);

end architecture;