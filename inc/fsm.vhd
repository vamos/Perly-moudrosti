-- fsm.vhd: Finite State Machine
-- Author(s): xdvora2t 
--
library ieee;
use ieee.std_logic_1164.all;
-- ----------------------------------------------------------------------------
--                        Entity declaration
-- ----------------------------------------------------------------------------
entity fsm is
port(
   CLK         : in  std_logic;
   RESET       : in  std_logic;

   -- Input signals
   KEY         : in  std_logic_vector(15 downto 0);
   CNT_OF      : in  std_logic;

   -- Output signals
   FSM_CNT_CE  : out std_logic;
   FSM_MX_MEM  : out std_logic;
   FSM_MX_LCD  : out std_logic;
   FSM_LCD_WR  : out std_logic;
   FSM_LCD_CLR : out std_logic
);
end entity fsm;

-- ----------------------------------------------------------------------------
--                      Architecture declaration
-- ----------------------------------------------------------------------------
architecture behavioral of fsm is
   type t_state is (TEST1, TEST11, TEST111, TEST1a, TEST1b,TEST1c,TEST1d,TEST1e,TEST1f,TEST1g,TEST1h,TEST1i,TEST1j,TEST2a, TEST2b,TEST2c,TEST2d,TEST2e,TEST2f,TEST2g,TEST2h, GOOD_PRINT, BAD_PRINT, UGLY, FINISH);
   signal present_state, next_state : t_state;

begin
-- -------------------------------------------------------
sync_logic : process(RESET, CLK)
begin
   if (RESET = '1') then
      present_state <= TEST1;
   elsif (CLK'event AND CLK = '1') then
      present_state <= next_state;
   end if;
end process sync_logic;

-- -------------------------------------------------------
next_state_logic : process(present_state, KEY, CNT_OF)
begin
   case (present_state) is
   -- ------------------------------------------------
	
   when TEST1 =>
      next_state <= TEST1;
		if (KEY(3) = '1') then
			next_state <= TEST11;	
		elsif (KEY(15) = '1') then
         next_state <= BAD_PRINT;	
		elsif (KEY(14 downto 0) /= "000000000000000") then
			next_state <= UGLY;
		end if;
   -- ------------------------------------------------
	
	when TEST11 =>
		next_state <= TEST11;
		if (KEY(2) = '1') then
			next_state <= TEST111;
		elsif (KEY(15) = '1') then
         next_state <= BAD_PRINT;
		elsif (KEY(14 downto 0) /= "000000000000000") then
			next_state <= UGLY;
		end if;	
  -- ------------------------------------------------
  
	when TEST111 =>
		next_state <= TEST111;
		if (KEY(0) = '1') then
			next_state <= TEST1a;
		elsif (KEY(1) ='1') then
			next_state <= TEST2a;
		elsif (KEY(15) = '1') then
         next_state <= BAD_PRINT; 
		elsif
		 (KEY(14 downto 0) /= "000000000000000") then
			next_state <= UGLY;	
		end if;		
	-- ------------------------------------------------
	
	when TEST1a =>
		next_state <= TEST1a;
		if (KEY(8) = '1') then
			next_state <= TEST1b;
		elsif (KEY(15) = '1') then
         next_state <= BAD_PRINT;
		elsif (KEY(14 downto 0) /= "000000000000000") then
			next_state <= UGLY;
		end if;		
	-- ------------------------------------------------
	
	when TEST1b =>
		next_state <= TEST1b;
		if (KEY(7) = '1') then
			next_state <= TEST1c;
		elsif (KEY(15) = '1') then
         next_state <= BAD_PRINT;
		elsif (KEY(14 downto 0) /= "000000000000000") then
			next_state <= UGLY;
		end if;		
	-- ------------------------------------------------
	
	when TEST1c =>
		next_state <= TEST1c;
		if (KEY(0) = '1') then
			next_state <= TEST1d;
		elsif (KEY(15) = '1') then
         next_state <= BAD_PRINT;
		elsif (KEY(14 downto 0) /= "000000000000000") then
			next_state <= UGLY;
		end if;	
	-- ------------------------------------------------
	
	when TEST1d =>
		next_state <= TEST1d;
		if (KEY(4) = '1') then
			next_state <= TEST1e;
		elsif (KEY(15) = '1') then
         next_state <= BAD_PRINT;
		elsif (KEY(14 downto 0) /= "000000000000000") then
			next_state <= UGLY;
		end if;		
	-- ------------------------------------------------
	
	when TEST1e =>
		next_state <= TEST1e;
		if (KEY(3) = '1') then
			next_state <= TEST1f;
		elsif (KEY(15) = '1') then
         next_state <= BAD_PRINT;
		elsif (KEY(14 downto 0) /= "000000000000000") then
			next_state <= UGLY;
		end if;		
	-- ------------------------------------------------
	
	when TEST1f =>
		next_state <= TEST1f;
		if (KEY(1) = '1') then
			next_state <= TEST1g;
		elsif (KEY(15) = '1') then
         next_state <= BAD_PRINT;
		elsif (KEY(14 downto 0) /= "000000000000000") then
			next_state <= UGLY;
		end if;		
	-- ------------------------------------------------
	
	when TEST1g =>
		next_state <= TEST1g;
		if (KEY(3) = '1') then
			next_state <= TEST1h;
		elsif (KEY(15) = '1') then
         next_state <= BAD_PRINT;
		elsif (KEY(14 downto 0) /= "000000000000000") then
			next_state <= UGLY;
		end if;		
	-- ------------------------------------------------
	
	when TEST1h =>
		next_state <= TEST1h;
		if (KEY(6) = '1') then
			next_state <= TEST1i;
		elsif (KEY(15) = '1') then
         next_state <= BAD_PRINT;
		elsif (KEY(14 downto 0) /= "000000000000000") then
			next_state <= UGLY;
		end if;		
	-- ------------------------------------------------
	
	when TEST1i =>
		next_state <= TEST1i;
		if (KEY(15) = '1') then
         next_state <= GOOD_PRINT;
		elsif (KEY(14 downto 0) /= "000000000000000") then
			next_state <= UGLY;
		end if;		
	-- ------------------------------------------------
	-- ------------------------------------------------
	
	when TEST2a =>
		next_state <= TEST2a;
		if (KEY(7) = '1') then
			next_state <= TEST2b;
		elsif (KEY(15) = '1') then
         next_state <= BAD_PRINT;
		elsif (KEY(14 downto 0) /= "000000000000000") then
			next_state <= UGLY;
		end if;		
	-- ------------------------------------------------
	
	when TEST2b =>
		next_state <= TEST2b;
		if (KEY(4) = '1') then
			next_state <= TEST2c;
		elsif (KEY(15) = '1') then
         next_state <= BAD_PRINT;
		elsif (KEY(14 downto 0) /= "000000000000000") then
			next_state <= UGLY;
		end if;		
	-- ------------------------------------------------
	
	when TEST2c =>
		next_state <= TEST2c;
		if (KEY(0) = '1') then
			next_state <= TEST2d;
		elsif (KEY(15) = '1') then
         next_state <= BAD_PRINT;
		elsif (KEY(14 downto 0) /= "000000000000000") then
			next_state <= UGLY;
		end if;	
	-- ------------------------------------------------
	
	when TEST2d =>
		next_state <= TEST2d;
		if (KEY(8) = '1') then
			next_state <= TEST2e;
		elsif (KEY(15) = '1') then
         next_state <= BAD_PRINT;
		elsif (KEY(14 downto 0) /= "000000000000000") then
			next_state <= UGLY;
		end if;		
	-- ------------------------------------------------
	
	when TEST2e =>
		next_state <= TEST2e;
		if (KEY(6) = '1') then
			next_state <= TEST2f;
		elsif (KEY(15) = '1') then
         next_state <= BAD_PRINT;
		elsif (KEY(14 downto 0) /= "000000000000000") then
			next_state <= UGLY;
		end if;		
	-- ------------------------------------------------
	
	when TEST2f =>
		next_state <= TEST2f;
		if (KEY(2) = '1') then
			next_state <= TEST2g;
		elsif (KEY(15) = '1') then
         next_state <= BAD_PRINT;
		elsif (KEY(14 downto 0) /= "000000000000000") then
			next_state <= UGLY;
		end if;		
	-- ------------------------------------------------
	
	when TEST2g =>
		next_state <= TEST2g;
		if (KEY(7) = '1') then
			next_state <= TEST2h;
		elsif (KEY(15) = '1') then
         next_state <= BAD_PRINT;
		elsif (KEY(14 downto 0) /= "000000000000000") then
			next_state <= UGLY;
		end if;		
	-- ------------------------------------------------
	
	when TEST2h =>
		next_state <= TEST2h;
		if (KEY(15) = '1') then
         next_state <= GOOD_PRINT;
		elsif (KEY(14 downto 0) /= "000000000000000") then
			next_state <= UGLY;
		end if;		
	-- ------------------------------------------------
	-- ------------------------------------------------
	
	when GOOD_PRINT =>
      next_state <= GOOD_PRINT;
      if (CNT_OF = '1') then
         next_state <= FINISH;
      end if;
   -- ------------------------------------------------
	
   when BAD_PRINT =>
      next_state <= BAD_PRINT;
      if (CNT_OF = '1') then
         next_state <= FINISH;
      end if;
   -- ------------------------------------------------
	
	when UGLY =>
      next_state <= UGLY;
		if (KEY(15) = '1') then
         next_state <= BAD_PRINT;
      end if;
   -- ------------------------------------------------
	
   when FINISH =>
      next_state <= FINISH;
      if (KEY(15) = '1') then
         next_state <= TEST1; 
      end if;
   -- ------------------------------------------------
	
   when others =>
      next_state <= TEST1;
   end case;
end process next_state_logic;
   -- ------------------------------------------------
	
output_logic : process(present_state, KEY)
begin
   FSM_CNT_CE     <= '0';
   FSM_MX_MEM     <= '0';
   FSM_MX_LCD     <= '0';
   FSM_LCD_WR     <= '0';
   FSM_LCD_CLR    <= '0';

   case (present_state) is
	
	when GOOD_PRINT =>
      FSM_CNT_CE     <= '1';
		FSM_MX_MEM 		<= '1'; -- good message
      FSM_MX_LCD     <= '1'; -- print 
      FSM_LCD_WR     <= '1'; -- on display
   -- ------------------------------------------------		 
   when BAD_PRINT =>
      FSM_CNT_CE     <= '1';
									  -- bad message
      FSM_MX_LCD     <= '1'; -- print 
      FSM_LCD_WR     <= '1'; -- on display
   -- ------------------------------------------------
   when FINISH =>
      if (KEY(15) = '1') then
         FSM_LCD_CLR    <= '1';
      end if;
   -- ------------------------------------------------
   when others =>
		  if (KEY(14 downto 0) /= "000000000000000") then
         FSM_LCD_WR     <= '1';
      end if;
      if (KEY(15) = '1') then
         FSM_LCD_CLR    <= '1';
      end if;
   end case;
end process output_logic;

end architecture behavioral;

