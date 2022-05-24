-- counter.vhd: IVH projekt
-- Author(s): Karel Jirgl (xjirgl01)
-- Copyright (C) 2022

library IEEE;
use ieee.std_logic_1164.all;
use ieee.std_logic_arith.all;
use ieee.std_logic_unsigned.all;



entity counter is
	Generic (
		CLK_PERIOD : integer := 10; -- ns
		OUT_PERIOD : integer := 1000 -- ns
	);
	Port (
		CLK : in  STD_LOGIC;
		RESET : in  STD_LOGIC;
		EN : out  STD_LOGIC
	);
end counter;



architecture behavioral of counter is

-- zde je funkce log2 z prednasek, pravdepodobne se vam bude hodit.
	function log2(A: integer) return integer is
		variable bits : integer := 0;
		variable b : integer := 1;
	begin
		while (b <= a) loop
			b := b * 2;
			bits := bits + 1;
		end loop;
		return bits;
	end function;

	constant PERIODS : integer := OUT_PERIOD/CLK_PERIOD; 

	signal cnt : std_logic_vector(log2(PERIODS)-1 downto 0) := (others => '0');

begin
-- citac bude mit 2 genericke parametry: periodu hodinoveho signalu (CLK_PERIOD) a vystupni
-- periodu (OUT_PERIOD) (obe dve zadane jako cas). Citac s periodou odpovidajici OUT_PERIOD
-- (t.j., napr za 1 ms) aktivuje na jeden hodinovy cyklus signal EN po dobu jednoho taktu CLK
-- reset je aktivni v 1: tj kdyz je reset = 1, tak se vymaze vnitrni citac
-- pro zjednoduseni pocitejte, ze OUT_PERIOD je delitelne CLK_PERIOD beze zbytku a je vetsi.

-- Signal EN bude aktivovany po 1 periodu hodinoveho vstupu CLK - k cemu vam staci pouze
-- signal CLK a jeho priznak CLK'event. Genericke parametry OUT_PERIOD a CLK_FPERIOD slouzi pouze
-- k vypoctu toho,  do kolika citac pocita.


	-- synchronni citac s asynchronnim nulovanim
	process(RESET, CLK)
	begin
		if (RESET = '1') then
			cnt <= (others => '0');
		elsif (CLK'event and CLK = '1') then
			cnt <= cnt + 1;

			if (cnt = PERIODS) then
				cnt <= (0 => '1', others => '0');
			end if;
		end if;
	end process;

	EN <= '1' when (cnt = 1) else '0';

end behavioral;
