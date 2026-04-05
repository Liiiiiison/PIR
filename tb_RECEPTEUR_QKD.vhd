library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.numeric_std.all;

entity tb_RECEPTEUR_QKD is
-- Vide
end entity;

architecture sim of tb_RECEPTEUR_QKD is

    -- Constantes de temps
    constant CLK_PERIOD : time := 10 ns;

    -- Signaux d'interface
    signal clk              : std_logic := '0';
    signal detector_pulse_0 : std_logic := '0';
    signal detector_pulse_1 : std_logic := '0';
    signal basis_modulator  : std_logic;
    signal filter_ready     : std_logic := '0';
    signal key_bit          : std_logic;
    signal key_valid        : std_logic; -- Nouveau signal ajouté
    signal basis_used       : std_logic := '0';

begin

    -- Instanciation de l'Unité Sous Test (UUT)
    UUT: entity work.RECEPTEUR_QKD
        port map (
            Clk              => clk,
            Detector_pulse_0 => detector_pulse_0,
            Detector_pulse_1 => detector_pulse_1,
            Basis_modulator  => basis_modulator,
            Filter_ready     => filter_ready,
            Key_bit          => key_bit,
            Key_valid        => key_valid, -- Connecté ici
            Basis_used       => basis_used
        );

    -- Génération de l'horloge
    clk_process : process
    begin
        while now < 1000 ns loop -- Un peu plus de temps pour voir les fronts
            clk <= '0';
            wait for CLK_PERIOD / 2;
            clk <= '1';
            wait for CLK_PERIOD / 2;
        end loop;
        wait;
    end process;

    -- Processus de stimulation
    stim_proc: process
    begin
        -- Initialisation
        detector_pulse_0 <= '0';
        detector_pulse_1 <= '0';
        filter_ready     <= '0';
        wait for 20 ns;

        -- SCÉNARIO 1 : Réception d'un Bit '0' en base 0
        -- On simule l'arrivée d'un photon sur le détecteur 0 (Horizontal)
        basis_used       <= '0'; 
        filter_ready     <= '1';
        wait for CLK_PERIOD; -- Temps pour que le modulateur se stabilise
        
        detector_pulse_0 <= '1';
        wait for CLK_PERIOD;
        detector_pulse_0 <= '0';
        
        wait for 30 ns;

        -- SCÉNARIO 2 : Détecteur actif mais Filtre non prêt (Phase de transition)
        -- Le signal key_valid doit rester à '0'
        filter_ready     <= '0';
        detector_pulse_1 <= '1';
        wait for CLK_PERIOD;
        detector_pulse_1 <= '0';

        wait for 30 ns;

        -- SCÉNARIO 3 : Réception d'un Bit '1' en base 1 (Base Diagonale)
        -- On change la base, et on reçoit un clic sur le détecteur 1
        basis_used       <= '1';
        filter_ready     <= '1';
        wait for CLK_PERIOD;
        
        detector_pulse_1 <= '1';
        wait for CLK_PERIOD;
        detector_pulse_1 <= '0';

        wait for 30 ns;
        
        -- SCÉNARIO 4 : Cas d'erreur (Double clic / Bruit)
        -- Si les deux détecteurs cliquent en même temps, key_valid doit être '0'
        detector_pulse_0 <= '1';
        detector_pulse_1 <= '1';
        wait for CLK_PERIOD;
        detector_pulse_0 <= '0';
        detector_pulse_1 <= '0';

        wait for 100 ns;
        
        wait;
    end process;

end architecture;