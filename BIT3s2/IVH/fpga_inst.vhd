-- top.vhd: IVH projekt
-- Author(s): Karel Jirgl (xjirgl01)
-- Copyright (C) 2022

library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
use IEEE.STD_LOGIC_UNSIGNED.ALL;
use work.newspaper_pack.all;



architecture behavioral of tlv_gp_ifc is
	
	-- LED Display Refresh Rate
	-- -> Display data on all columns simultaneously
	-- ---------------------------------------------
	-- No flicker is visible at a refresh rate greater than 50Hz:
	-- 20 MHz CLK = 20 000 000 Hz -> 20 000 000 Hz / 50 Hz = 400 000 Hz -> 400 000 Hz / 16 rows = 25 000 Hz refresh rate -> 25 000 Hz
	-- 20 MHz CLK = 20 000 000 Hz -> 20 000 000 Hz / 60 Hz = 333 333 Hz -> 333 333 Hz / 16 rows = 20 833 Hz refresh rate -> 20 833 Hz
	constant CLK_PERIOD : integer := 50; -- 20 000 000 Hz (Frequency) = 50 ns (Period)
	constant LED_OUT_PERIOD : integer := 48000; -- 20 833 Hz (Frequency) = 48 000 ns (Period)
	signal LED_EN : std_logic := '0';
	
	constant SEC_OUT_PERIOD : integer := 1000000000; -- 1s = 20 000 000 Hz (Frequency) = 1 000 000 000 ns (Period)
	signal SEC_EN : std_logic := '0';
	
	
	
	-- ROM
	signal ROM_DATA : COLUMN_ARRAY_T := (others => (others => '0')); -- data from ROM to CONTROLLER
	-- CONTROLLER
	signal CURRENT_STATE : std_logic_vector(log2(N_STATES)-1 downto 0) := "00"; -- currently selected image in ROM
	signal ROT_DIRECTION : DIRECTION_T := DIR_LEFT; -- rotation direction
	signal ROT_EN : std_logic := '0'; -- rotation enabled
	signal CONTROLLER_DATA : COLUMN_ARRAY_T := (others => (others => '0')); -- data from CONTROLLER to COLUMNs
	-- COLUMN
	signal COLUMN_STATE : COLUMN_ARRAY_T := (others => (others => '0')); -- data from COLUMNs to neighboring COLUMNs and LED display CONTROLLER
	-- LED CONTROLLER
	signal A : std_logic_vector(3 downto 0) := "0000"; -- id sloupce
	signal R : std_logic_vector(0 to 7) := "00000000"; -- data sloupce
	
begin

	-- Instance pameti ROM ------------------------------------------------
	ROM_MEM : entity work.rom_memory
	generic map(
		DATA_BLOCKS	=> 
			"0000000000000000"&
			"0000010000100000"&
			"0000010000100000"&
			"0000010000100000"&
			"0001000000001000"&
			"0000100000010000"&
			"0000011111100000"&
			"0000000000000000"&
			
			"0000000110000000"&
			"0000001001010000"&
			"0000010000110000"&
			"0000100000010000"&
			"0001111111111000"&
			"0011000000001100"&
			"0001000110001000"&
			"0001111111111000"&
			
			"0000000000111110"&
			"0000000000100010"&
			"0000011100111110"&
			"0000010100100000"&
			"0100010100100010"&
			"0011111111111100"&
			"0001000000001000"&
			"0000111111110000"&
			
			"0000000000000000"&
			"0111100100111110"&
			"0100000100001000"&
			"0100000100001000"&
			"0111100100001000"&
			"0100000100001000"&
			"0100000100001000"&
			"0000000000000000"
	)
	port map(
		BLOCK_ADDR	=> CURRENT_STATE,
		DATA_OUT    => ROM_DATA
	);
	
	
	
	-- Generovani SLOUPCU ------------------------------------------------
	GEN_COLUMN_ARRAY: for i in 0 to N_COLUMNS-1 generate
	
		GEN_COLUMN_ARRAY_IF_1: if (i = 0) generate

			COLUMN_CONTROLLER : entity work.column_ctrl
			generic map(
				N	=> N_ROWS
			)
			port map(
				CLK				=> SEC_EN,
				RESET			=> reset,
				
				STATE			=> COLUMN_STATE(i),
				INIT_STATE		=> CONTROLLER_DATA(i),
				NEIGH_LEFT		=> COLUMN_STATE(N_COLUMNS-1),
				NEIGH_RIGHT		=> COLUMN_STATE(i+1),
				
				DIRECTION		=> ROT_DIRECTION,
				EN				=> ROT_EN
			);
		
		end generate GEN_COLUMN_ARRAY_IF_1;

		GEN_COLUMN_ARRAY_IF_2: if (i = N_COLUMNS-1) generate

			COLUMN_CONTROLLER : entity work.column_ctrl
			generic map(
				N	=> N_ROWS
			)
			port map(
				CLK				=> SEC_EN,
				RESET			=> reset,
				
				STATE			=> COLUMN_STATE(i),
				INIT_STATE		=> CONTROLLER_DATA(i),
				NEIGH_LEFT		=> COLUMN_STATE(i-1),
				NEIGH_RIGHT		=> COLUMN_STATE(0),
				
				DIRECTION		=> ROT_DIRECTION,
				EN				=> ROT_EN
			);
			
		end generate GEN_COLUMN_ARRAY_IF_2;
		
		GEN_COLUMN_ARRAY_IF_3: if (i /= 0 and i /= N_COLUMNS-1) generate
		
			COLUMN_CONTROLLER : entity work.column_ctrl
			generic map(
				N	=> N_ROWS
			)
			port map(
				CLK				=> SEC_EN,
				RESET			=> reset,
				
				STATE			=> COLUMN_STATE(i),
				INIT_STATE		=> CONTROLLER_DATA(i),
				NEIGH_LEFT		=> COLUMN_STATE(i-1),
				NEIGH_RIGHT		=> COLUMN_STATE(i+1),
				
				DIRECTION		=> ROT_DIRECTION,
				EN				=> ROT_EN
			);
			
		end generate GEN_COLUMN_ARRAY_IF_3;

	end generate GEN_COLUMN_ARRAY;



	-- LED display controller ------------------------------------------------
	TIMER_LED : entity work.counter
	GENERIC MAP (
		CLK_PERIOD => CLK_PERIOD,
		OUT_PERIOD => LED_OUT_PERIOD
	)
	PORT MAP (
		CLK => clk,
		RESET => reset,
		EN => LED_EN
	);
	
	RENDER_LED : process (clk) is
	begin
		if rising_edge(clk) then
			if (LED_EN = '1') then
				A <= A + 1; -- change column id
			end if;
		end if;
	end process;
	-- send current column to LED display
	R <= COLUMN_STATE(conv_integer(A));
	
	
	
	-- Controller ------------------------------------------------
	TIMER_SEC : entity work.counter
	GENERIC MAP (
		CLK_PERIOD => CLK_PERIOD,
		OUT_PERIOD => (SEC_OUT_PERIOD/2)/2/2 -- (one clock per half a second) -> the rest (/2) is just to speed up rotation
	)
	PORT MAP (
		CLK => clk,
		RESET => reset,
		EN => SEC_EN
	);
	
	CONTROLLER : process (clk) is
		variable LAST_STATE : integer := 0;
		variable LAST_ROTATION : DIRECTION_T := DIR_LEFT;
		
		variable COUNTER : integer := 0;
		constant COUNTER_ANI : integer := 64; -- 64 steps is for animation
		constant COUNTER_MAX : integer := (2)*16*3*2+COUNTER_ANI; -- (2) -> two steps per second; 16 = 16 columns; 3 = 3 times rotation to one side; 2 = rotation to both directions (left and rigth)
		constant COUNTER_ANI_START : integer := COUNTER_MAX-COUNTER_ANI-1;
	begin
	
		if RESET = '1' then
			COUNTER := 0;
			CONTROLLER_DATA <= ROM_DATA; -- ROM DATA directly to COLUMN
		elsif rising_edge(clk) then
		
			if (COUNTER < COUNTER_ANI_START or COUNTER > COUNTER_MAX-1) then -- when animation is off
				CONTROLLER_DATA <= ROM_DATA; -- ROM DATA directly to COLUMN
			end if;
			
			if (SEC_EN = '1') then
			
				-- ANIMATIONS
				if (COUNTER >= COUNTER_ANI_START and COUNTER < COUNTER_MAX-1) then
					CONTROLLER_DATA <= LEDAnimations(CONTROLLER_DATA, LAST_STATE, COUNTER-COUNTER_ANI_START);
				end if;
				
				
				
				-- STATE CHANGE -- image change
				if (COUNTER = COUNTER_MAX-1) then -- odd pulse
					if (LAST_STATE = N_STATES-1) then
						CURRENT_STATE <= (others => '0');
						LAST_STATE := 0;
					else
						CURRENT_STATE <= CURRENT_STATE + 1;
						LAST_STATE := LAST_STATE + 1;
					end if;
				end if;
				
				-- ROTATION ENABLE
				if (COUNTER mod 2 = 0 and COUNTER < COUNTER_ANI_START) then -- even pulse -> wait one clock tick to load new image before rotation
					ROT_EN <= '1';
				else
					ROT_EN <= '0';
				end if;
				
				-- ROTATION DIRECTION
				if (COUNTER = ((COUNTER_MAX-COUNTER_ANI)/2)-1 or COUNTER = (COUNTER_MAX)-1) then -- odd pulse
					if (LAST_ROTATION = DIR_LEFT) then
						LAST_ROTATION := DIR_RIGHT;
						ROT_DIRECTION <= DIR_RIGHT;
					else
						LAST_ROTATION := DIR_LEFT;
						ROT_DIRECTION <= DIR_LEFT;
					end if;
				end if;
				
				
				
				-- COUNTER
				if (COUNTER = COUNTER_MAX-1) then -- odd pulse
					COUNTER := 0;
				else
					COUNTER := COUNTER + 1;
				end if;
				
			end if;
			
		end if;
		
	end process;



    -- mapovani vystupu
    -- nemenit
    X(6) <= A(3);
    X(8) <= A(1);
    X(10) <= A(0);
    X(7) <= '0'; -- en_n
    X(9) <= A(2);

    X(16) <= R(1);
    X(18) <= R(0);
    X(20) <= R(7);
    X(22) <= R(2);
  
    X(17) <= R(4);
    X(19) <= R(3);
    X(21) <= R(6);
    X(23) <= R(5);
	
end behavioral;
