-- rom_memory.vhd: IVH projekt
-- Author(s): Karel Jirgl (xjirgl01)
-- Copyright (C) 2022

library ieee;
use ieee.std_logic_1164.all;
use ieee.std_logic_unsigned.all;
use work.newspaper_pack.all;



entity rom_memory is
	generic (
		DATA_BLOCKS	: std_logic_vector(0 to (N_COLUMNS*N_ROWS*N_STATES)-1)
	);
	port (
		BLOCK_ADDR		: in  std_logic_vector(log2(N_STATES)-1 downto 0);
		DATA_OUT		: out COLUMN_ARRAY_T
	);
end entity rom_memory;



architecture behavioral of rom_memory is
	
	signal DATA_OUT_TEMP : std_logic_vector(0 to (N_COLUMNS*N_ROWS)-1);
	
begin

	main : process(BLOCK_ADDR)
	begin

		for i in 0 to (N_COLUMNS*N_ROWS)-1 loop
			
			-- from row interpretation to column interpretation
			--   -----        | | |
			--   -----   ->   | | |
			--   -----        | | |
			
			--report "ROW INDEX: (" & integer'image(i / N_ROWS) & ")" severity note;
			--report "ITEM INDEX: (" & integer'image(i mod N_ROWS) & ")" severity note;
			--report "NEW ITEM INDEX: (" & integer'image(((i mod N_ROWS)*N_COLUMNS)+(i / N_ROWS)) & ")" severity note;
			
			DATA_OUT_TEMP(i) <= DATA_BLOCKS((N_COLUMNS*N_ROWS*conv_integer(BLOCK_ADDR))+(((i mod N_ROWS)*N_COLUMNS)+(i / N_ROWS)));

		end loop;

	end process;
	
	
	
	main2 : process(DATA_OUT_TEMP)
	begin
	
		for i in 0 to (N_COLUMNS)-1 loop

			DATA_OUT(i) <= GETCOLUMN(DATA_OUT_TEMP, i, N_ROWS);
		
		end loop;
	
	end process;
	
end architecture behavioral;
