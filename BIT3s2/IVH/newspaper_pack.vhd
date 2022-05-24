-- newspaper_pack.vhd: IVH projekt
-- Author(s): Karel Jirgl (xjirgl01)
-- Copyright (C) 2022

library ieee;
use ieee.std_logic_1164.all;
use ieee.std_logic_arith.all;
use ieee.std_logic_unsigned.all;



package newspaper_pack is

	constant N_ROWS : integer := 8;
	constant N_COLUMNS : integer := 16;
	constant N_STATES : integer := 4;

	type DIRECTION_T is (DIR_LEFT, DIR_RIGHT);
	type COLUMN_ARRAY_T is array (0 to N_COLUMNS-1) of std_logic_vector(0 to N_ROWS-1);

	function GETCOLUMN(signal DATA : in std_logic_vector; COLID : in integer; ROWS : in integer) return std_logic_vector;
	function log2(A: integer) return integer;
	function VecToStr(vector : std_logic_vector) return string;
	function LEDAnimations(data : COLUMN_ARRAY_T; animation : integer; position : integer) return COLUMN_ARRAY_T;

end newspaper_pack;



package body newspaper_pack is

	function GETCOLUMN (
		signal DATA : in std_logic_vector;
		COLID : in integer;
		ROWS : in integer
	) return std_logic_vector is
		variable result	: std_logic_vector(0 to (ROWS-1)) := (others => '0');
		constant COLS	: integer := DATA'length/ROWS;
		constant COLNUM	: integer := COLID mod COLS;
	begin
		if (DATA'length < 1) then
			report "DATA vector is empty!" severity error;
		elsif (DATA'length mod ROWS /= 0) then
			report "DATA vector is not divisible by number of ROWS!" severity error;
		elsif (COLID < 0) then
			report "COLID is smaller than the first index (0)!" severity warning;
			result := DATA((DATA'length-ROWS) to (DATA'length-1));
		elsif (COLID > COLS-1) then
			report "COLID is bigger than the last index (" & integer'image(COLS-1) & ")!" severity warning;
			result := DATA(0 to (ROWS-1));
		else
			--report integer'image(COLNUM*ROWS) & " to " & integer'image((COLNUM*ROWS)+(ROWS-1));
			result := DATA((COLNUM*ROWS) to ((COLNUM*ROWS)+(ROWS-1)));
		end if;

		return result;
	end;


	-- zde je funkce log2 z prednasek, pravdepodobne se vam bude hodit.
	function log2(A: integer) return integer is
		variable bits : integer := 0;
		variable b : integer := 1;
	begin
		while (b < a) loop
			b := b * 2;
			bits := bits + 1;
		end loop;
		return bits;
	end function;
	
	
	function VecToStr(
		vector : std_logic_vector
	) return string is
		variable resultASC	: string(0 to (vector'length-1));
		variable resultDESC	: string((vector'length-1) downto 0);
	begin
		--report "LENGTH: " & integer'image(vector'length);
		--report "ASCENDING: " & boolean'image(vector'ascending);
		
		for i in vector'range loop
			if (vector(i) = '1') then
				resultASC(i) := '1';
				resultDESC(i) := '1';
			elsif (vector(i) = '0') then
				resultASC(i) := '0';
				resultDESC(i) := '0';
			else
				resultASC(i) := 'X';
				resultDESC(i) := 'X';
			end if;
		end loop;
		
		if (vector'ascending) then
			return resultASC;
		else
			return resultDESC;
		end if;
	end;
	
	
	function LEDAnimations(
		data : COLUMN_ARRAY_T;
		animation : integer;
		position : integer
	) return COLUMN_ARRAY_T is
		variable last_data : COLUMN_ARRAY_T := data;
	begin
	
		if (animation = 0) then
			
			if (position = 0) then
				last_data(7) := last_data(7) xor "00011000";
				last_data(8) := last_data(8) xor "00011000";
			elsif (position = 2) then
				last_data(6) := last_data(6) xor "00010000";
				last_data(9) := last_data(9) xor "00010000";
			elsif (position = 4) then
				last_data(6) := last_data(6) xor "01000000";
				last_data(7) := last_data(7) xor "10000000";
				last_data(8) := last_data(8) xor "10000000";
				last_data(9) := last_data(9) xor "01000000";
			elsif (position = 6) then
				last_data(3) := "00000100";
				last_data(4) := "00000010";
				last_data(11) := "00000010";
				last_data(12) := "00000100";
			elsif (position = 8) then
				last_data(3) := "00000010";
				last_data(4) := "00000010";
				last_data(12) := "00000010";
				last_data(12) := "00000010";
			elsif (position = 10) then
				last_data(3) := "00000001";
				last_data(4) := "00000010";
				last_data(11) := "00000010";
				last_data(12) := "00000001";
			--elsif (position = 16) then
			--	last_data(5) := last_data(5) xor "10010000";
			--	last_data(6) := last_data(6) xor "11110000";
			--	last_data(7) := last_data(7) xor "10101000";
			--	last_data(8) := last_data(8) xor "10101000";
			--	last_data(9) := last_data(9) xor "11110000";
			--	last_data(10) := last_data(10) xor "10010000";
			--elsif (position = 19) then
			--	last_data(5) := last_data(5) xor "00100000";
			--	last_data(6) := last_data(6) xor "11110000";
			--	last_data(7) := last_data(7) xor "10101000";
			--	last_data(8) := last_data(8) xor "10101000";
			--	last_data(9) := last_data(9) xor "11110000";
			--	last_data(10) := last_data(10) xor "00100000";
			end if;
		
		elsif (animation = 1) then
			
			if (position = 1) then
				last_data(0) := "11110000";
			elsif (position = 2) then
				last_data(0) := "11111111";
			elsif (position = 3) then
				last_data(1) := "11110000";
			elsif (position = 4) then
				last_data(1) := "11111111";
			elsif (position = 5) then
				last_data(2) := last_data(2) or "11110000";
			elsif (position = 6) then
				last_data(2) := last_data(2) or "11111111";
			elsif (position = 7) then
				last_data(3) := last_data(3) or "11110000";
			elsif (position = 8) then
				last_data(3) := last_data(3) or "11111111";
				
			elsif (position = 9) then
				last_data(4) := last_data(4) or "11110000";
			elsif (position = 10) then
				last_data(4) := last_data(4) or "11111111";
			elsif (position = 11) then
				last_data(5) := last_data(5) or "11110000";
			elsif (position = 12) then
				last_data(5) := last_data(5) or "11111111";
			elsif (position = 13) then
				last_data(6) := last_data(6) or "11110000";
			elsif (position = 14) then
				last_data(6) := last_data(6) or "11111111";
			elsif (position = 15) then
				last_data(7) := last_data(7) or "11110000";
			elsif (position = 16) then
				last_data(7) := last_data(7) or "11111111";
				
			elsif (position = 17) then
				last_data(8) := last_data(8) or "11110000";
			elsif (position = 18) then
				last_data(8) := last_data(8) or "11111111";
			elsif (position = 19) then
				last_data(9) := last_data(9) or "11110000";
			elsif (position = 20) then
				last_data(9) := last_data(9) or "11111111";
			elsif (position = 21) then
				last_data(10) := last_data(10) or "11110000";
			elsif (position = 22) then
				last_data(10) := last_data(10) or "11111111";
			elsif (position = 23) then
				last_data(11) := last_data(11) or "11110000";
			elsif (position = 24) then
				last_data(11) := last_data(11) or "11111111";
			
			elsif (position = 25) then
				last_data(12) := last_data(12) or "11110000";
			elsif (position = 26) then
				last_data(12) := last_data(12) or "11111111";
			elsif (position = 27) then
				last_data(13) := last_data(13) or "11110000";
			elsif (position = 28) then
				last_data(13) := last_data(13) or "11111111";
			elsif (position = 29) then
				last_data(14) := "11110000";
			elsif (position = 30) then
				last_data(14) := "11111111";
			elsif (position = 31) then
				last_data(15) := "11110000";
			elsif (position = 32) then
				last_data(15) := "11111111";
			end if;
			
		elsif (animation = 2) then
		
			if (position = 16) then
				last_data(0) := last_data(0) or "10101010";
				last_data(1) := last_data(1) or "10101010";
				last_data(2) := last_data(2) or "10101010";
				last_data(3) := last_data(3) or "10101010";
				last_data(4) := last_data(4) or "10101010";
				last_data(5) := last_data(5) or "10101010";
				last_data(6) := last_data(6) or "10101010";
				last_data(7) := last_data(7) or "10101010";
				last_data(8) := last_data(8) or "10101010";
				last_data(9) := last_data(9) or "10101010";
				last_data(10) := last_data(10) or "10101010";
				last_data(11) := last_data(11) or "10101010";
				last_data(12) := last_data(12) or "10101010";
				last_data(13) := last_data(13) or "10101010";
				last_data(14) := last_data(14) or "10101010";
				last_data(15) := last_data(15) or "10101010";
			elsif (position = 32) then
				--last_data(0) := "10101010";
				last_data(1) := "10101010";
				last_data(2) := "10101010";
				last_data(3) := "10101010";
				last_data(4) := "10101010";
				last_data(5) := "10101010";
				last_data(6) := "10101010";
				last_data(7) := "10101010";
				last_data(8) := "10101010";
				last_data(9) := "10101010";
				last_data(10) := "10101010";
				last_data(11) := "10101010";
				last_data(12) := "10101010";
				last_data(13) := "10101010";
				last_data(14) := "10101010";
				--last_data(15) := "10101010";
			elsif (position = 48) then
				last_data(0) := "11111111";
				last_data(1) := "11111111";
				last_data(2) := "11111111";
				last_data(3) := "11111111";
				last_data(4) := "11111111";
				last_data(5) := "11111111";
				last_data(6) := "11111111";
				last_data(7) := "11111111";
				last_data(8) := "11111111";
				last_data(9) := "11111111";
				last_data(10) := "11111111";
				last_data(11) := "11111111";
				last_data(12) := "11111111";
				last_data(13) := "11111111";
				last_data(14) := "11111111";
				last_data(15) := "11111111";
			end if;
			
		elsif (animation = 3) then
			
			if (position = 8) then
				last_data(0) := "11111111";
				last_data(1) := "11111111";
				last_data(2) := "11111111";
				last_data(3) := "11111111";
				last_data(4) := "11111111";
				last_data(5) := "11111111";
				last_data(6) := "11111111";
				last_data(7) := "11111111";
				last_data(8) := "11111111";
				last_data(9) := "11111111";
				last_data(10) := "11111111";
				last_data(11) := "11111111";
				last_data(12) := "11111111";
				last_data(13) := "11111111";
				last_data(14) := "11111111";
				last_data(15) := "11111111";
			elsif (position = 16) then
				last_data(0) := "11111111";
				last_data(1) := "10000001";
				last_data(2) := "10000001";
				last_data(3) := "11111111";
				last_data(4) := "10000111";
				last_data(5) := "11111011";
				last_data(6) := "11111101";
				last_data(7) := "11111011";
				last_data(8) := "10000111";
				last_data(9) := "11111111";
				last_data(10) := "10000001";
				last_data(11) := "11100111";
				last_data(12) := "11100111";
				last_data(13) := "11100111";
				last_data(14) := "10000001";
				last_data(15) := "11111111";
			end if;
			
		end if;
		
		return last_data;
	
	end;

end newspaper_pack;
