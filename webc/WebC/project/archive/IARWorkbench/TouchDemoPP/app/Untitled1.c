OS_INTERWORK int __low_level_init(void) {
      // start PLL if not used as CPU clock source
      if (_PLLSTAT & (1 << 25)) {
        _PLLCON  |= (1 << 1);
        _PLLFEED  = 0xAA;
        _PLLFEED  = 0x55;
      }
      _PLLCON     = 0;                         // Set PLL disabled and disconnected
      _PLLFEED    = 0xAA;
      _PLLFEED    = 0x55;
      _SCS        |= (1 << 4)                  // The frequency range of the main oscillator is 15 MHz to 24 MHz.
                  |  (1 << 5)                  // Main oscillator enable.
                    ;
      _CLKSRCSEL  = 1;                         // Selects the RTC oscillator as the PLL clock source
                                               // (Select internal RC oscillator 4 MHz as clock source)
    
      _PLLCFG     = OS_PLL_MULTIPLIER_VAL;     // Set to 11. MSEL, default = 35 with 4MHz clock input
      _PLLFEED    = 0xAA;
      _PLLFEED    = 0x55;
      _PLLCON     = 1;                         // Enable PLL, Disconnect
      _PLLFEED    = 0xAA;
      _PLLFEED    = 0x55;
      do {
      } while ((_PLLSTAT & (1 << 26)) == 0);   // Check PLOCK bit	
      _CPUCLKCFG  = 5;                         // Set CPU clock divider
      _USBCLKCFG  = 5;                         // Set USB clock divider
      _PCLKSEL0   = 0;
      _PCLKSEL1   = 0;
      _PLLCON     = (1 << 0) | (1 << 1);       // Enable PLL and select as clock source
      _PLLFEED    =  0xAA;
      _PLLFEED    =  0x55;

};

main
  OS_InitHW(void)
      // Init clock
          // 1. Init OSC
          SCS_bit.OSCRANGE = 0;
          SCS_bit.OSCEN = 1;
          // 2.  Wait for OSC ready
          while(!SCS_bit.OSCSTAT);
          // 3. Disconnect PLL
          PLLCON_bit.PLLC = 0;
          PLLFEED = 0xAA;
          PLLFEED = 0x55;
          // 4. Disable PLL
          PLLCON_bit.PLLE = 0;
          PLLFEED = 0xAA;
          PLLFEED = 0x55;
          // 5. Select source clock for PLL
          CLKSRCSEL_bit.CLKSRC = 1; // Selects the main oscillator as a PLL clock source.
          // 6. Set PLL settings 288 MHz
          PLLCFG_bit.MSEL = 24-1;
          PLLCFG_bit.NSEL = 2-1;
          PLLFEED = 0xAA;
          PLLFEED = 0x55;
          // 7. Enable PLL
          PLLCON_bit.PLLE = 1;
          PLLFEED = 0xAA;
          PLLFEED = 0x55;
          // 8. Wait for the PLL to achieve lock
          while(!PLLSTAT_bit.PLOCK);
          // 9. Set clk divider settings
          CCLKCFG   = 4-1;            // 1/4 Fpll - 72 MHz
          USBCLKCFG = 6-1;            // 1/6 Fpll - 48 MHz
          PCLKSEL0 = PCLKSEL1 = 0;    // other peripherals - 18MHz (Fcclk/4)
          // 10. Connect the PLL
          PLLCON_bit.PLLC = 1;
          PLLFEED = 0xAA;
          PLLFEED = 0x55;
        
          // stop all Peripherals' clocks
          PCONP = 0;

