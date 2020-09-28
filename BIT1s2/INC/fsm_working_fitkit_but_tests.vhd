-- fsm.vhd: Finite State Machine
-- Author(s): Karel Jirgl
-- access codes: kod1 = 2011305092; kod2 = 2346522608;

library ieee;
use ieee.std_logic_1164.all;

-- ----------------------------------------------------------------------------
--                        Entity declaration
-- ----------------------------------------------------------------------------
entity fsm is
port(
	CLK         : in  std_logic; -- clock sync signal
	RESET       : in  std_logic; -- clock reset signal

	-- Input signals
	KEY         : in  std_logic_vector(15 downto 0); -- key number
	CNT_OF      : in  std_logic; -- counter overflow signal (when message successfully read from memory)

	-- Output signals
	FSM_CNT_CE  : out std_logic; -- counter clock enable signal (enable reading from memory)
	FSM_MX_MEM  : out std_logic; -- multiplexor memory selector (select message Pristup povolen or Pristup odepren)
	FSM_MX_LCD  : out std_logic; -- multiplexor LCD print type  (print message from memory or append one star '*')
	FSM_LCD_WR  : out std_logic; -- write to LCD display
	FSM_LCD_CLR : out std_logic  -- clear LCD display
);
end entity fsm;

-- ----------------------------------------------------------------------------
--                      Architecture declaration
-- ----------------------------------------------------------------------------
architecture behavioral of fsm is
	type t_state is (READ_FIRST_NUMBER, PRINT_MESSAGE_ACCESS_DENIED, PRINT_MESSAGE_ACCESS_ALLOWED,
	CODE_1_FIRST, CODE_1,
						CODE1_2, CODE1_3, CODE1_4, CODE1_5, CODE1_6, CODE1_7, CODE1_8, CODE1_9, CODE1_10,
						CODE2_2, CODE2_3, CODE2_4, CODE2_5, CODE2_6, CODE2_7, CODE2_8, CODE2_9, CODE2_10,
	CODE_WRONG_FIRST, CODE_WRONG);
	signal present_state, next_state : t_state;
begin
-- -------------------------------------------------------   MAIN LOOP
	sync_logic : process(RESET, CLK)
	begin
		if (RESET = '1') then
			present_state <= READ_FIRST_NUMBER;
		elsif (CLK'event AND CLK = '1') then
			present_state <= next_state;
		end if;
	end process sync_logic;

-- -------------------------------------------------------   LOGIC LOOP
next_state_logic : process(present_state, KEY, CNT_OF)
begin
	next_state <= present_state;                                                -- repeat same state if nothing changed
	if (KEY(14) = '1') then                                                     -- clear LCD and reset input by pressing key 14 ('*')
		next_state <= READ_FIRST_NUMBER; 
	elsif(not (KEY(15) = '1' and KEY(14 downto 0) /= "000000000000000")) then   -- check state only if key 15 and other key are not down at the same time
		case (present_state) is
		-- - - - - - - - - - - - - - - - - - - - - - -
		-- wait for a first code number and clear LCD
		when READ_FIRST_NUMBER =>
			if (KEY(2) = '1') then                             -- right code number
				next_state <= CODE_1_FIRST;
			elsif (KEY(15) = '1') then                         -- wrong code (too short)
				next_state <= PRINT_MESSAGE_ACCESS_DENIED; 
			elsif (KEY(13 downto 0) /= "00000000000000") then  -- wrong code number (skip key 14 '*' for clearing LCD)
				next_state <= CODE_WRONG_FIRST;
			end if;
		-- - - - - - - - - - - - - - - - - - - - - - -
		-- print first star ('*') after LCD is cleared
		when CODE_1_FIRST =>
			next_state <= CODE_1;
		when CODE_WRONG_FIRST =>
			next_state <= CODE_WRONG;
		-- - - - - - - - - - - - - - - - - - - - - - -
		-- wait for a next code numbers
		-- - - - - - - - - - - - - - - - - - - - - - -

		-- both codes have the same start
		-- - - - - - - - - - - - - - - - - - - - - - -
		when CODE_1 =>
			if (KEY(0) = '1') then                             -- right code number - kod1
				next_state <= CODE1_2;
			elsif (KEY(3) = '1') then                          -- right code number - kod2
				next_state <= CODE2_2;
			elsif (KEY(15) = '1') then                         -- wrong code (too short)
				next_state <= PRINT_MESSAGE_ACCESS_DENIED; 
			elsif (KEY(13 downto 0) /= "00000000000000") then  -- wrong code number
				next_state <= CODE_WRONG;
			end if;

		-- kod1
		-- - - -
		when CODE1_2 =>
			if (KEY(1) = '1') then                             -- right code number
				next_state <= CODE1_3;
			elsif (KEY(15) = '1') then                         -- wrong code (too short)
				next_state <= PRINT_MESSAGE_ACCESS_DENIED; 
			elsif (KEY(13 downto 0) /= "00000000000000") then  -- wrong code number
				next_state <= CODE_WRONG;
			end if;
			
		when CODE1_3 =>
			if (KEY(1) = '1') then                             -- right code number
				next_state <= CODE1_4;
			elsif (KEY(15) = '1') then                         -- wrong code (too short)
				next_state <= PRINT_MESSAGE_ACCESS_DENIED; 
			elsif (KEY(13 downto 0) /= "00000000000000") then  -- wrong code number
				next_state <= CODE_WRONG;
			end if;

		when CODE1_4 =>
			if (KEY(3) = '1') then                             -- right code number
				next_state <= CODE1_5;
			elsif (KEY(15) = '1') then                         -- wrong code (too short)
				next_state <= PRINT_MESSAGE_ACCESS_DENIED; 
			elsif (KEY(13 downto 0) /= "00000000000000") then  -- wrong code number
				next_state <= CODE_WRONG;
			end if;
			
		when CODE1_5 =>
			if (KEY(0) = '1') then                             -- right code number
				next_state <= CODE1_6;
			elsif (KEY(15) = '1') then                         -- wrong code (too short)
				next_state <= PRINT_MESSAGE_ACCESS_DENIED; 
			elsif (KEY(13 downto 0) /= "00000000000000") then  -- wrong code number
				next_state <= CODE_WRONG;
			end if;
			
		when CODE1_6 =>
			if (KEY(5) = '1') then                             -- right code number
				next_state <= CODE1_7;
			elsif (KEY(15) = '1') then                         -- wrong code (too short)
				next_state <= PRINT_MESSAGE_ACCESS_DENIED; 
			elsif (KEY(13 downto 0) /= "00000000000000") then  -- wrong code number
				next_state <= CODE_WRONG;
			end if;
			
		when CODE1_7 =>
			if (KEY(0) = '1') then                             -- right code number
				next_state <= CODE1_8;
			elsif (KEY(15) = '1') then                         -- wrong code (too short)
				next_state <= PRINT_MESSAGE_ACCESS_DENIED; 
			elsif (KEY(13 downto 0) /= "00000000000000") then  -- wrong code number
				next_state <= CODE_WRONG;
			end if;
			
		when CODE1_8 =>
			if (KEY(9) = '1') then                             -- right code number
				next_state <= CODE1_9;
			elsif (KEY(15) = '1') then                         -- wrong code (too short)
				next_state <= PRINT_MESSAGE_ACCESS_DENIED; 
			elsif (KEY(13 downto 0) /= "00000000000000") then  -- wrong code number
				next_state <= CODE_WRONG;
			end if;
			
		when CODE1_9 =>
			if (KEY(2) = '1') then                             -- right code number
				next_state <= CODE1_10;
			elsif (KEY(15) = '1') then                         -- wrong code (too short)
				next_state <= PRINT_MESSAGE_ACCESS_DENIED; 
			elsif (KEY(13 downto 0) /= "00000000000000") then  -- wrong code number
				next_state <= CODE_WRONG;
			end if;

		when CODE1_10 =>
			if (KEY(15) = '1') then                            -- CODE kod1 OK
				next_state <= PRINT_MESSAGE_ACCESS_ALLOWED; 
			elsif (KEY(13 downto 0) /= "00000000000000") then  -- wrong code number
				next_state <= CODE_WRONG;
			end if;

		-- kod2
		-- - - -		
		when CODE2_2 =>
			if (KEY(4) = '1') then                             -- right code number
				next_state <= CODE2_3;
			elsif (KEY(15) = '1') then                         -- wrong code (too short)
				next_state <= PRINT_MESSAGE_ACCESS_DENIED; 
			elsif (KEY(13 downto 0) /= "00000000000000") then  -- wrong code number
				next_state <= CODE_WRONG;
			end if;
			
		when CODE2_3 =>
			if (KEY(6) = '1') then                             -- right code number
				next_state <= CODE2_4;
			elsif (KEY(15) = '1') then                         -- wrong code (too short)
				next_state <= PRINT_MESSAGE_ACCESS_DENIED; 
			elsif (KEY(13 downto 0) /= "00000000000000") then  -- wrong code number
				next_state <= CODE_WRONG;
			end if;

		when CODE2_4 =>
			if (KEY(5) = '1') then                             -- right code number
				next_state <= CODE2_5;
			elsif (KEY(15) = '1') then                         -- wrong code (too short)
				next_state <= PRINT_MESSAGE_ACCESS_DENIED; 
			elsif (KEY(13 downto 0) /= "00000000000000") then  -- wrong code number
				next_state <= CODE_WRONG;
			end if;
			
		when CODE2_5 =>
			if (KEY(2) = '1') then                             -- right code number
				next_state <= CODE2_6;
			elsif (KEY(15) = '1') then                         -- wrong code (too short)
				next_state <= PRINT_MESSAGE_ACCESS_DENIED; 
			elsif (KEY(13 downto 0) /= "00000000000000") then  -- wrong code number
				next_state <= CODE_WRONG;
			end if;
			
		when CODE2_6 =>
			if (KEY(2) = '1') then                             -- right code number
				next_state <= CODE2_7;
			elsif (KEY(15) = '1') then                         -- wrong code (too short)
				next_state <= PRINT_MESSAGE_ACCESS_DENIED; 
			elsif (KEY(13 downto 0) /= "00000000000000") then  -- wrong code number
				next_state <= CODE_WRONG;
			end if;
			
		when CODE2_7 =>
			if (KEY(6) = '1') then                             -- right code number
				next_state <= CODE2_8;
			elsif (KEY(15) = '1') then                         -- wrong code (too short)
				next_state <= PRINT_MESSAGE_ACCESS_DENIED; 
			elsif (KEY(13 downto 0) /= "00000000000000") then  -- wrong code number
				next_state <= CODE_WRONG;
			end if;
			
		when CODE2_8 =>
			if (KEY(0) = '1') then                             -- right code number
				next_state <= CODE2_9;
			elsif (KEY(15) = '1') then                         -- wrong code (too short)
				next_state <= PRINT_MESSAGE_ACCESS_DENIED; 
			elsif (KEY(13 downto 0) /= "00000000000000") then  -- wrong code number
				next_state <= CODE_WRONG;
			end if;
			
		when CODE2_9 =>
			if (KEY(8) = '1') then                             -- right code number
				next_state <= CODE2_10;
			elsif (KEY(15) = '1') then                         -- wrong code (too short)
				next_state <= PRINT_MESSAGE_ACCESS_DENIED; 
			elsif (KEY(13 downto 0) /= "00000000000000") then  -- wrong code number
				next_state <= CODE_WRONG;
			end if;

		when CODE2_10 =>
			if (KEY(15) = '1') then                            -- CODE kod2 OK
				next_state <= PRINT_MESSAGE_ACCESS_ALLOWED; 
			elsif (KEY(13 downto 0) /= "00000000000000") then  -- wrong code number
				next_state <= CODE_WRONG;
			end if;

		-- - - - - - - - - - - - - - - - - - - - - - -
		-- wait for a next code numbers - end
		-- - - - - - - - - - - - - - - - - - - - - - -
		-- handle wrong code
		when CODE_WRONG =>
			if (KEY(15) = '1') then                            -- wrong code
				next_state <= PRINT_MESSAGE_ACCESS_DENIED;
			end if;
		-- - - - - - - - - - - - - - - - - - - - - - -
		-- print successful message
		when PRINT_MESSAGE_ACCESS_ALLOWED =>
			if (CNT_OF = '1') then -- message successfully read from memory and printed
				next_state <= READ_FIRST_NUMBER;
			end if;
		-- - - - - - - - - - - - - - - - - - - - - - -
		-- print unsuccessful message
		when PRINT_MESSAGE_ACCESS_DENIED =>
			if (CNT_OF = '1') then -- message successfully read from memory and printed
				next_state <= READ_FIRST_NUMBER;
			end if;
		-- - - - - - - - - - - - - - - - - - - - - - -
		end case;
	end if;
end process next_state_logic;

-- -------------------------------------------------------   PRINT LOOP
	output_logic : process(present_state, KEY)
	begin
		FSM_CNT_CE     <= '0'; -- TRUE - activate reading from memory
		FSM_MX_MEM     <= '0'; -- TRUE - Pristup povolen; FALSE - Pristup odepren
		FSM_MX_LCD     <= '0'; -- TRUE - print message; FALSE - print star ('*')
		FSM_LCD_WR     <= '0'; -- TRUE - write to LCD
		FSM_LCD_CLR    <= '0'; -- TRUE - clear LCD

		case (present_state) is
		-- - - - - - - - - - - - - - - - - - - - - - -
		when PRINT_MESSAGE_ACCESS_DENIED =>
			FSM_MX_LCD     <= '1'; -- set message to be written
			FSM_CNT_CE     <= '1'; -- activate reading from memory
			if (CNT_OF = '0') then -- when memory is ready
				FSM_LCD_WR     <= '1'; -- write to LCD
			end if;
		-- - - - - - - - - - - - - - - - - - - - - - -
		when PRINT_MESSAGE_ACCESS_ALLOWED =>
			FSM_MX_MEM     <= '1'; -- set Pristup povolen message
			FSM_MX_LCD     <= '1'; -- set message to be written
			FSM_CNT_CE     <= '1'; -- activate reading from memory
			if (CNT_OF = '0') then -- when memory is ready
				FSM_LCD_WR     <= '1'; -- write to LCD
			end if;
		-- - - - - - - - - - - - - - - - - - - - - - -
		when READ_FIRST_NUMBER =>
			if (KEY(14 downto 0) /= "000000000000000") then
				FSM_LCD_CLR    <= '1'; -- clear LCD for a new code after message
			end if;
		-- - - - - - - - - - - - - - - - - - - - - - -
		when others =>
			-- else if statement must be in this order
			if (KEY(15) = '1' or KEY(14) = '1') then
				FSM_LCD_CLR    <= '1'; -- clear LCD for a message or for a new code after key 14 is pressed
			elsif (KEY(13 downto 0) /= "00000000000000" or present_state = CODE_1_FIRST or present_state = CODE_WRONG_FIRST) then
				FSM_LCD_WR     <= '1'; -- write a star to LCD ('*')
			end if;
		-- - - - - - - - - - - - - - - - - - - - - - -
		end case;
	end process output_logic;

end architecture behavioral;