library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;

entity sobel_alu is
    port (
      signal dataa: in std_logic_vector(31 downto 0);
      signal datab: in std_logic_vector(31 downto 0);
      signal result: out std_logic_vector(31 downto 0)

    );
end sobel_alu;

architecture rtl of sobel_alu is
    signal p00, p01, p02 : unsigned(7 downto 0);
    signal p10, p12 : unsigned(7 downto 0);
    signal p20, p21, p22 : unsigned(7 downto 0);

    signal gx  : integer;
    signal gy  : integer;
    signal sum : integer;

    begin
      p00 <= unsigned(dataa( 7 downto  0));
      p01 <= unsigned(dataa(15 downto  8));
      p02 <= unsigned(dataa(23 downto 16));
      p10 <= unsigned(dataa(31 downto 24));
      
      p12 <= unsigned(datab(7 downto  0));
      p20 <= unsigned(datab(15 downto  8));
      p21 <= unsigned(datab(23 downto 16));
      p22 <= unsigned(datab(31 downto 24));

      gx <=  -to_integer(p00)
            + to_integer(p02)
            - (to_integer(p10) * 2)
            + (to_integer(p12) * 2)
            - to_integer(p20)
            + to_integer(p22);

      gy <=   to_integer(p00)
            + (to_integer(p01) * 2)
            + to_integer(p02)
            - to_integer(p20)
            - (to_integer(p21) * 2)
            - to_integer(p22);

    sum <= (abs(gx) + abs(gy));

    result <= x"000000FF" when sum > 128 else x"00000000";

end rtl;
