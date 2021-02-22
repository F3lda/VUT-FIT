-- cpu.vhd: Simple 8-bit CPU (BrainF*ck interpreter)
-- Copyright (C) 2020 Brno University of Technology,
--                    Faculty of Information Technology
-- Author(s): xjirgl01
--

library ieee;
use ieee.std_logic_1164.all;
use ieee.std_logic_arith.all;
use ieee.std_logic_unsigned.all;


-- ----------------------------------------------------------------------------
--                        Entity declaration
-- ----------------------------------------------------------------------------
entity cpu is
 port (
   CLK   : in std_logic;  -- hodinovy signal
   RESET : in std_logic;  -- asynchronni reset procesoru
   EN    : in std_logic;  -- povoleni cinnosti procesoru
 
   -- synchronni pamet ROM
   CODE_ADDR : out std_logic_vector(11 downto 0); -- adresa do pameti
   CODE_DATA : in std_logic_vector(7 downto 0);   -- CODE_DATA <- rom[CODE_ADDR] pokud CODE_EN='1'
   CODE_EN   : out std_logic;                     -- povoleni cinnosti
   
   -- synchronni pamet RAM
   DATA_ADDR  : out std_logic_vector(9 downto 0); -- adresa do pameti
   DATA_WDATA : out std_logic_vector(7 downto 0); -- ram[DATA_ADDR] <- DATA_WDATA pokud DATA_EN='1'
   DATA_RDATA : in std_logic_vector(7 downto 0);  -- DATA_RDATA <- ram[DATA_ADDR] pokud DATA_EN='1'
   DATA_WE    : out std_logic;                    -- cteni (0) / zapis (1)
   DATA_EN    : out std_logic;                    -- povoleni cinnosti 
   
   -- vstupni port
   IN_DATA   : in std_logic_vector(7 downto 0);   -- IN_DATA <- stav klavesnice pokud IN_VLD='1' a IN_REQ='1'
   IN_VLD    : in std_logic;                      -- data platna
   IN_REQ    : out std_logic;                     -- pozadavek na vstup data
   
   -- vystupni port
   OUT_DATA : out  std_logic_vector(7 downto 0);  -- zapisovana data
   OUT_BUSY : in std_logic;                       -- LCD je zaneprazdnen (1), nelze zapisovat
   OUT_WE   : out std_logic                       -- LCD <- OUT_DATA pokud OUT_WE='1' a OUT_BUSY='0'
 );
end cpu;
	

-- ----------------------------------------------------------------------------
--                      Architecture declaration
-- ----------------------------------------------------------------------------
architecture behavioral of cpu is

 -- zde dopiste potrebne deklarace signalu
	
	-- PC (program counter - pointer to ROM [program memory])
		signal pc_data : std_logic_vector(11 downto 0);
		signal pc_inc : std_logic;
		signal pc_dec : std_logic;
		signal pc_load : std_logic;
	-- PC (program counter - pointer to ROM [program memory])
	
	-- RAS (return address stack - new LOOP)
		-- STACK
			constant RASstackSize : integer := 16;
			type RASstack is array(0 to RASstackSize-1) of std_logic_vector(11 downto 0);
			signal RASstackData : RASstack;
			signal RASstackTop : integer range -1 to RASstackSize-1;
		-- STACK
		signal ras_data : std_logic_vector(11 downto 0);
		signal ras_data_ok : std_logic;
		signal ras_push : std_logic;
		signal ras_pop : std_logic;
	-- RAS (return address stack - new LOOP)
	
	-- CNT (brackets counter - exit LOOP)
		signal cnt_data : std_logic_vector(11 downto 0);
		signal cnt_inc : std_logic;
		signal cnt_dec : std_logic;
	-- CNT (brackets counter - exit LOOP)
	
	-- PTR (pointer to RAM)
		signal ptr_data : std_logic_vector(9 downto 0);
		signal ptr_inc : std_logic;
		signal ptr_dec : std_logic;
	-- PTR (pointer to RAM)
	
	-- MUX
		signal mux_output : std_logic_vector(7 downto 0);
		signal mux_select : std_logic_vector(1 downto 0);
	-- MUX
	
	-- FSM (finite state machine)
		type fsmState is (s_start, s_cmd_get, s_cmd_get_next, s_cmd_decode, 
			s_pointer_inc, 
			s_pointer_dec, 
			s_value_inc, s_value_inc_mux, s_value_inc_done, 
			s_value_dec, s_value_dec_mux, s_value_dec_done,
			s_put, s_put_done,
			s_get, s_get_done,
			s_while_start, s_while_start_check, s_while_start_exit, s_while_start_exit_loop,
			s_while_end, s_while_end_check,
			s_null
		);
		signal fsm_state: fsmState := s_start;
		signal fsm_state_next: fsmState := s_start;
	-- FSM (finite state machine)
	
begin

 -- zde dopiste vlastni VHDL kod

	---------- PC (program counter - pointer to ROM [program memory]) ----------
	PC: process (CLK, RESET, pc_inc, pc_dec, pc_load) is
	begin
		--- inicializace
		if (RESET = '1') then
			pc_data <= (others => '0');
			
		--- beh procesu (hodin)
		elsif (rising_edge(CLK)) then

			if (pc_inc = '1') then

				pc_data <= pc_data + 1;

			elsif (pc_dec = '1') then

				pc_data <= pc_data - 1;

			elsif (pc_load = '1') then
				
				pc_data <= ras_data;

			end if;

		end if;
	end process;
	CODE_ADDR <= pc_data;
	---------- PC (program counter - pointer to ROM [program memory]) - END ----------
	
	---------- RAS (return address stack - new LOOP) ----------
	RAS: process (CLK, RESET, ras_push, ras_pop) is
	begin
		--- inicializace
		if (RESET = '1') then
			RASstackTop <= -1;
			RASstackData <= (others => (others => '0'));
			ras_data_ok <= '0';
			
		--- beh procesu (hodin)
		elsif (rising_edge(CLK)) then
		
			if (ras_push = '1') then

				if (RASstackTop = RASstackSize-1) then 
					ras_data_ok <= '1';
				else
					ras_data_ok <= '0';

					ras_data <= pc_data;
					RASstackData(RASstackTop + 1) <= pc_data;
					RASstackTop <= RASstackTop + 1;
				end if;

			elsif (ras_pop = '1') then

				if(RASstackTop = -1) then
					ras_data_ok <= '1';
				else
					ras_data_ok <= '0';

					if(RASstackTop /= 0) then
						ras_data <= RASstackData(RASstackTop - 1);
					else
						ras_data <= (others => '0');
					end if;
					RASstackData(RASstackTop) <= (others => '0');
					RASstackTop <= RASstackTop - 1;
				end if;
				
			end if;

		end if;
	end process;
	---------- RAS (return address stack - new LOOP) - END ----------
	
	---------- CNT (brackets counter - exit LOOP) ----------
	CNT: process (CLK, RESET, cnt_inc, cnt_dec) is
	begin
		--- inicializace
		if (RESET = '1') then
			cnt_data <= (others => '0');
			
		--- beh procesu (hodin)
		elsif (rising_edge(CLK)) then

			if (cnt_inc = '1') then

				cnt_data <= cnt_data + 1;

			elsif (cnt_dec = '1') then

				cnt_data <= cnt_data - 1;

			end if;

		end if;
	end process;
	---------- CNT (brackets counter - exit LOOP) - END ----------
	
	---------- PTR (pointer to RAM) ----------
	PTR: process (CLK, RESET, ptr_inc, ptr_dec) is
	begin
		--- inicializace
		if (RESET = '1') then
			ptr_data <= (others => '0');
			
		--- beh procesu (hodin)
		elsif (rising_edge(CLK)) then

			if (ptr_inc = '1') then

				ptr_data <= ptr_data + 1;

			elsif (ptr_dec = '1') then

				ptr_data <= ptr_data - 1;

			end if;

		end if;
	end process;
	DATA_ADDR <= ptr_data;
	---------- PTR (pointer to RAM) - END ----------
	
	---------- MUX ----------
	MUX: process (CLK, RESET, mux_select) is
	begin
		--- inicializace
		if (RESET = '1') then
			mux_output <= (others => '0');
			
		--- beh procesu (hodin)
		elsif (rising_edge(CLK)) then

			case mux_select is
				when "00" =>
					mux_output <= IN_DATA;
				when "01" =>
					mux_output <= DATA_RDATA + 1;
				when "10" =>
					mux_output <= DATA_RDATA - 1;
				when others =>
					mux_output <= DATA_RDATA;
			end case;
			
		end if;
	end process;
	DATA_WDATA <= mux_output;
	OUT_DATA <= DATA_RDATA;
	---------- MUX - END ----------
	
	---------- FSM (finite state machine) ----------
	FSMinit: process (CLK, RESET, EN) is
	begin
		if (RESET = '1') then
			fsm_state <= s_start;
		elsif (rising_edge(CLK)) then
			--if (EN = '1') then
				fsm_state <= fsm_state_next;
			--end if;
		end if;
	end process;
	--------------------
	FSM: process (EN, fsm_state, OUT_BUSY, IN_VLD, CODE_DATA, DATA_RDATA) is
	begin
		if (EN = '1') then
			
			pc_inc <= '0';
			pc_dec <= '0';
			pc_load <= '0';
			
			ras_push <= '0';
			ras_pop <= '0';
			
			cnt_inc <= '0';
			cnt_dec <= '0';
			
			ptr_inc <= '0';
			ptr_dec <= '0';
			
			mux_select <= "00";
			
			CODE_EN <= '0';
			
			DATA_EN <= '0';
			DATA_WE <= '0';
			
			IN_REQ <= '0';
			OUT_WE <= '0';
			
			
			case fsm_state is
			
				when s_start =>
					fsm_state_next <= s_cmd_get; -- start from 0
				
				when s_cmd_get_next =>
					pc_inc <= '1'; -- move on next CMD
					fsm_state_next <= s_cmd_get;
				
				when s_cmd_get =>
					CODE_EN <= '1'; -- read current CMD
					fsm_state_next <= s_cmd_decode;
					
				when s_cmd_decode =>
					case CODE_DATA is
						when "00111110" => 
							fsm_state_next <= s_pointer_inc;
						when "00111100" => 
							fsm_state_next <= s_pointer_dec;
						
						when "00101011" => 
							fsm_state_next <= s_value_inc;
						when "00101101" => 
							fsm_state_next <= s_value_dec;
						
						when "01011011" => 
							fsm_state_next <= s_while_start;
						when "01011101" => 
							fsm_state_next <= s_while_end;
						
						when "00101110" => 
							fsm_state_next <= s_put;
						when "00101100" => 
							fsm_state_next <= s_get;
						
						when "00000000" =>
							fsm_state_next <= s_null;
						
						when others => -- mezera/novy radek/jiny znak -> get next
							fsm_state_next <= s_cmd_get_next;
					end case;
				
				
				when s_pointer_inc =>
					ptr_inc <= '1';
					fsm_state_next <= s_cmd_get_next;
				
				when s_pointer_dec =>
					ptr_dec <= '1';
					fsm_state_next <= s_cmd_get_next;
				
				
				-- read
				when s_value_inc =>
					DATA_EN <= '1';
					DATA_WE <= '0';
					fsm_state_next <= s_value_inc_mux;
				-- add
				when s_value_inc_mux =>
					mux_select <= "01";
					fsm_state_next <= s_value_inc_done;
				-- write
				when s_value_inc_done =>
					DATA_EN <= '1';
					DATA_WE <= '1';
					fsm_state_next <= s_cmd_get_next;
				
				
				-- read
				when s_value_dec =>
					DATA_EN <= '1';
					DATA_WE <= '0';
					fsm_state_next <= s_value_dec_mux;
				-- sub
				when s_value_dec_mux =>
					mux_select <= "10";
					fsm_state_next <= s_value_dec_done;
				-- write
				when s_value_dec_done =>
					DATA_EN <= '1';
					DATA_WE <= '1';
					fsm_state_next <= s_cmd_get_next;
				
				
				when s_put => -- read from RAM
					DATA_EN <= '1';
					DATA_WE <= '0';
					fsm_state_next <= s_put_done;
				
				when s_put_done =>
					if (OUT_BUSY = '1') then -- OUTPUT busy -> try again
						DATA_EN <= '1';
						DATA_WE <= '0';
						fsm_state_next <= s_put_done;
					else -- write to OUTPUT
						OUT_WE <= '1';
						fsm_state_next <= s_cmd_get_next;
					end if;
				
				
				when s_get => -- request INPUT
					IN_REQ <= '1';
					mux_select <= "00";
					fsm_state_next <= s_get_done;
				
				when s_get_done =>
					if (IN_VLD /= '1') then -- INPUT not valid -> wait
						IN_REQ <= '1';
						mux_select <= "00";
						fsm_state_next <= s_get_done;
					else -- write to RAM
						DATA_EN <= '1';
						DATA_WE <= '1';
						fsm_state_next <= s_cmd_get_next;
					end if;
				
				
				-- [ -> move on a next CMD and read current RAM data
				when s_while_start =>
					pc_inc <= '1';
					DATA_EN <= '1';
					DATA_WE <= '0';
					fsm_state_next <= s_while_start_check;
				
				when s_while_start_check =>
					if (DATA_RDATA /= "0000000000") then -- current RAM data != 0 -> push new LOOP to the stack and read current CMD and continue
						ras_push <= '1';
						fsm_state_next <= s_cmd_get;
					else -- read current CMD, set LOOP count = 1 and exit LOOP
						CODE_EN <= '1';
						cnt_inc <= '1';
						fsm_state_next <= s_while_start_exit;
					end if;
				
				when s_while_start_exit =>
					if (cnt_data = "000000000000") then -- if LOOP count == 0 -> read current CMD and continue
						fsm_state_next <= s_cmd_get;
					else -- check brackets and get a next cmd
						if (CODE_DATA = "01011011") then -- [
							cnt_inc <= '1';
						elsif (CODE_DATA = "01011101") then -- ]
							cnt_dec <= '1';
						end if;
						
						pc_inc <= '1'; -- move on a next CMD
						fsm_state_next <= s_while_start_exit_loop;
					end if;
					
				when s_while_start_exit_loop =>
					CODE_EN <= '1'; -- read current CMD
					fsm_state_next <= s_while_start_exit;
				
				
				-- ] -> read current RAM data
				when s_while_end =>
					DATA_EN <= '1';
					DATA_WE <= '0';
					fsm_state_next <= s_while_end_check;
					
				when s_while_end_check =>
					if (DATA_RDATA /= "0000000000") then -- current RAM data != 0 -> get LOOP start from the stack and read current CMD and continue
						pc_load <= '1';
						fsm_state_next <= s_cmd_get;
					else -- pop LOOP from the stack and move on a next CMD and continue
						ras_pop <= '1';
						fsm_state_next <= s_cmd_get_next;
					end if;
				
				
				when others =>
					null;
				
			end case;
			
		end if;
	end process;
	---------- FSM (finite state machine) - END ----------

 -- pri tvorbe kodu reflektujte rady ze cviceni INP, zejmena mejte na pameti, ze 
 --   - nelze z vice procesu ovladat stejny signal,
 --   - je vhodne mit jeden proces pro popis jedne hardwarove komponenty, protoze pak
 --   - u synchronnich komponent obsahuje sensitivity list pouze CLK a RESET a 
 --   - u kombinacnich komponent obsahuje sensitivity list vsechny ctene signaly.
 
end behavioral;
