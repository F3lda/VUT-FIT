-- fpga_xc3s50: Top level entity XC3S50
-- Copyright (C) 2006 Brno University of Technology,
--                    Faculty of Information Technology
-- Author(s): Ladislav Capka <xcapka01 AT stud.fit.vutbr.cz>
--
-- LICENSE TERMS
--
-- Redistribution and use in source and binary forms, with or without
-- modification, are permitted provided that the following conditions
-- are met:
-- 1. Redistributions of source code must retain the above copyright
--    notice, this list of conditions and the following disclaimer.
-- 2. Redistributions in binary form must reproduce the above copyright
--    notice, this list of conditions and the following disclaimer in
--    the documentation and/or other materials provided with the
--    distribution.
-- 3. All advertising materials mentioning features or use of this software
--    or firmware must display the following acknowledgement:
--
--      This product includes software developed by the University of
--      Technology, Faculty of Information Technology, Brno and its
--      contributors.
--
-- 4. Neither the name of the Company nor the names of its contributors
--    may be used to endorse or promote products derived from this
--    software without specific prior written permission.
--
-- This software or firmware is provided ``as is'', and any express or implied
-- warranties, including, but not limited to, the implied warranties of
-- merchantability and fitness for a particular purpose are disclaimed.
-- In no event shall the company or contributors be liable for any
-- direct, indirect, incidental, special, exemplary, or consequential
-- damages (including, but not limited to, procurement of substitute
-- goods or services; loss of use, data, or profits; or business
-- interruption) however caused and on any theory of liability, whether
-- in contract, strict liability, or tort (including negligence or
-- otherwise) arising in any way out of the use of this software, even
-- if advised of the possibility of such damage.
--
-- $Id$
--
--
library IEEE;
use IEEE.std_logic_1164.all;


use work.clkgen_cfg.all;

entity fpga is
port (

   -- hodiny
   SMCLK   : in std_logic;
   ACLK    : in std_logic;
   FCLK    : in std_logic;

   -- LED
   LEDF    : out std_logic;

   -- SPI
   SPI_CLK : in std_logic;
   SPI_CS  : in std_logic;
   SPI_FPGA_CS : in std_logic;
   SPI_DI  : out std_logic;
   SPI_DO  : in std_logic;

   -- klavesnice 4x4
   KIN   : out std_logic_vector(3 downto 0);
   KOUT  : in  std_logic_vector(3 downto 0);

   -- displej
   LE  : out std_logic;
   LRS : out std_logic;
   LRW : out std_logic;
   LD : inout std_logic_vector(7 downto 0)      := (others => 'Z');

   -- SDRAM
   RA      : out std_logic_vector(14 downto 0);
   RD      : inout std_logic_vector(7 downto 0) := (others => 'Z');
   RDQM    : out std_logic;
   RCS     : out std_logic;
   RRAS    : out std_logic;
   RCAS    : out std_logic;
   RWE     : out std_logic;
   RCLK    : out std_logic;
   RCKE    : out std_logic;

   -- P3M
   P3M     : inout std_logic_vector(7 downto 0)  := (others => 'Z');

   -- AFBUS
   AFBUS   : inout std_logic_vector(11 downto 0) := (others => 'Z');

   -- PC interface
   X       : inout std_logic_vector(45 downto 0) := (others => 'Z')

);
end fpga;


architecture arch_gp_ifc of fpga is

	--  type dcm_freq is (DCM_100MHz, DCM_75MHz, DCM_50MHz, DCM_40MHz, DCM_25MHz, DCM_20MHz);
	constant DCM_FREQUENCY : dcm_freq := DCM_25MHz;
	

   signal clk          : std_logic;
   signal clk_locked   : std_logic;
   signal reset        : std_logic;
   signal smclk_x1     : std_logic;
   signal ispi_rst     : std_logic;
   signal ispi_cs      : std_logic;
   signal ispi_di      : std_logic;
   signal ispi_di_req  : std_logic;
   signal ispi_do      : std_logic;
   signal ispi_do_vld  : std_logic;


   component clkgen is
      generic (
         FREQ    : dcm_freq := DCM_FREQUENCY
      );
      port (
         CLK        : in    std_logic;
         RST        : in    std_logic;
         CLK1X_OUT  : out   std_logic;
         CLKFX_OUT  : out   std_logic;
         LOCKED_OUT : out   std_logic
      );
   end component; 

   component tlv_gp_ifc
      port (
         -- hodiny
         SMCLK     : in std_logic;
         ACLK      : in std_logic;
         FCLK      : in std_logic;
         CLK       : in std_logic;

         RESET     : in std_logic;
         IRQ       : out std_logic := 'Z';

         -- LED
         LEDF      : out std_logic;

         -- interni SPI
         SPI_CS    : in  std_logic;
         SPI_DI    : out std_logic;
         SPI_DI_REQ: in  std_logic;
         SPI_DO    : in  std_logic;
         SPI_DO_VLD: in  std_logic;

         -- klavesnice 4x4
         KIN       : out std_logic_vector(3 downto 0);
         KOUT      : in  std_logic_vector(3 downto 0);

         -- displej
         LE        : out std_logic;
         LRS       : out std_logic;
         LRW       : out std_logic;
         LD        : inout std_logic_vector(7 downto 0) := (others => 'Z');

         -- SDRAM
         RA        : out std_logic_vector(14 downto 0);
         RD        : inout std_logic_vector(7 downto 0) := (others => 'Z');
         RDQM      : out std_logic;
         RCS       : out std_logic;
         RRAS      : out std_logic;
         RCAS      : out std_logic;
         RWE       : out std_logic;
         RCLK      : out std_logic;
         RCKE      : out std_logic;

         -- P3M
         P3M       : inout std_logic_vector(7 downto 0) := (others => 'Z');

         -- AFBUS
         AFBUS     : inout std_logic_vector(11 downto 0) := (others => 'Z');

         -- JP10
         X       : inout std_logic_vector(45 downto 6) := (others => 'Z');

         -- audio codec
         ALRCIN    : inout std_logic := 'Z';
         ADIN      : out std_logic;
         ALRCOUT   : inout std_logic := 'Z';
         ADOUT     : in std_logic    := 'Z';
         ABCLK     : inout std_logic := 'Z' 
      );
   end component;

begin
   --X(5 downto 1) <= (others => '0');

   -- DCM - clock generator (default 25MHz)
   DCMclkgen: entity work.clkgen
      generic map (
         FREQ        => DCM_FREQUENCY
      )
      port map (
         CLK         => SMCLK,
         RST         => P3M(0),
         CLK1X_OUT   => smclk_x1,
         CLKFX_OUT   => clk,
         LOCKED_OUT  => clk_locked
      ); 

   -- Resetovaci signal - zavisi na nabehnuti DCM a resetu z MCU
   reset <= P3M(0) or (not clk_locked); 

   -- FPGA design
   fpga_inst: tlv_gp_ifc
      port map (
         ACLK   => ACLK,
         FCLK   => FCLK,
         SMCLK => smclk_x1,
         CLK   => clk,
         RESET => reset,
         IRQ   => X(0),

         LEDF => LEDF,

         SPI_CS   => ispi_cs,
         SPI_DI   => ispi_di,
         SPI_DI_REQ => ispi_di_req,
         SPI_DO     => ispi_do,
         SPI_DO_VLD => ispi_do_vld,

         KIN  => KIN,
         KOUT => KOUT,

         LE  => LE,
         LRW => LRW,
         LRS => LRS,
         LD  => LD,

         RA   => RA,
         RD   => RD,
         RDQM => RDQM,
         RCS  => RCS,
         RRAS => RRAS,
         RCAS => RCAS,
         RWE  => RWE,
         RCKE => RCKE,
         RCLK => RCLK,

         P3M => P3M,

         AFBUS => AFBUS,

         X => X(45 downto 6),

         ALRCIN  => X(3),
         ADIN    => X(2),
         ALRCOUT => X(5),
         ADOUT   => X(4),
         ABCLK   => X(1)
      );

end arch_gp_ifc;
