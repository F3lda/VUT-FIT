library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.std_logic_arith.all;
use IEEE.std_logic_unsigned.all;


-- Definice vstupu a vystupu Krabicky celeho obvodu
entity vigenere is
   port(
         CLK : in std_logic;
         RST : in std_logic;
         DATA : in std_logic_vector(7 downto 0);
         KEY : in std_logic_vector(7 downto 0);

         CODE : out std_logic_vector(7 downto 0)
    );
end vigenere;


-- Krabicka celeho obvodu
architecture behavioral of vigenere is

	-- Definice vnitrnich signalu
	
	signal posuvTyp: std_logic_vector(1 downto 0); -- z FSM Mealy do Multiplexoru
	signal posuvVelikost: std_logic_vector(7 downto 0); -- vypocet velikosti posuvu do posuvniku
	signal dataPosunutaNahoru: std_logic_vector(7 downto 0); -- z posuvniku nahoru do multiplexoru
	signal dataPosunutaDolu: std_logic_vector(7 downto 0); -- z posuvniku dolu do multiplexoru
	
	type typStavTypPosuvu is (nahoru, dolu);
	signal stavTypPosuvu: typStavTypPosuvu := nahoru;
	signal dalsiStavTypPosuvu: typStavTypPosuvu := dolu;
	
	signal hashTag: std_logic_vector(7 downto 0) := "00100011"; -- kod Hash Tagu

begin
	-- Jednotlive porcesy (Mensi krabicky uvnitr obvodu) a logika celeho obvodu

	---------- VYPOCET POSUVU ----------
	vypocetPosuvu: process (DATA, KEY) is
	begin
		posuvVelikost <= KEY - 64; -- 64 = '@'
	end process;
	---------- VYPOCET POSUVU - END ----------

	---------- POSUNUTI ----------
	posunutiNahoru: process (DATA, posuvVelikost) is
		variable velikostAbecedy: std_logic_vector(7 downto 0) := "00011010"; -- = velikost abecedy = 26
	begin
		if ((DATA + posuvVelikost) > 90) then -- 90 = 'Z'
			dataPosunutaNahoru <= ((DATA + posuvVelikost) - velikostAbecedy);
		else
			dataPosunutaNahoru <= (DATA + posuvVelikost);
		end if;
	end process;
	--------------------
	posunutiDolu: process (DATA, posuvVelikost) is
		variable velikostAbecedy: std_logic_vector(7 downto 0) := "00011010"; -- = velikost abecedy = 26
	begin
		if ((DATA - posuvVelikost) < 65) then -- 65 = 'A'
			dataPosunutaDolu <= ((DATA - posuvVelikost) + velikostAbecedy);
		else
			dataPosunutaDolu <= (DATA - posuvVelikost);
		end if;
	end process;
	---------- POSUNUTI - END ----------
	
	---------- VYPOCET TYPU POSUVU ----------
	stavovaLogika: process (CLK, RST) is
	begin
		if (RST = '1') then
			stavTypPosuvu <= nahoru;
		elsif (rising_edge(CLK)) then
			stavTypPosuvu <= dalsiStavTypPosuvu;
		end if;
	end process;
	--------------------
	vypocetTypuPosuvu: process (DATA, CLK, RST, stavTypPosuvu) is
	begin
		if (stavTypPosuvu = nahoru) then
			dalsiStavTypPosuvu <= dolu;
			posuvTyp <= "01";
		elsif (stavTypPosuvu = dolu) then
			dalsiStavTypPosuvu <= nahoru;
			posuvTyp <= "10";
		end if;
		
		if ((DATA > 47 and DATA < 58) or (RST = '1')) then -- = (DATA >= '0' and DATA <= '9')
			posuvTyp <= "00";
		end if;
	end process;
	---------- VYPOCET TYPU POSUVU - END ----------
		
	---------- MULTIPLEXOR ----------
	multiplexor: process (dataPosunutaNahoru, dataPosunutaDolu, posuvTyp) is
	begin
		if (posuvTyp = "01") then
			CODE <= dataPosunutaNahoru;
		elsif (posuvTyp = "10") then
			CODE <= dataPosunutaDolu;
		else
			CODE <= hashTag;
		end if;
	end process;
	---------- MULTIPLEXOR - END ----------

end behavioral;
