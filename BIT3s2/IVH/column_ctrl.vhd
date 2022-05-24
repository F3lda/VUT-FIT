-- column_ctrl.vhd: IVH projekt
-- Author(s): Karel Jirgl (xjirgl01)
-- Copyright (C) 2022

library ieee;
use ieee.std_logic_1164.all;
use ieee.std_logic_unsigned.all;
use work.newspaper_pack.all;



entity column_ctrl is
	generic (
		N	: integer
	);
	port (
		CLK				: in  std_logic;
		RESET			: in  std_logic;
		
		STATE			: out std_logic_vector(0 to N-1);
		INIT_STATE		: in std_logic_vector(0 to N-1);
		NEIGH_LEFT		: in std_logic_vector(0 to N-1);
		NEIGH_RIGHT		: in std_logic_vector(0 to N-1);
		
		DIRECTION		: in DIRECTION_T;
		EN				: in std_logic
	);
end entity column_ctrl;



architecture behavioral of column_ctrl is

begin

	main : process (clk) is
		variable LAST_INIT_STATE : std_logic_vector(0 to N-1) := (others => '0');
	begin
	
		if RESET = '1' then
			STATE <= INIT_STATE;
		elsif rising_edge(clk) then
			if (LAST_INIT_STATE /= INIT_STATE) then -- on INIT_STATE changed -> change STATE
			
				LAST_INIT_STATE := INIT_STATE;
				
				STATE <= INIT_STATE;
				
			elsif (EN = '1') then -- change rotation direction
			
				if (DIRECTION = DIR_LEFT) then
					STATE <= NEIGH_RIGHT;
				elsif (DIRECTION = DIR_RIGHT) then
					STATE <= NEIGH_LEFT;
				end if;
			
			end if;
		end if;
	
	end process;
	
end architecture behavioral;
