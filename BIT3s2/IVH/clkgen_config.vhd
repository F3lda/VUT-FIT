-- clkgen_config.vhd: DCM constants & definitions
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

library ieee;
use ieee.std_logic_1164.all;
use ieee.std_logic_arith.all;

package clkgen_cfg is
   type dcm_freq is (DCM_100MHz, DCM_75MHz, DCM_50MHz, DCM_40MHz, DCM_25MHz, DCM_20MHz);
   -- DCM_100MHz -> 99.5328 MHz
   -- DCM_75MHz  -> 73.7280 MHz
   -- DCM_50MHz  -> 49.7664 MHz
   -- DCM_40MHz  -> 39.8131 MHz
   -- DCM_25MHz  -> 24.8832 MHz
   -- DCM_20MHz  -> 19.9066 MHz

   -- DCM coeficients for input 7.3728 MHz (SMCLK)
   function getdiv(constant mode : dcm_freq) return integer;
   function getmul(constant mode : dcm_freq) return integer;
end clkgen_cfg;

package body clkgen_cfg is

   function getdiv(constant mode : dcm_freq) return integer is
   begin
      if mode = DCM_100MHz then return 2;
      elsif mode = DCM_75MHz then return 1;
      elsif mode = DCM_50MHz then return 4;
      elsif mode = DCM_40MHz then return 5;
      elsif mode = DCM_25MHz then return 7;
      elsif mode = DCM_20MHz then return 10;
      end if;
      -- default 25 MHz
      return 7;
   end function;
   
   function getmul(constant mode : dcm_freq) return integer is
   begin
      if mode = DCM_100MHz then return 27;
      elsif mode = DCM_75MHz then return 10;
      elsif mode = DCM_50MHz then return 27;
      elsif mode = DCM_40MHz then return 27;
      elsif mode = DCM_25MHz then return 25;
      elsif mode = DCM_20MHz then return 27;
      end if;
      -- default 25 MHz
      return 25;
   end function;
   
end clkgen_cfg;
