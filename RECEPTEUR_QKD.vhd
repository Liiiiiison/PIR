--Juste avant le cube PBS, on place un modulateur de phase/polarisation (souvent un cristal de Niobate de Lithium).

--Si vous voulez mesurer dans la base Rectiligne (+) :
--Le module de commande n'applique aucune tension au cristal. Le photon passe tel quel. S'il était à 0°, il va au Détecteur 0. S'il était à 90°, il va au Détecteur 1.

--Si vous voulez mesurer dans la base Diagonale (×) :
--Le module de commande applique une tension spécifique au cristal. Cela a pour effet de faire pivoter la polarisation du photon de 45° optiquement (sans rien bouger).

--Un photon qui était "Diagonal" (45°) devient "Horizontal" (0°) après le cristal et finit donc dans le Détecteur 0.

--Un photon qui était "Anti-diagonal" (135°) devient "Vertical" (90°) et finit dans le Détecteur 1.


--en optique quantique, un détecteur de photon unique est "aveugle" à la polarisation. Il fait "Clic !" quand un photon arrive, peu importe son angle.

--Pour connaître la polarisation d'un photon, on utilise un Séparateur de Faisceau Polarisant (PBS) ( le cube PBS). Le PBS agit comme un aiguillage :

--Les photons polarisés horizontalement (H) vont tout droit → vers le Détecteur 0.

--Les photons polarisés verticalement (V) sont déviés → vers le Détecteur 1.



-- POUR GENERER UN VCD
--restart
--open_vcd test_vcd.vcd
--log_vcd [get_objects -recursive /tb_RECEPTEUR_QKD_SERIES_STAT/UUT/*]
--run 1000 ns
--close_vcd


library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.numeric_std.all; 

entity RECEPTEUR_QKD is
    Port (
        Clk              : in  std_logic;
        -- Signaux provenant des deux détecteurs
        Detector_pulse_0 : in  std_logic; -- Clic = Bit '0' reçu
        Detector_pulse_1 : in  std_logic; -- Clic = Bit '1' reçu
        
        Filter_ready     : in  std_logic;
        Basis_used       : in  std_logic; -- La base qu'on a décidé de mesurer
        
        Basis_modulator  : out std_logic;
        
        -- Sorties vers la mémoire/traitement
        Key_valid        : out std_logic; -- Indique si un photon a été lu ce cycle
        Key_bit          : out std_logic
    );
end entity;

architecture beh of RECEPTEUR_QKD is
begin
    
    -- On pilote le modulateur optique en temps réel
    Basis_modulator <= Basis_used; 

    Detection_Logic: process(Clk)
    begin
        if rising_edge(Clk) then
            -- Par défaut, pas de clé générée à ce cycle d'horloge
            Key_valid <= '0'; 
            Key_bit   <= '0';

            if Filter_ready = '1' then
                
                -- Si le détecteur 0 clique, c'est un bit 0 dans la base choisie
                if Detector_pulse_0 = '1' and Detector_pulse_1 = '0' then
                    Key_bit   <= '0';
                    Key_valid <= '1';
                
                -- Si le détecteur 1 clique, c'est un bit 1 dans la base choisie
                elsif Detector_pulse_1 = '1' and Detector_pulse_0 = '0' then
                    Key_bit   <= '1';
                    Key_valid <= '1';
                
                -- Cas d'erreur (coïncidence impossible d'un même photon aux deux endroits)
                elsif Detector_pulse_0 = '1' and Detector_pulse_1 = '1' then
                    Key_valid <= '0'; -- On rejette l'événement
                end if;

            end if;
        end if;
    end process;

end architecture;