library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.numeric_std.all;

entity tb_RECEPTEUR_QKD_SERIES_STAT is
-- Vide
end entity;

architecture sim of tb_RECEPTEUR_QKD_SERIES_STAT is

    constant CLK_PERIOD : time := 10 ns;

    signal clk              : std_logic := '0';
    signal detector_pulse_0 : std_logic := '0';
    signal detector_pulse_1 : std_logic := '0';
    signal basis_modulator  : std_logic;
    signal filter_ready     : std_logic := '0';
    signal key_bit          : std_logic;
    signal key_valid        : std_logic;
    signal basis_used       : std_logic := '0';

begin

    UUT: entity work.RECEPTEUR_QKD
        port map (
            Clk              => clk,
            Detector_pulse_0 => detector_pulse_0,
            Detector_pulse_1 => detector_pulse_1,
            Basis_modulator  => basis_modulator,
            Filter_ready     => filter_ready,
            Key_bit          => key_bit,
            Key_valid        => key_valid,
            Basis_used       => basis_used
        );

    -- Horloge
    clk <= not clk after CLK_PERIOD / 2;

    stim_proc: process
    begin
        -- Initialisation
        filter_ready <= '1';
        wait for 25 ns;

        -----------------------------------------------------------
        -- SCÉNARIO 1 : 100% de '0' en Base 0 (Rectiligne)
        -----------------------------------------------------------
--        report "Test: 100% de 0 en Base rectiligne";
--        basis_used <= '0';
--        for i in 1 to 10 loop
--            detector_pulse_0 <= '1';
--            wait for CLK_PERIOD;
--            detector_pulse_0 <= '0';
--            wait for CLK_PERIOD; -- Un cycle vide entre les photons
--        end loop;

        -----------------------------------------------------------
        -- SCÉNARIO 2 : 100% de '0' en Base 1 (Diagonale)
        -----------------------------------------------------------
--        report "Test: 100% de 0 en Base diagonale";
--        basis_used <= '1';
--        for i in 1 to 10 loop
--            detector_pulse_0 <= '1'; -- Rappel : Le Det 0 capte le '0' de la base active
--            wait for CLK_PERIOD;
--            detector_pulse_0 <= '0';
--            wait for CLK_PERIOD;
--        end loop;

        -----------------------------------------------------------
        -- SCÉNARIO 3 : 50% de '0' et 50% de '1' (Alternance)
        -----------------------------------------------------------
--        report "Test: 50% de 0 / 50% de 1";
--        for i in 1 to 5 loop
--            detector_pulse_0 <= '1'; wait for CLK_PERIOD; detector_pulse_0 <= '0';

--            wait for CLK_PERIOD; -- wait one clock period to let filter change possible

--            detector_pulse_1 <= '1'; wait for CLK_PERIOD; detector_pulse_1 <= '0';
--            wait for CLK_PERIOD;
--        end loop;
        
        
        -----------------------------------------------------------
        -- SCÉNARIO 3 BIS : 50% de '0' et 50% de '1' NO WAIT
        -----------------------------------------------------------
--        report "Test: 50% de 0 / 50% de 1";
--        for i in 1 to 5 loop
--            detector_pulse_0 <= '1';
--            wait for CLK_PERIOD;
--            detector_pulse_0 <= '0';
--            detector_pulse_1 <= '1';
--            wait for CLK_PERIOD;
--            detector_pulse_1 <= '0';
--        end loop;


        -----------------------------------------------------------
        -- SCÉNARIO 4 : 30% de photons reçus (70% de silence)
        -----------------------------------------------------------
--        report "Test: 30% de taux de réception ( base rectiligne )";
--        for i in 1 to 10 loop
--            if i <= 3 then
--                detector_pulse_0 <= '1';
--            else
--                detector_pulse_0 <= '0';
--            end if;
--            wait for CLK_PERIOD;
--            detector_pulse_0 <= '0';
--            wait for CLK_PERIOD * 2;
--        end loop;

        -----------------------------------------------------------
        -- SCÉNARIO 5 : 100% de '1'
        -----------------------------------------------------------
--        report "Test: 100% de 1 sur les deux bases";
--        report "Base rectiligne";
--        basis_used <= '0';
--        wait for 20 ns;
--        for i in 1 to 5 loop
--            detector_pulse_1 <= '1'; wait for CLK_PERIOD; detector_pulse_1 <= '0';
--            wait for CLK_PERIOD;
--        end loop;
        
--        report "Base diagonale";
--        basis_used <= '1';
--        wait for 20 ns;
--        for i in 1 to 5 loop
--            detector_pulse_1 <= '1'; wait for CLK_PERIOD; detector_pulse_1 <= '0';
--            wait for CLK_PERIOD;
--        end loop;

        -- wait for 100 ns;
        assert false report "Tous les tests statistiques sont terminés" severity note;
        wait;
    end process;

end architecture;