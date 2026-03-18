library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
use IEEE.NUMERIC_STD.ALL;

entity AtomicBitExtractor is
    Generic (
        TAILLE_CLE : integer := 32
    );
    Port (
        sequence  : in  std_logic_vector(TAILLE_CLE-1 downto 0);
        mask      : in  std_logic_vector(TAILLE_CLE-1 downto 0);
        reconcilied_key : out std_logic_vector(TAILLE_CLE-1 downto 0)
    );
end AtomicBitExtractor;

architecture beh of AtomicBitExtractor is

    type t_wire_matrix is array (0 to TAILLE_CLE) of std_logic_vector(TAILLE_CLE-1 downto 0);
    signal intermediate_data : t_wire_matrix;
    
    type t_count_array is array (0 to TAILLE_CLE) of integer range 0 to TAILLE_CLE;
    signal bit_counts : t_count_array;


    signal zero_base is std_logic_vector(TAILLE_CLE-1 downto 0)

begin

    -- Initial state (Zero bits extracted, zero count)
    intermediate_data(0) <= (others => '0');
    bit_counts(0)        <= 0;

    -- this unrolls into a chain of multiplexers and adders
    gen_extractor: for i in 0 to TAILLE_CLE-1 generate
    begin

        process(mask, sequence, intermediate_data, bit_counts)
        begin
            if mask(i) = '1' then

                -- isolate the targeted bit of the sequence and place it in the right position
                -- based on the previous state of the computation
                -- padding operator : "&"
                intermediate_data(i+1) <= intermediate_data(i) or ( sequence(i) & zero_base(0 to i));

                bit_counts(i+1) <= bit_counts(i) + 1;
            else
                -- non matching bit, do not do anything
                -- pass the same state to next iteration
                intermediate_data(i+1) <= intermediate_data(i);
                bit_counts(i+1) <= bit_counts(i);
            end if;
        end process;
    end generate;

    
    -- last state is reconcilied key
    reconcilied_key <= intermediate_data(TAILLE_CLE);

end beh;