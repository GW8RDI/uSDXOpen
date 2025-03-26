//  QCX-SSB.ino - https://github.com/threeme3/QCX-SSB
//
//  Copyright 2019, 2020, 2021   Guido PE1NNZ <pe1nnz@amsat.org>
//  Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions: The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software. THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

//  Copyright all additions 2022-2023 Rob Colclough GW8RDI, use of the additions and changes is permitted for all private, non-commecial use at user´s risk.  No responsibility is accepted for any losses that may occur through the use of this modified code.

// THIS CODE SUPERCEEDS VERSIONS 1.02X AND 1.03. 1.03 HAS UPDATES FOR OLED CHPSETS BUT DOES NOT HAVE FUNCTIONAL OR DSP CHANGES COMPARED TO 1.02w.

// GW8RDI IMPORTANT NOTES: ***   DO NOT RUSH - READ THE NOTES BELOW SEVERAL TIMES!

// *** THIS OPEN SOFTWARE IS FULLY SUPPORTED* FREE OF COST BY GW8RDI and others ***

/*
Comfigured for (tr)usdx clone:

Compile results 17 April 2023
"C:\\Users\\User\\AppData\\Local\\Arduino15\\packages\\arduino\\tools\\avr-gcc\\7.3.0-atmel3.6.1-arduino7/bin/avr-size" -A "C:\\Users\\Usuario\\AppData\\Local\\Temp\\arduino\\sketches\\E2EA2B2706C6565E78832871CCAA7296/usdx.ino.elf"
Sketch uses 32244 bytes (99%) of program storage space. Maximum is 32256 bytes.
Global variables use 1499 bytes (73%) of dynamic memory, leaving 549 bytes for local variables. Maximum is 2048 bytes.
*/

// *** ISP DATA CORRUPTION WARNING1!!   ALWAYS REMOVE C24 (C27?) (or as marked) it's a 10nF on the ISP HEADER'S MOSI line (PA ctrl out) (PB3) of the ISP header,
//  and disconnect the internal mic by plugging in a disconnected Jack plug.  FAILING TO DO THIS CAN CORRUPT YOUR MCU CHIP!!

// *** CHECK WITH ME FIRST PLEASE ***

// PLEASE DO NOT PUBLISH WHAT YOU THINK ARE FAULTS WITH THIS RELEASE *** BEFORE CHECKING WITH ME, GW8RDI ***.
// 99% OF REPORTED PROBLEMS ARE JUST CONFIGURATION RELATED, NOT BUGS OR LIMITATIONS.
// SO IF YOU USE THIS SOFTWARE, YOU AGREE NOT TO PUBLISH UNTIL YOU´VE CONTACTED ME, GW8RDI
// THIS AVOIDS CONFUSIONS.

// Supports up to 9 bands (using 8 filters, 15 and 17M share). To increase change N_BANDS, and add frequencies to array "band[N_BANDS] ="
// SSB TX quality tests, see below MORE_MIC_GAIN, QUAD

/*  WELCOME TO THE OPEN SOURCE USDX PROJECT 2022 AND ONWARDS!

	This version is for all uSDX/uSDR transceivers, release numbers are 4.XXx.

	Compiled and programmed using Arudino IDE 2.0.4, available from www.arduinio.cc

	To support the add-on module for uSDX with powerful DSP processor, version numbers will be 3.XXx.

	*** READ ALL THE NOTES here and ask questions on the FB "USDX USDR" group before programming, it may save a lot of heartache!
	Our intention is to make the uSDX a great option, with all the facilities and interfaces you can imagine!
	The quadrature mixer (designed by Dan Tayloe N7VE) in the uSDX makes it as good a receiver as a professional rig, when built correctly.
	The TX modulation has been improved and gets good reports, and with the add-on module it can be as good as the best out there.
	Enjoy!
	73 Rob, GW8RDI
*/

//  G8RDI Modifications log:
#define VERSION   "4.00c"    // Fixed format "9.99z" : Additions and changes Copyright 2022-2023 GW8RDI - You can use and distribute if you maintain the copyright message, commercial use is prohibited.

//  2022/03/04 - Added delay to show serial number at start - G8RDI mod
//               Added band change direction based on last freq step directions. See "case BE | DC:" - GW8RDI mod
//               Set PB3/PB5 to output in init to drive LCD backlight
//  2022/03/05 - Release 1.02wA2 2022/03/20 : Added Cat 8.6 ad QUAD enable 8.7 menu items
//  2022/03/06 - With MORE_MIC_GAIN enabled & QUAD disabled, SSB TX voice is sounding much better!
//  2022/04/24 - Release 1.02wA3: Fixed band dir. bug
//  2022/05/08 - Added KEEP_BAND_DATA to maintain last freq and mode set on each band.
//  2022/05/09 - Release 1.02wA4 : Maintains last freq and mode set for each band, up to 9 bands set. Added error code display.
//  2022/05/11 - Changed menu to cycle end-start, start-end
//  2022/07/19 - Added full CW mode update on band change, minor fixes
//  2022/07/19 - Release 1.02wA5 : Maintains last freq and mode set for each band, up to 9 bands set. Added error code display.
//  2022/07/28 - Release 1.02wA6 : Minor changes. Reduced CW message duplication to release more memory.
//  2022/08/16 - Release 1.02wA7 : Directional band-change went in the wrong direction if SWAP_ROTARY was not enabled. This fix solves that issue.
//  2022/08/19 - Release 1.02wA8 : Directional band-change saved last band-mode to Flash, and not mode of band changed to, causing incorrect mode restore if shutdown without band change.  Minor other changes, DEBUG modes changes callsign.
//  2022/08/22 - Release 1.02wA8a : Red Corners Unit: CW messages updated, Band menu fix as was showing 6m in the list. Added WHITE_CORNERS to config list.
//  2022/08/22 - Release 2.00a : Jumped version to avoid confusion with prev. releases. Added SWR, 115200 baud CAT, experimental changes to AM and FM, and minor tidy up
//  2022/10/12 - Release 2.00b : NR stays as last used.
//  2022/11/06 - Release 2.00c : #define KEYER CW keyer for Iambic -> NOTE: Auto CW msg sending aborts if not installed, and 700Hz filter selection gives reduced gain.
//  : Added new post mag IQ filter, added BlackBrick config.
//  2023/02/09 - Release 2.00d : Added CAT freq. error handling, in RIT mode the TX freq. is now displayed, plus FIR noise filter, see #define NR_FIR below. NR 0-2 is 1.02x method, 3-8 is now FIR DSP filter method to be used with Full b/w and Att2 = 2
//  2023/02/09 - Release 2.00e : Ammended code to support +/- 99 KHz RIT receiver offset, and added CAT command to set the RIT offset.
//  2023/03/14 - Release 2.00f : Ammended code with new "TT" CAT command to set a TX offset frequency; see CAT_XO_CMD. Minor improvements for setting mode display, etc.
//  2023/03/14 - Update to 2.00f : Ammended code as missing #ifdef KEEP_BAND_DATA on line 5922
//  2023/04/01 - Release 4.00a : Changed to version 4.00 as DL2MAN jumped his version up to 2.00!  CAT now enables directly from the menu without needing a reboot to activate!  CAT MDx; now refreshes LCD fully after CW mode.
//                               Re-coded KEEP_BAND_DATA switch statements which freed 328 bytes!!
//  2023/04/11 - Release 4.00b : Fixed CAT mode change to allow it to select AM and FM, AM important to correctly track IQ flip. Also commented-out redundant BE | DC for mode change, as now used for directional band change.
//                               RIT mode now allows mode to be changed.
//  2023/04/17 - Release 4.00c : (UPDATE-2) Ammended configuration for TRUSDX clone so that latched-relay band switching and SWR selection is included.

//  : Added new post mag IQ filter, added BlackBrick config.
//  : todo see "// xyzzy Test with i_d"

// NOTE update #define VERSION "????" above!

// See "BACKLOG:" FOR PENDING WORK CARRIED FORWARD

// *** Use of this modified software is at the users risk ***  PLEASE READ THE INSTRUCTIONS AVAILABLE IN THE FB GROUP "uSDX uSDR Radios" or uSDX Group IO online.

// Notes: To have CW tone in the menu, enable #define FILTER_700HZ. You´ll need to find 28 bytes depending on your config, i.e. disabling #define DIAG

// Configuration switches; remove/add a double-slash at line-start to enable/disable a feature; to save space disable e.g. CAT, DIAG, KEYER

// *** BEFORE ALL, READ THIS BLOCK !!!!!!!!

/* THIS CONFIG: / To change, ADD or DELETE the "//" in front of #define lines below
		XXBLACK BRICK UNIT, FRONT PANEL ON LARGER SIDE: 25.0, SWR, SWAP-ROTARY, BACKLIGHT_PIN 0x08
		RED BUTTONS UNIT 27001400 (CHANGE BELOW TO SET YOUR OFFSET OR SET 27000000), NO SWR, SWAP-ROTARY
		XXWHITE BUTTONS UNIT 27000000 (CHANGE BELOW TO SET YOUR OFFSET OR SET 27000000), NO SWR, SWAP-ROTARY - NEEDS CHECKING TODO
		XXRED CORNERS UNIT 25000000 SWR, SWAP-ROTARY
		XXTRUESDX UNIT - PLEASE CONTACT GW8RDI TO CHECK YOUR CONFIGURATION BEFORE PROGAMMING AS WE ARE IN TESTING PHASE.
*/
// NOTE: ONLY ENABLE ONE OF THE MODELS BELOW BY ADDING OR REMOVING THE UNCOMMENT "//"
//________________________________________________________________________________________________________________________
//#define BLACK_BRICK 1   // Backlight control PortD is PD3 0x08, SWR, NO ROTARY SWAP

//#define RED_CORNERS 1 // Backlight control PortD is PD3 0x08, or PD5 0x20 for Red Corners rig. Disable for Red and White buttons and most black brick uSDX.

// !!!!! ALWAYS DISABLE LINE BELOW !!!!!!
///#define MY_RED_CORNERS 1	// Only for my(GW8RDI) Red Corners with reversed rotary part!
// !!!!! ALWAYS DISABLE LINE ABOVE !!!!!!

//#define RED_BUTTONS 1 // Used for Small HF SDR TRANSCEIVER uSDX model, without SWR circuit.  May have SMD inductors.

#define WHITE_BUTTONS 1 // Small black unit with white or red buttons on front, without SWR circuit.

//#define TRUSDX 1 // Small USDX clone in 3D printed case marked "DL2MAN & PE1NNZ".  CHECK WITH GW8RDI BEFORE USING THIS FOR UPDATES AND CONFIG DETAILS!  SWR protection via PA sensing resistor can be added if needed.
// NOTE:  DL2MAN claims (as of 17 April 2023) that his license blocks users from installing other software (Microsoft vs Linux et al), but it is understood that this violates consumer rights laws in the USA, UK and European Union.

// *** NOTE ***: If none of the above are enabled, configuration may match other units, but if tuning direction is reversed, backlight or frequency wrong, adjust as needed.
// IF IN DOUBT PLEASE ASK ME FIRST: GW8RDI
//________________________________________________________________________________________________________________________

#if defined(RED_CORNERS) || defined(BLACK_BRICK)
#define BACKLIGHT_PIN 0x20
#else
#define BACKLIGHT_PIN 0x08
#endif

#ifndef TRUSDX
#define KEEP_BAND_DATA 1        // Maintain last freq and mode set on each band - GW8RDI mod
#endif

#define SHOW_USB_LSB_CW_ONLY 1  // If defined, Menu will only cycle thro these 3 modes

// AM & FM Modulation changes
//#define FM_ARCTAN 1         // Enable FM differentiator TEST - GW8RDI mod
//#define AM_MOD_MAGN_SQRT 1  // Use more accurate SQRT method

//****************************************************************
//#define DEBUG_G8RDI 1   // Enables display of error codes on LCD
//****************************************************************

// Change Callsign from G8RDI to match your own, or set it to "uSDR+  " if you don't want it customised:-

// *** CALLSIGN NO MORE THAN 5 CHARACTERS!!! DON'T REMOVE THE 2 SPACES!!!  IGNORE THIS WARNING IT WILL CRASH THE PROGRAM!!! ***
#ifdef DEBUG_G8RDI
#define MY_CALLSIGN_PADDED "DEBUG  "
#else
// Put your callsigne below and remove the "///" in front to activate.
///#define MY_CALLSIGN "G8RDI"               // <----- Add your callsign here or enable line below, replacing G8RDI!  If you don´t want the LCD to show your callsign, enable the line "uSDR+" below.

///#define MY_CALLSIGN_PADDED "G8RDI  "      // <----- Also add your callsign here BUT keep the 2 spaces at the end!
/// Disable below line if using your own callsign by adding // in front.
#define MY_CALLSIGN_PADDED "uSDR+  "    // Ensure two spaces at end of heading and that it is under 7 characters (including the 2 spaces), or this program may not work correctly.

//#define MY_PREFIX ""  // No prefix, use this line by removing the //, add below to replica line.
#define MY_PREFIX ""    // Add visiting country prefix here

#define MY_NAME "ROB"   // <---- *** ADD YOUR NAME HERE FOR CW MESSAGES
#endif
#define CALLSIGN_LENGTH 5       // Change length to match your callsign but remember the LCD isn't very wide!

// *** MEMORY LIMITATION OF ATMEGA328 *** This means you may have to mix and match functions option defines.  CAT requires considerable memory, so use only if needed.

// If your dial goes the wrong way, change SWAP_ROTARY
#if defined(RED_CORNERS) || defined(BLACK_BRICK)
// SWAP_ROTARY is isually required for Red Corners unless Rotary type changed, like mine!
#ifdef MY_RED_CORNERS
#define REVERSE_BAND_CHANGE	1	//If your freq. change is correct, but band jump goes backwards, define REVERSE_BAND_CHANGE
#else
#define SWAP_ROTARY    1   // Swap rotary direction (enable for WB2CBA-uSDX)  NOTE:  To enable SWAP without RED_CORNERS enabled, comment out the lines above and below with // character, i.e. //#ifdef RED_CORNERS and //#endif
//#define REVERSE_BAND_CHANGE	1	//If your freq. change is correct, but band jump goes backwards, define REVERSE_BAND_CHANGE
#endif
// :( No space for SWR with both CW Msgs and CAT
#define SWR_METER      1   // Supports SWR meter with bridge on A6/A7 (LQPF ATMEGA328P) by Alain, K1FM, see: https://groups.io/g/ucx/message/6262 and https://groups.io/g/ucx/message/6361
#endif

//***************** TRUSDX FEATURES
#if defined(TRUSDX)

//#define LCD_I2C        1   // LCD with I2C (PCF8574 module          ), connect SDA (PD2), SCL (PD3), NOTE that this display is pretty slow

#define OLED_SSD1306     1   // OLED display (SSD1306 128x32 or 128x64), connect SDA (PD2), SCL (PD3)
//#define OLED_SH1106    1   // OLED display (SH1106 1.3" inch display), connect SDA (PD2), SCL (PD3), NOTE that this display is pretty slow

#define SWR_METER        1   // Supports SWR meter with bridge on A6/A7 (LQPF ATMEGA328P) by Alain, K1FM, see: https://groups.io/g/ucx/message/6262 and https://groups.io/g/ucx/message/6361

#endif
//*****************

#define LPF_SWITCHING_DL2MAN_USDX_REV3 1    // Default
//#define LPF_SWITCHING_DL2MAN_USDX_REV3_NOLATCH 1    // NOTE: CHANGE IF THIS VERSION LATCHES

#if defined(BLACK_BRICK)
#define SWR_METER      1   // Supports SWR meter with bridge on A6/A7 (LQPF ATMEGA328P) by Alain, K1FM, see: https://groups.io/g/ucx/message/6262 and https://groups.io/g/ucx/message/6361
#endif

//#define FAST_AGC         1   // Adds fast AGC option (good for CW) Slow mode not recommended.  Remove for CAT if memory errors.

#define CAT              1   // CAT-interface - OTHER OPTIONS, SUCH AS CW_MESSAGES and KEEP_BAND_DATA MAY TO BE DISABLED TO MAKE SPACE FOR CAT
//#define CAT_EXT        1   // Extended CAT support: remote button and screen control commands over CAT
//#define CAT_STREAMING    1   // Streams audio and IQ, only 8KHz b/w, & needs faster 115200 baud RS232
#define CAT_FAST         1   // Uses faster 115200 baud (can be changed to 57600), else 38400, 8, 1, N.

// If short of memory on compile and not using Spectrum display, disable CAT_XO_CMD:- Like this:-> //#define CAT_XO_CMD
#ifdef CAT
#define CAT_TX_CMD          1  // GW8RDI mod - added - Send TX and RX status CAT cmds as PTT is pressed and released
#ifndef TRUSDX
#define CAT_XO_CMD          1  // GW8RDI mod - added - Set TX offset freq. for Quantum Spectrum module from QuantumSDR.com
// Note: to use CAT_XO_CMD, RIT_ENABLE must also be enabled.
#endif
#endif

// Lines below NEEDED FOR CW, removed to make space for CAT
//#define KEYER            1   // CW keyer for Iambic - NOTE: Auto CW msg sending aborts if not installed as changes dit timing. Can be removed to save memory for CAT
//#define KEY_CLICK        1   // G8RDI mod - may be removed to free memory for CAT - NEEDED FOR CW msg sending else CW TX sounds mushy & CW msg sending stops after one peep! // Reduce key clicks by envelope shaping
//#define FILTER_700HZ   1    // G8RDI mod - Moved here - Enabled shows in Menu

// CW Messages: Note: If CAT is enabled, CW messages may cause a program memory overflow. KEEP_BAND_DATA can be disabled to release memory for CW at cost of losing band frequency memory.
//#define CW_MESSAGE 1          // Transmits pre-defined CW messages on-demand (left-click menu item 4.2)
//#define CW_MESSAGE_EXT 1      // Additional CW messages

// NOTE: DO NOT CHANGE THE CW_MESSAGE LINES BELOW AS THEY ARE INCORPORATED OR NOT BASED ON CW_MESSAGE and CW_MESSAGE_EXT above.
// Note: !!!Do not exceed CW_MESSAGE_LENGTH when ammending messages!!!
#ifdef CW_MESSAGE_EXT
#define CW_MESSAGE_LENGTH 48  //48/32
#else
#define CW_MESSAGE_LENGTH 48  //48/32/16
#endif

// CHANGE THE CW MESSAGE TEXT BELOW AS YOU LIKE BUT LESS THAN CW_MESSAGE_LENGTH+1 CHARS!  THE ## MEANS +, USED TO CONCATANTE STRINGS.
// DO NOT COMMENT OUT LINES BELOW, THESE TEXTS ARE NOT INCLUDED WHEN CW_MESSAGES/EXT ARE DISABLED
#define CW_STD_MSG "CQ CQ DE " MY_CALLSIGN " +"  // 16 chars, change in code of size changes.
#define CW_MSG1 "CQ CQ DE " MY_CALLSIGN " +"
#define CW_MSG2 "CQ CQ DE " MY_PREFIX MY_CALLSIGN " +"
#define CW_MSG3 MY_PREFIX MY_CALLSIGN
#define CW_MSG4 "GE TKS 5NN 5NN NAME IS " MY_NAME " HW?"
#define CW_MSG5 "FB RPTR TX 5W ANT EFW 73 CUAGN"
#define CW_MSG6 "73 GL TU EE"
// Examples:
//"CQ"  MY_CALLSIGN " +", "CQ CQ DE " MY_CALLSIGN + MY_CALLSIGN " +", "GE TKS 5NN 5NN NAME IS ROB ROB HW?", "FB RPTR TX 5W 5W ANT ENDFED 73 CUAGN", "73 TU E E", "G8RDI"
//"CQ CQ DE " MY_CALLSIGN " " MY_CALLSIGN " +", "GE TKS 5NN 5NN NAME IS " MY_NAME "" MY_NAME " HW?", "FB RPTR TX 5W 5W ANT ENDFED 73 CUAGN", "73 TU E E", MY_CALLSIGN
//#define CW_MSG2 '"CQ CQ DE " MY_CALLSIGN " +"'  // Remove/add your Area prefit
//#define CW_MSG3 MY_CALLSIGN

//#define NR_FIR 1  // GW8RDI mod. Usually this won´t fit with CAT, but removing other options, such as CW messages, etc., can make enough space

///G8RDI comment out FAST_AGC & DIAG below to save mem space for CAT
//#define DIAG             1   // Hardware diagnostics on startup (use to debug problems)

#ifndef CAT_XO_CMD  // Undefined CW_VOLUME to make space for CAT_XO_CMD
#define CW_VOLUME        1    // Enable separate CW tone volume in the menu
#endif

#define CW_DECODER       1   // CW decoder
//#define CW_INTERMEDIATE  1   // CW decoder shows intermediate characters (only available for LCD and F_MCU at 20M), sequences like:  EIS[HV] EIUF EAW[JP] EARL TMO TMG[ZQ] TND[BX] TNK[YC], may be good to learn CW; a full list of possible sequences:  EISH5 EISV3 EIUF EIUU2 EAWJ1 EAWP EARL TMOO0 TMOO9 TMOO8 TMGZ7 TMGQ TNDB6 TNDX TNKY TNKC
//#define CW_FREQS_QRP   1   // Defaults to CW QRP   frequencies when changing bands
//#define CW_FREQS_FISTS 1   // Defaults to CW FISTS frequencies when changing bands

// NOTE: Make sure you have the correct xtal frequency enabled. This is the xtal near the SI5351/SI3253 chip, not the one near the Atmega MCU.
//#define F_XTAL    27005000   // 27MHz SI5351 crystal
//#define F_XTAL  25004000   // 25MHz SI5351 crystal  (enable for WB2CBA-uSDX, SI5351 break-out board or uSDXDuO)
#if defined(RED_CORNERS) || defined(BLACK_BRICK)
//#ifdef RED_CORNERS
#define F_XTAL  25000000   // 25MHz SI5351 crystal  (enable for 25MHz TCXO)
#else
#ifdef MY_RED_CORNERS
#define F_XTAL  27001400
#else
#define F_XTAL  27000000   // !!!! SET YOUR EXACT XTAL FREQ OR 27000000 !!!!  27MHz usually on black bricks, Red Buttons (27001400 is my calibration offset!!!) and White buttons versions
#endif
#endif

// GW8RDI NOTE: Enable to have battery voltage shown on the LCD.
// GW8RDI WARNING!!! The problem with the original code is that it switches the ADC VREF up to 5V to read the bat, V, this causes some noise on the IQ sampling which enters the audio,
//  and can interfere with the reading of buttons which are sensed through an ADC.
// A better solution is to simply use a 2 resistor voltage divider and not change VREF, calculating the bar. voltage based on the res. divider ration. Alternatively, only show voltage in a menu function.  todo - change code.
//#define VSS_METER      1   // Supports Vss measurement (as s-meter option), requires resistor of 1M between 12V and pin 26 (PC3)

//#define QCX            1   // Supports older (non-SDR) QCX HW modifications (QCX, QCX-SSB, QCX-DSP with I/Q alignment-feature)
//#define OLED_SSD1306   1   // OLED display (SSD1306 128x32 or 128x64), connect SDA (PD2), SCL (PD3)
//#define OLED_SH1106    1   // OLED display (SH1106 1.3" inch display), connect SDA (PD2), SCL (PD3), NOTE that this display is pretty slow
//#define LCD_I2C        1   // LCD with I2C (PCF8574 module          ), connect SDA (PD2), SCL (PD3), NOTE that this display is pretty slow
//#define LPF_SWITCHING_DL2MAN_USDX_REV3           1   // Enable 8-band filter bank switching:     latching relays wired to a TCA/PCA9555 GPIO extender on the PC4/PC5 I2C bus; relays are using IO0.0 as common (ground), IO1.0..7 used by the individual latches K0-7 switching respectively LPFs for 10m, 15m, 17m, 20m, 30m, 40m, 60m, 80m
//#define LPF_SWITCHING_DL2MAN_USDX_REV3_NOLATCH 1   // Enable 8-band filter bank switching: non-latching relays wired to a TCA/PCA9555 GPIO extender on the PC4/PC5 I2C bus; relays are using IO0.0 as common (ground), IO1.0..7 used by the individual latches K0-7 switching respectively LPFs for 10m, 15m, 17m, 20m, 30m, 40m, 60m, 80m. Enable this if you are using 8-band non-latching version for the relays, the radio will draw extra 15mA current but will work ity any relay (Tnx OH2UDS/TA7W Baris)
//#define LPF_SWITCHING_DL2MAN_USDX_REV2         1   // Enable 5-band filter bank switching:     latching relays wired to a TCA/PCA9555 GPIO extender on the PC4/PC5 I2C bus; relays are using IO0.1 as common (ground), IO0.3, IO0.5, IO0.7, IO1.1, IO1.3 used by the individual latches K1-5 switching respectively LPFs for 20m, 30m, 40m, 60m, 80m
//#define LPF_SWITCHING_DL2MAN_USDX_REV2_BETA    1   // Enable 5-band filter bank switching:     latching relays wired to a PCA9539PW   GPIO extender on the PC4/PC5 I2C bus; relays are using IO0.1 as common (ground), IO0.3, IO0.5, IO0.7, IO1.1, IO1.3 used by the individual latches K1-5 switching respectively LPFs for 20m, 30m, 40m, 60m, 80m
//#define LPF_SWITCHING_DL2MAN_USDX_REV1         1   // Enable 3-band filter bank switching:     latching relays wired to a PCA9536D    GPIO extender on the PC4/PC5 I2C bus; relays are using IO0 as common (ground), IO1-IO3 used by the individual latches K1-3 switching respectively LPFs for 20m, 40m, 80m
//#define LPF_SWITCHING_WB2CBA_USDX_OCTOBAND     1   // Enable 8-band filter bank switching: non-latching relays wired to a MCP23008    GPIO extender on the PC4/PC5 I2C bus; relays are using GND as common (ground), GP0..7 used by the individual latches K1-8 switching respectively LPFs for 80m, 60m, 40m, 30m, 20m, 17m, 15m, 10m
//#define LPF_SWITCHING_PE1DDA_USDXDUO           14  // Enable 2-band filter bank switching: non-latching relay  wired to pin PD5 (pin 11); specify as value the frequency in MHz for which (and above) the relay should be altered (e.g. put 14 to enable the relay at 14MHz and above to use the 20m LPF).
#define SI5351_ADDR   0x60   // SI5351A I2C address: 0x60 for SI5351A-B-GT, Si5351A-B04771-GT, MS5351M; 0x62 for SI5351A-B-04486-GT; 0x6F for SI5351A-B02075-GT; see here for other variants: https://www.silabs.com/TimingUtility/timing-download-document.aspx?OPN=Si5351A-B02075-GT&OPNRevision=0&FileType=PublicAddendum
//#define F_MCU   16000000   // 16MHz ATMEGA328P crystal (enable for unmodified Arduino Uno/Nano boards with 16MHz crystal). You may change this value to any other crystal frequency (up to 28MHz may work)

// Advanced configuration switches
//#define CONDENSED      1   // Display in 4 line mode (for OLED and LCD2004 modules)
#define TX_ENABLE        1   // Disable this for RX only (no transmit), e.g. to support uSDX for kids idea: https://groups.io/g/ucx/topic/81030243#6276
#define SEMI_QSK         1   // Just after keying the transmitter, keeps the RX muted for a short amount of time in the anticipation for continued keying
#define RIT_ENABLE       1   // Receive-In-Transit alternates the receiving frequency with an user-defined offset to compensate for any necessary tuning needed on receive
#define VOX_ENABLE       1   // Voice-On-Xmit which is switching the transceiver into transmit as soon audio is detected (above noise gate level)
//#define MOX_ENABLE     1   // Monitor-On-Xmit which is audio monitoring on speaker during transmit

//#define ONEBUTTON      1   // Use single (encoder) button to control full the rig; optionally use L/R buttons to completely replace rotory encoder function
//#define DEBUG          1   // for development purposes only (adds debugging features such as CPU, sample-rate measurement, additional parameters)
//#define TESTBENCH      1   // Tests RX chain by injection of sine wave, measurements results are sent over serial

// G8RDI removed for memory due to CAT  
//#define TX_DELAY       1   // Enables a delay in the actual transmission to allow relay-switching to be completed before the power is applied (see also NTX, PTX definitions below for GPIO that can switch relay/PA)
//#define NTX            11  // Enables LOW  on TX, used as PTT out to enable external PAs (a value of 11 means PB3 is used)
#define PTX            11  // Enables HIGH on TX, used as PTT out to enable external PAs (a value of 11 means PB3 is used)
//#define CLOCK          1   // Enables clock
// G8RDI removed to save memory #define:
//#define F_XTAL  20000000   // Enable this for uSDXDuO, 20MHz SI5351 crystal
//#define TX_CLK0_CLK1   1   // Enable this for uSDXDuO, i.e. when PA is driven by CLK0, CLK1 (not CLK2); NTX pin may be used for enabling the TX path (this is like RX pin, except that RX may also be used as attenuator)
//#define F_CLK2  12000000   // Enables a fixed CLK2 clock output of choice (only applicable when TX_CLK0_CLK1 is enabled), e.g. for up-converter or to clock UART USB device

// QCX pin defintions
#define LCD_D4  0         //PD0    (pin 2)
#define LCD_D5  1         //PD1    (pin 3)
#define LCD_D6  2         //PD2    (pin 4)
#define LCD_D7  3         //PD3    (pin 5)
#define LCD_EN  4         //PD4    (pin 6)
#define FREQCNT 5         //PD5    (pin 11)
#define ROT_A   6         //PD6    (pin 12)
#define ROT_B   7         //PD7    (pin 13)
#define RX      8         //PB0    (pin 14)
#define SIDETONE 9        //PB1    (pin 15)
#define KEY_OUT 10        //PB2    (pin 16)
#define SIG_OUT 11        //PB3    (pin 17)
#define DAH     12        //PB4    (pin 18)
#define DIT     13        //PB5    (pin 19)
#define AUDIO1  14        //PC0/A0 (pin 23)
#define AUDIO2  15        //PC1/A1 (pin 24)
#define DVM     16        //PC2/A2 (pin 25)
#define BUTTONS 17        //PC3/A3 (pin 26)
#define LCD_RS  18        //PC4    (pin 27)
#define SDA     18        //PC4    (pin 27)
#define SCL     19        //PC5    (pin 28)
//#define NTX   11        //PB3    (pin 17)
//#define PTX   11        //PB3    (pin 17)

#ifdef SWAP_ROTARY
#undef ROT_A
#undef ROT_B
#define ROT_A   7         //PD7    (pin 13)
#define ROT_B   6         //PD6    (pin 12)
#endif

#if (defined(OLED_SSD1306) || defined(OLED_SH1106))
#define OLED    1
#endif

#if (defined(CAT) || defined(TESTBENCH)) && !(OLED)
#define _SERIAL  1       // Coexistence support for serial port and LCD on the same pins
#endif

#ifdef LPF_SWITCHING_DL2MAN_USDX_REV3_NOLATCH
#define LPF_SWITCHING_DL2MAN_USDX_REV3         1
#endif

#ifdef TX_CLK0_CLK1
#ifdef F_CLK2
#define TX1RX0  0b11111000
#define TX1RX1  0b11111000
#define TX0RX1  0b11111000
#define TX0RX0  0b11111011
#else //!F_CLK2
#define TX1RX0  0b11111100
#define TX1RX1  0b11111100
#define TX0RX1  0b11111100
#define TX0RX0  0b11111111
#endif //F_CLK2
#else  //!TX_CLK0_CLK1
#define TX1RX0  0b11111011
#define TX1RX1  0b11111000
#define TX0RX1  0b11111100
#define TX0RX0  0b11111111
#endif //TX_CLK0_CLK1

#if defined(F_CLK2) && !defined(TX_CLK0_CLK1)
#error "TX_CLK0_CLK1 must be enabled in order to use F_CLK2."
#endif

#ifndef TX_ENABLE
#undef KEYER
#undef TX_DELAY
#undef SEMI_QSK
#undef RIT_ENABLE
#undef VOX_ENABLE
#undef MOX_ENABLE
#endif //!TX_ENABLE

#ifdef SWR_METER
float FWD;
float SWR;
float ref_V = 5 * 1.15;
static uint32_t stimer;
#define PIN_FWD  A6
#define PIN_REF  A7
#endif

/*
// UCX installation: On blank chip, use (standard Arduino Uno) fuse settings (E:FD, H:DE, L:FF), and use customized Optiboot bootloader for 20MHz clock, then upload via serial interface (with RX, TX and DTR lines connected to pin 1, 2, 3 respectively)
// UCX pin defintions
+#define SDA     3         //PD3    (pin 5)
+#define SCL     4         //PD4    (pin 6)
+#define ROT_A   6         //PD6    (pin 12)
+#define ROT_B   7         //PD7    (pin 13)
+#define RX      8         //PB0    (pin 14)
+#define SIDETONE 9        //PB1    (pin 15)
+#define KEY_OUT 10        //PB2    (pin 16)
+#define NTX     11        //PB3    (pin 17)
+#define DAH     12        //PB4    (pin 18)
+#define DIT     13        //PB5    (pin 19)
+#define AUDIO1  14        //PC0/A0 (pin 23)
+#define AUDIO2  15        //PC1/A1 (pin 24)
+#define DVM     16        //PC2/A2 (pin 25)
+#define BUTTONS 17        //PC3/A3 (pin 26)
// In addition set:
#define OLED  1
#define ONEBUTTON  1
#define ONEBUTTON_INV 1
#undef DEBUG
adjust I2C and I2C_ ports,
ssb_cap=1; dsp_cap=2;
#define _DELAY() for(uint8_t i = 0; i != 5; i++) asm("nop");
#define F_XTAL 20004000
#define F_CPU F_XTAL
*/

//FUSES = { .low = 0xFF, .high = 0xD6, .extended = 0xFD };   // Fuse settings should be set at programming (Arduino IDE > Tools > Burn bootloader)

#if(ARDUINO < 10810)
#if (ARDUINO != 10607)  // G8RDI mod - IDE 2.0.1 has its version set to 10607, an IDE bug since fixed.
# error "Unsupported Arduino IDE version, use Arduino IDE 1.8.10 or later from https://www.arduino.cc/en/software"
#endif
#endif

#if !(defined(ARDUINO_ARCH_AVR))
#error "Unsupported architecture, select Arduino IDE > Tools > Board > Arduino AVR Boards > Arduino Uno."
#endif
#if(F_CPU != 16000000)
#error "Unsupported clock frequency, Arduino IDE must specify 16MHz clock; alternate crystal frequencies may be specified with F_MCU."
#endif
#undef F_CPU
#define F_CPU 20007000  // Actual crystal frequency of 20MHz XTAL1, note that this declaration is just informative and does not correct the timing in Arduino functions like delay(); hence a 1.25 factor needs to be added for correction.
#ifndef F_MCU
#define F_MCU 20000000  // 20MHz ATMEGA328P crystal
#endif

extern char __bss_end;
static int freeMemory() { char* sp = reinterpret_cast<char*>(SP); return sp - &__bss_end; }  // see: http://www.nongnu.org/avr-libc/user-manual/malloc.html

#ifdef CAT_EXT
volatile uint8_t cat_key = 0;
uint8_t _digitalRead(uint8_t pin) {  // reads pin or (via CAT) artificially overriden pins
	serialEvent();  // allows CAT update
	if (cat_key) { return (pin == BUTTONS) ? ((cat_key & 0x07) > 0) : (pin == DIT) ? ~cat_key & 0x10 : (pin == DAH) ? ~cat_key & 0x20 : 0; } // overrides digitalRead(DIT, DAH, BUTTONS);
	return digitalRead(pin);
}
#else
#define _digitalRead(x) digitalRead(x)
#endif //CAT_EXT

//#define ONEBUTTON_INV 1 // Encoder button goes from PC3 to GND (instead PC3 to 5V, with 10k pull down)
#ifdef ONEBUTTON_INV
uint8_t inv = 1;
#else
uint8_t inv = 0;
#endif

//#ifdef KEYER
// Iambic Morse Code Keyer Sketch, Contribution by Uli, DL2DBG. Copyright (c) 2009 Steven T. Elliott Source: http://openqrp.org/?p=343,  Trimmed by Bill Bishop - wrb[at]wrbishop.com.  This library is free software; you can redistribute it and/or modify it under the terms of the GNU Lesser General Public License as published by the Free Software Foundation; either version 2.1 of the License, or (at your option) any later version. This library is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more details: Free Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA.

// keyerControl bit definitions
#define DIT_L    0x01     // Dit latch
#define DAH_L    0x02     // Dah latch
#define DIT_PROC 0x04     // Dit is being processed
#define PDLSWAP  0x08     // 0 for normal, 1 for swap
#define IAMBICB  0x10     // 0 for Iambic A, 1 for Iambic B
#define IAMBICA  0x00     // 0 for Iambic A, 1 for Iambic B
#define SINGLE   2        // Keyer Mode 0 1 -> Iambic2  2 ->SINGLE

int keyer_speed = 25;
static unsigned long ditTime;                    // No. milliseconds per dit
static uint8_t keyerControl;
static uint8_t keyerState;
static uint8_t keyer_mode = 2; //->  SINGLE
static uint8_t keyer_swap = 0; //->  DI/DAH

static uint32_t ktimer;
static int Key_state;
int debounce;

enum KSTYPE { IDLE, CHK_DIT, CHK_DAH, KEYED_PREP, KEYED, INTER_ELEMENT }; // State machine states

void update_PaddleLatch() // Latch dit and/or dah press, called by keyer routine
{
	if (_digitalRead(DIT) == LOW) {
		keyerControl |= keyer_swap ? DAH_L : DIT_L;
	}
	if (_digitalRead(DAH) == LOW) {
		keyerControl |= keyer_swap ? DIT_L : DAH_L;
	}
}

void loadWPM(int wpm) // Calculate new time constants based on wpm value
{
#if(F_MCU != 20000000)
	ditTime = (1200ULL * F_MCU / 16000000) / wpm;   //ditTime = 1200/wpm;  compensated for F_CPU clock (running in a 16MHz Arduino environment)
#else
	ditTime = (1200 * 5 / 4) / wpm;   //ditTime = 1200/wpm;  compensated for 20MHz clock (running in a 16MHz Arduino environment)
#endif
}
//#endif //KEYER
static uint8_t practice = false;  // Practice mode

static int8_t prev_mode;

volatile uint8_t cat_active = 0;  // Run-time set when serial data being processed to keep shared LCD pins controlled
volatile uint32_t rxend_event = 0;
volatile uint8_t vox = 0;

#include <avr/sleep.h>
#include <avr/wdt.h>

//#define _I2C_DIRECT_IO    1 // Enables communications that is not using the standard I/O pull-down approach with pull-up resistors, instead I/O is directly driven with 0V/5V
class I2C_ { // Secundairy I2C class used by I2C LCD/OLED, uses alternate pins: PD2 (SDA) and PD3 (SCL)
public:
#if(F_MCU > 20900000)
#ifdef OLED_SH1106
#define _DELAY() for(uint8_t i = 0; i != 9; i++) asm("nop");
#else
#ifdef OLED_SSD1306
#define _DELAY() for(uint8_t i = 0; i != 6; i++) asm("nop");
#else // other (I2C_LCD)
#define _DELAY() for(uint8_t i = 0; i != 7; i++) asm("nop");
#endif
#endif
#else // slow F_MCU
#ifdef OLED_SH1106
#define _DELAY() for(uint8_t i = 0; i != 8; i++) asm("nop");
#else
#ifdef OLED_SSD1306
#define _DELAY() for(uint8_t i = 0; i != 4; i++) asm("nop"); // 4=731kb/s
#else // other (I2C_LCD)
#define _DELAY() for(uint8_t i = 0; i != 5; i++) asm("nop");
#endif
#endif
#endif // F_MCU
#define _I2C_SDA (1<<2) // PD2
#define _I2C_SCL (1<<3) // PD3
#ifdef _I2C_DIRECT_IO
#define _I2C_INIT() _I2C_SDA_HI(); _I2C_SCL_HI(); DDRD |= (_I2C_SDA | _I2C_SCL);  // direct I/O (no need for pull-ups)
#define _I2C_SDA_HI() PORTD |=  _I2C_SDA;
#define _I2C_SDA_LO() PORTD &= ~_I2C_SDA;
#define _I2C_SCL_HI() PORTD |=  _I2C_SCL; _DELAY();
#define _I2C_SCL_LO() PORTD &= ~_I2C_SCL; _DELAY();
#else // !_I2C_DIRECT_IO
#define _I2C_INIT()   PORTD &= ~_I2C_SDA; PORTD &= ~_I2C_SCL; _I2C_SDA_HI(); _I2C_SCL_HI();  // open-drain
#define _I2C_SDA_HI() DDRD &= ~_I2C_SDA;
#define _I2C_SDA_LO() DDRD |=  _I2C_SDA;
#define _I2C_SCL_HI() DDRD &= ~_I2C_SCL; _DELAY();
#define _I2C_SCL_LO() DDRD |=  _I2C_SCL; _DELAY();
#endif // !_I2C_DIRECT_IO
#define _I2C_START() _I2C_SDA_LO(); _DELAY(); _I2C_SCL_LO(); // _I2C_SDA_HI();
#define _I2C_STOP()  _I2C_SDA_LO(); _I2C_SCL_HI(); _I2C_SDA_HI();
#define _I2C_SUSPEND() //_I2C_SDA_LO(); // SDA_LO to allow re-use as output port
#define _SendBit(data, bit) \
    if(data & 1 << bit){ \
      _I2C_SDA_HI();  \
    } else {         \
      _I2C_SDA_LO();  \
    }                \
    _I2C_SCL_HI();    \
    _I2C_SCL_LO();
	inline void start() { _I2C_INIT(); _I2C_START(); };
	inline void stop() { _I2C_STOP(); _I2C_SUSPEND(); };
	inline void SendByte(uint8_t data) {
		_SendBit(data, 7);
		_SendBit(data, 6);
		_SendBit(data, 5);
		_SendBit(data, 4);
		_SendBit(data, 3);
		_SendBit(data, 2);
		_SendBit(data, 1);
		_SendBit(data, 0);
		_I2C_SDA_HI();  // recv ACK
		_DELAY(); //
		_I2C_SCL_HI();
		_I2C_SCL_LO();
	}
	void SendRegister(uint8_t addr, uint8_t* data, uint8_t n) {
		start();
		SendByte(addr << 1);
		while (n--) SendByte(*data++);
		stop();
	}
	//void SendRegister(uint8_t addr, uint8_t val){ SendRegister(addr, &val, 1); }

	void begin() {};
	void beginTransmission(uint8_t addr) { start(); SendByte(addr << 1); };
	bool write(uint8_t byte) { SendByte(byte); return 1; };
	uint8_t endTransmission() { stop(); return 0; };
};
I2C_ Wire;
//#include <Wire.h>

uint8_t backlight = 8;
//#define RS_HIGH_ON_IDLE   1   // Experimental LCD support where RS line is high on idle periods to comply with SDA I2C standard.

class LCD : public Print {  // inspired by: http://www.technoblogy.com/show?2BET
public:  // LCD1602 display in 4-bit mode, RS is pull-up and kept low when idle to prevent potential display RFI via RS line
#define _dn  0      // PD0 to PD3 connect to D4 to D7 on the display
#define _en  4      // PD4 - MUST have pull-up resistor
#define _rs  4      // PC4 - MUST have pull-up resistor
#define RS_PULLUP  1   // Use pullup on RS line, ensures compliancy to the absolute maximum ratings for the si5351 sda input that is shared with rs pin of lcd
#ifdef RS_PULLUP
#define LCD_RS_HI() DDRC &= ~(1 << _rs); asm("nop"); // RS high (pull-up)
#define LCD_RS_LO() DDRC |= 1 << _rs;                // RS low (pull-down)
#else
#define LCD_RS_LO() PORTC &= ~(1 << _rs);        // RS low
#define LCD_RS_HI() PORTC |= (1 << _rs);         // RS high
#endif //RS_PULLUP
#define LCD_EN_LO() PORTD &= ~(1 << _en);        // EN low
#define LCD_EN_HI() PORTD |= (1 << _en);         // EN high
#define LCD_PREP_NIBBLE(b) (PORTD & ~(0xf << _dn)) | (b) << _dn | 1 << _en // Send data and enable high
	uint8_t _cols;
	void begin(uint8_t x = 0, uint8_t y = 0) {        // Send command , make sure at least 40ms after power-up before sending commands
		//bool reinit = (x == 0) && (y == 0);
#ifdef LCD_I2C
#define PCF_ADDR  0x27                         // LCD I2C address where PCF8574 addess selection A0, A1, A2 are all open
#define PCF_RS 0x01
#define PCF_RW 0x02  // the 0xF0 bits are used for 4-bit data to the display.
#define PCF_EN 0x04
#define PCF_BACKLIGHT 0x08
		Wire.beginTransmission(PCF_ADDR);
		Wire.write(0);
		Wire.endTransmission();
		delayMicroseconds(50000);
#else //!LCD_I2C
		DDRD |= 0xf << _dn | 1 << _en;                 // Make data, EN outputs
		DDRC |= 1 << _rs;
		//PORTC &= ~(1 << _rs);                          // Set RS low in case to support pull-down when DDRC is output
		delayMicroseconds(50000);                      // *
		LCD_RS_LO(); LCD_EN_LO();
#endif //!LCD_I2C
		cmd(0x33);                                     // Ensures display is in 8-bit mode
		delayMicroseconds(4500); cmd(0x33); delayMicroseconds(4500); cmd(0x33); delayMicroseconds(150); // * Ensures display is in 8-bit mode
		cmd(0x32);                                     // Puts display in 4-bit mode
		cmd(0x28);                                     // * Function set: 2-line, 5x8 
		cmd(0x0c);                                     // Display on
		//if(reinit) return;
		cmd(0x01);                                     // Clear display
		delay(3);                                      // Allow to execute Clear on display [https://www.sparkfun.com/datasheets/LCD/HD44780.pdf, p.49, p58]
		cmd(0x06);                                     // * Entrymode: left, shift-dec
	}
#ifdef LCD_I2C
	void nib(uint8_t b, bool isData) {                             // Send four bit nibble to display
		b = (b << 4) | ((backlight) ? PCF_BACKLIGHT : 0) | ((isData) ? PCF_RS : 0);
		Wire.write(b | PCF_EN);  // write command EN HI
		delayMicroseconds(4);    // enable pulse must be >450ns
		Wire.write(b);           // write command EN LO
		delayMicroseconds(60);   // commands need > 37us to settle
		Wire.write(b);           // must write for some unknown reason
	}
	void cmd(uint8_t b) {
		Wire.beginTransmission(PCF_ADDR);
		nib(b >> 4, false); nib(b, false);
		Wire.endTransmission();
	}
	size_t write(uint8_t b) {
		Wire.beginTransmission(PCF_ADDR);
		nib((b >> 4), true); nib((b), true);
		Wire.endTransmission();
	}
#else //!LCD_I2C
	// Since LCD is using PD0(RXD), PD1(TXD) pins in the data-path, some co-existence feature is required when using the serial port.
	// The following functions are temporarily disabling the serial port when LCD writes happen, and make sure that serial transmission is ended.
	// To prevent that LCD writes are received by the serial receiver, PC2 is made HIGH during writes to pull-up TXD via a diode.
	// The RXD, TXD lines are connected to the host via 1k resistors, a 1N4148 is placed between PC2 (anode) and the TXD resistor.

	// NOISE LEAK INTO RX!!! G8RDI NOTE

	// There are two drawbacks when continuous LCD writes happen:
	// 1. noise is leaking via the AREF pull-ups into the receiver
	// 2. serial data cannot be received

	void pre() {
#ifdef _SERIAL
		if (!vox) if (cat_active) { Serial.flush(); for (; millis() < rxend_event;)wdt_reset(); PORTC |= 1 << 2; DDRC |= 1 << 2; } UCSR0B &= ~((1 << RXEN0) | (1 << TXEN0)); // Complete serial TX and RX; mask PD1 LCD data-exchange by pulling-up TXD via PC2 HIGH; enable PD0/PD1, disable serial port
#endif
		noInterrupts();  // ***!!!*** do not allow LCD tranfer to be interrupted, to prevent backlight to lighten-up
	}
	void post() {
		///if(backlight) PORTD |= 0x08; else PORTD &= ~0x08;   // Backlight control
		if (backlight)
			PORTD |= BACKLIGHT_PIN;
		else
			PORTD &= ~BACKLIGHT_PIN;   // Backlight control - G8RDI MOD
#ifdef _SERIAL
		//UCSR0B |= (1<<RXEN0)|(1<<TXEN0); if(!vox) if(cat_active){ DDRC &= ~(1<<2); } // Enable serial port, disable PD0, PD1; disable PC2
		UCSR0B |= (1 << RXEN0) | (1 << TXEN0); if (!vox) if (cat_active) { PORTC &= ~(1 << 2); } // Enable serial port, disable PD0, PD1; PC2 LOW to prevent CAT TX disruption via MIC input
#endif
		interrupts();
	}
#ifdef RS_HIGH_ON_IDLE
	void cmd(uint8_t b) {
		pre();
		uint8_t nibh = LCD_PREP_NIBBLE(b >> 4);       // Prepare high nibble data and enable high
		PORTD = nibh;                                  // Send high nibble data and enable high
		uint8_t nibl = LCD_PREP_NIBBLE(b & 0xf);       // Prepare low nibble data and enable high
		LCD_RS_LO();
		LCD_EN_LO();
		PORTD = nibl;                                  // Send low nibble data and enable high
		asm("nop"); asm("nop");                        // Keep RS low, but complete enable cycle (should be 500ns)
		LCD_EN_LO();
		LCD_RS_HI();
		post();
		delayMicroseconds(60);                         // Execution time  (37+4)*1.25 us
	}
	size_t write(uint8_t b) {                         // Write data:    send nibbles while RS high
		pre();
		uint8_t nibh = LCD_PREP_NIBBLE(b >> 4);       // Prepare high nibble data and enable high
		PORTD = nibh;                                  // Send high nibble data and enable high
		uint8_t nibl = LCD_PREP_NIBBLE(b & 0xf);       // Prepare low nibble data and enable high
		LCD_RS_HI();
		LCD_EN_LO();
		PORTD = nibl;                                  // Send low nibble data and enable high
		asm("nop"); asm("nop");                        // Keep RS high, but complete enable cycle (should be 500ns)
		LCD_EN_LO();
		post();
		delayMicroseconds(60);                         // Execution time  (37+4)*1.25 us
		return 1;
	}
#else //!RS_HIGH_ON_IDLE
	void nib(uint8_t b) {                             // Send four bit nibble to display
		pre();
		PORTD = LCD_PREP_NIBBLE(b);                    // Send data and enable high
		//asm("nop");                                    // Enable high pulse width must be at least 230ns high, data-setup time 80ns
		delayMicroseconds(4);
		LCD_EN_LO();
		post();
		//delayMicroseconds(52);                         // Execution time
		delayMicroseconds(60);                         // Execution time
	}
	void cmd(uint8_t b) { nib(b >> 4); nib(b & 0xf); } // Write command: send nibbles while RS low
	size_t write(uint8_t b) {                         // Write data:    send nibbles while RS high
		pre();
		//LCD_EN_HI();                                   // Complete Enable cycle must be at least 500ns (so start early)
		uint8_t nibh = LCD_PREP_NIBBLE(b >> 4);       // Prepare high nibble data and enable high
		PORTD = nibh;                                  // Send high nibble data and enable high
		uint8_t nibl = LCD_PREP_NIBBLE(b & 0xf);       // Prepare low nibble data and enable high
		//asm("nop");                                    // Enable high pulse width must be at least 230ns high, data-setup time 80ns; ATMEGA clock-cycle is 50ns (so at least 5 cycles)
		LCD_RS_HI();
		LCD_EN_LO();
		PORTD = nibl;                                  // Send low nibble data and enable high
		LCD_RS_LO();
		//asm("nop"); asm("nop");                        // Complete Enable cycle must be at least 500ns
		//PORTD = nibl;                                  // Send low nibble data and enable high
		//asm("nop");                                    // Enable high pulse width must be at least 230ns high, data-setup time 80ns; ATMEGA clock-cycle is 50ns (so at least 5 cycles)
		LCD_RS_HI();
		LCD_EN_LO();
		LCD_RS_LO();
		post();
		delayMicroseconds(60);                         // Execution time  (37+4)*1.25 us
		return 1;
	}
#endif // RS_HIGH_ON_IDLE
#endif //!LCD_I2C
#ifdef CONDENSED
	void setCursor(uint8_t x, uint8_t y) { cmd(0x80 | (x + (uint8_t[]) { 0x00, 0x40, 0x00 + 20, 0x40 + 20 } [y])); }  // ONLY for LCD2004 display
#else
	void setCursor(uint8_t x, uint8_t y) { cmd(0x80 | (x + y * 0x40)); }
#endif
	void cursor() { cmd(0x0e); }
	void noCursor() { cmd(0x0c); }
	void noDisplay() { cmd(0x08); }
	void createChar(uint8_t l, uint8_t glyph[]) { cmd(0x40 | ((l & 0x7) << 3)); for (int i = 0; i != 8; i++) write(glyph[i]); }
};

/*
class LCD : public Print {  // inspired by: http://www.technoblogy.com/show?2BET
public:  // LCD1602 display in 4-bit mode, RS is pull-up and kept low when idle to prevent potential display RFI via RS line
  #define _dn  0      // PD0 to PD3 connect to D4 to D7 on the display
  #define _en  4      // PC4 - MUST have pull-up resistor
  #define _rs  4      // PC4 - MUST have pull-up resistor
  #define LCD_RS_HI() DDRC &= ~(1 << _rs);         // RS high (pull-up)
  #define LCD_RS_LO() DDRC |= 1 << _rs;            // RS low (pull-down)
  #define LCD_EN_LO() PORTD &= ~(1 << _en);        // EN low
  #define LCD_PREP_NIBBLE(b) (PORTD & ~(0xf << _dn)) | (b) << _dn | 1 << _en // Send data and enable high
  void begin(uint8_t x, uint8_t y){                // Send command
  DDRD |= 0xf << _dn | 1 << _en;                 // Make data, EN and RS pins outputs
  PORTC &= ~(1 << _rs);                          // Set RS low in case to support pull-down when DDRC is output
  delayMicroseconds(50000);                      // * At least 40ms after power rises above 2.7V before sending commands
  LCD_RS_LO();
  cmd(0x33);                                     // Ensures display is in 8-bit mode
  cmd(0x32);                                     // Puts display in 4-bit mode
  cmd(0x0e);                                     // Display and cursor on
  cmd(0x01);                                     // Clear display
  delay(3);                                      // Allow to execute on display [https://www.sparkfun.com/datasheets/LCD/HD44780.pdf, p.49, p58]
  }
  void nib(uint8_t b){                             // Send four bit nibble to display
  PORTD = LCD_PREP_NIBBLE(b);                    // Send data and enable high
  delayMicroseconds(4);
  LCD_EN_LO();
  delayMicroseconds(60);                         // Execution time  (was: 37)
  }
  void cmd(uint8_t b){ nib(b >> 4); nib(b & 0xf); }// Write command: send nibbles while RS low
  size_t write(uint8_t b){                         // Write data:    send nibbles while RS high
  uint8_t nibh = LCD_PREP_NIBBLE(b >>  4);       // Prepare high nibble data and enable high
  uint8_t nibl = LCD_PREP_NIBBLE(b & 0xf);       // Prepare low nibble data and enable high
  PORTD = nibh;                                  // Send high nibble data and enable high
  LCD_RS_HI();
  LCD_EN_LO();
  PORTD = nibl;                                  // Send low nibble data and enable high
  LCD_RS_LO();
  LCD_RS_HI();
  LCD_EN_LO();
  LCD_RS_LO();
  delayMicroseconds(41);                         // Execution time
  PORTD |= 0x02;                                 // To support serial-interface keep LCD_D5 high, so that DVM is not pulled-down via D
  return 1;
  }
  void setCursor(uint8_t x, uint8_t y){ cmd(0x80 | (x + y * 0x40)); }
  void cursor(){ cmd(0x0e); }
  void noCursor(){ cmd(0x0c); }
  void noDisplay(){ cmd(0x08); }
  void createChar(uint8_t l, uint8_t glyph[]){ cmd(0x40 | ((l & 0x7) << 3)); for(int i = 0; i != 8; i++) write(glyph[i]); }
};
*/
/*
#include <LiquidCrystal.h>
class LCD_ : public LiquidCrystal {
public: // QCXLiquidCrystal extends LiquidCrystal library for pull-up driven LCD_RS, as done on QCX. LCD_RS needs to be set to LOW in advance of calling any operation.
  //LCD_(uint8_t rs = LCD_RS, uint8_t en = LCD_EN, uint8_t d4 = LCD_D4, uint8_t d5, = LCD_D5 uint8_t d6 = LCD_D6, uint8_t d7 = LCD_D7) : LiquidCrystal(rs, en, d4, d5, d6, d7){ };
  LCD_() : LiquidCrystal(LCD_RS, LCD_EN, LCD_D4, LCD_D5, LCD_D6, LCD_D7){ };
  virtual size_t write(uint8_t value){ // overwrites LiquidCrystal::write() and re-implements LCD data writes
  pinMode(LCD_RS, INPUT);  // pull-up LCD_RS
  write4bits(value >> 4);
  write4bits(value);
  pinMode(LCD_RS, OUTPUT); // pull-down LCD_RS
  return 1;
  };
  void write4bits(uint8_t value){
  digitalWrite(LCD_D4, (value >> 0) & 0x01);
  digitalWrite(LCD_D5, (value >> 1) & 0x01);
  digitalWrite(LCD_D6, (value >> 2) & 0x01);
  digitalWrite(LCD_D7, (value >> 3) & 0x01);
  digitalWrite(LCD_EN, LOW);  // pulseEnable
  delayMicroseconds(1);
  digitalWrite(LCD_EN, HIGH);
  delayMicroseconds(1);    // enable pulse must be >450ns
  digitalWrite(LCD_EN, LOW);
  delayMicroseconds(100);   // commands need > 37us to settle
  };
};
*/

/* // 6x8 technoblogy font
const uint8_t font[]PROGMEM = {
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x5F, 0x00, 0x00, 0x00,
   0x00, 0x07, 0x00, 0x07, 0x00, 0x00,
   0x14, 0x7F, 0x14, 0x7F, 0x14, 0x00,
   0x24, 0x2A, 0x7F, 0x2A, 0x12, 0x00,
   0x23, 0x13, 0x08, 0x64, 0x62, 0x00,
   0x36, 0x49, 0x56, 0x20, 0x50, 0x00,
   0x00, 0x08, 0x07, 0x03, 0x00, 0x00,
   0x00, 0x1C, 0x22, 0x41, 0x00, 0x00,
   0x00, 0x41, 0x22, 0x1C, 0x00, 0x00,
   0x2A, 0x1C, 0x7F, 0x1C, 0x2A, 0x00,
   0x08, 0x08, 0x3E, 0x08, 0x08, 0x00,
   0x00, 0x80, 0x70, 0x30, 0x00, 0x00,
   0x08, 0x08, 0x08, 0x08, 0x08, 0x00,
   0x00, 0x00, 0x60, 0x60, 0x00, 0x00,
   0x20, 0x10, 0x08, 0x04, 0x02, 0x00,
   0x3E, 0x51, 0x49, 0x45, 0x3E, 0x00,
   0x00, 0x42, 0x7F, 0x40, 0x00, 0x00,
   0x72, 0x49, 0x49, 0x49, 0x46, 0x00,
   0x21, 0x41, 0x49, 0x4D, 0x33, 0x00,
   0x18, 0x14, 0x12, 0x7F, 0x10, 0x00,
   0x27, 0x45, 0x45, 0x45, 0x39, 0x00,
   0x3C, 0x4A, 0x49, 0x49, 0x31, 0x00,
   0x41, 0x21, 0x11, 0x09, 0x07, 0x00,
   0x36, 0x49, 0x49, 0x49, 0x36, 0x00,
   0x46, 0x49, 0x49, 0x29, 0x1E, 0x00,
   0x00, 0x00, 0x14, 0x00, 0x00, 0x00,
   0x00, 0x40, 0x34, 0x00, 0x00, 0x00,
   0x00, 0x08, 0x14, 0x22, 0x41, 0x00,
   0x14, 0x14, 0x14, 0x14, 0x14, 0x00,
   0x00, 0x41, 0x22, 0x14, 0x08, 0x00,
   0x02, 0x01, 0x59, 0x09, 0x06, 0x00,
   0x3E, 0x41, 0x5D, 0x59, 0x4E, 0x00,
   0x7C, 0x12, 0x11, 0x12, 0x7C, 0x00,
   0x7F, 0x49, 0x49, 0x49, 0x36, 0x00,
   0x3E, 0x41, 0x41, 0x41, 0x22, 0x00,
   0x7F, 0x41, 0x41, 0x41, 0x3E, 0x00,
   0x7F, 0x49, 0x49, 0x49, 0x41, 0x00,
   0x7F, 0x09, 0x09, 0x09, 0x01, 0x00,
   0x3E, 0x41, 0x41, 0x51, 0x73, 0x00,
   0x7F, 0x08, 0x08, 0x08, 0x7F, 0x00,
   0x00, 0x41, 0x7F, 0x41, 0x00, 0x00,
   0x20, 0x40, 0x41, 0x3F, 0x01, 0x00,
   0x7F, 0x08, 0x14, 0x22, 0x41, 0x00,
   0x7F, 0x40, 0x40, 0x40, 0x40, 0x00,
   0x7F, 0x02, 0x1C, 0x02, 0x7F, 0x00,
   0x7F, 0x04, 0x08, 0x10, 0x7F, 0x00,
   0x3E, 0x41, 0x41, 0x41, 0x3E, 0x00,
   0x7F, 0x09, 0x09, 0x09, 0x06, 0x00,
   0x3E, 0x41, 0x51, 0x21, 0x5E, 0x00,
   0x7F, 0x09, 0x19, 0x29, 0x46, 0x00,
   0x26, 0x49, 0x49, 0x49, 0x32, 0x00,
   0x03, 0x01, 0x7F, 0x01, 0x03, 0x00,
   0x3F, 0x40, 0x40, 0x40, 0x3F, 0x00,
   0x1F, 0x20, 0x40, 0x20, 0x1F, 0x00,
   0x3F, 0x40, 0x38, 0x40, 0x3F, 0x00,
   0x63, 0x14, 0x08, 0x14, 0x63, 0x00,
   0x03, 0x04, 0x78, 0x04, 0x03, 0x00,
   0x61, 0x59, 0x49, 0x4D, 0x43, 0x00,
   0x00, 0x7F, 0x41, 0x41, 0x41, 0x00,
   0x02, 0x04, 0x08, 0x10, 0x20, 0x00,
   0x00, 0x41, 0x41, 0x41, 0x7F, 0x00,
   0x04, 0x02, 0x01, 0x02, 0x04, 0x00,
   0x40, 0x40, 0x40, 0x40, 0x40, 0x00,
   0x00, 0x03, 0x07, 0x08, 0x00, 0x00,
   0x20, 0x54, 0x54, 0x78, 0x40, 0x00,
   0x7F, 0x28, 0x44, 0x44, 0x38, 0x00,
   0x38, 0x44, 0x44, 0x44, 0x28, 0x00,
   0x38, 0x44, 0x44, 0x28, 0x7F, 0x00,
   0x38, 0x54, 0x54, 0x54, 0x18, 0x00,
   0x00, 0x08, 0x7E, 0x09, 0x02, 0x00,
   0x18, 0xA4, 0xA4, 0x9C, 0x78, 0x00,
   0x7F, 0x08, 0x04, 0x04, 0x78, 0x00,
   0x00, 0x44, 0x7D, 0x40, 0x00, 0x00,
   0x20, 0x40, 0x40, 0x3D, 0x00, 0x00,
   0x7F, 0x10, 0x28, 0x44, 0x00, 0x00,
   0x00, 0x41, 0x7F, 0x40, 0x00, 0x00,
   0x7C, 0x04, 0x78, 0x04, 0x78, 0x00,
   0x7C, 0x08, 0x04, 0x04, 0x78, 0x00,
   0x38, 0x44, 0x44, 0x44, 0x38, 0x00,
   0xFC, 0x18, 0x24, 0x24, 0x18, 0x00,
   0x18, 0x24, 0x24, 0x18, 0xFC, 0x00,
   0x7C, 0x08, 0x04, 0x04, 0x08, 0x00,
   0x48, 0x54, 0x54, 0x54, 0x24, 0x00,
   0x04, 0x04, 0x3F, 0x44, 0x24, 0x00,
   0x3C, 0x40, 0x40, 0x20, 0x7C, 0x00,
   0x1C, 0x20, 0x40, 0x20, 0x1C, 0x00,
   0x3C, 0x40, 0x30, 0x40, 0x3C, 0x00,
   0x44, 0x28, 0x10, 0x28, 0x44, 0x00,
   0x4C, 0x90, 0x90, 0x90, 0x7C, 0x00,
   0x44, 0x64, 0x54, 0x4C, 0x44, 0x00,
   0x00, 0x08, 0x36, 0x41, 0x00, 0x00,
   0x00, 0x00, 0x77, 0x00, 0x00, 0x00,
   0x00, 0x41, 0x36, 0x08, 0x00, 0x00,
   0x02, 0x01, 0x02, 0x04, 0x02, 0x00,
   0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00 };

#define FONT_W 12//6
#define FONT_H 2
#define FONT_STRETCHV 1
#define FONT_STRETCHH 1//0
*/
#ifndef REMOVEFONT   // G8RDI mod - FONT NOT COMPILED IF NOT USED
// C64 real
//G8RDI mod to free more prog memory (FONT NOT COMPILED IF NOT USED) //const uint8_t font[]PROGMEM = {
const uint8_t font[]PROGMEM = {
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // ' '
   0x00, 0x00, 0x00, 0x4f, 0x4f, 0x00, 0x00, 0x00, // !
   0x00, 0x07, 0x07, 0x00, 0x00, 0x07, 0x07, 0x00, // "
   0x14, 0x7f, 0x7f, 0x14, 0x14, 0x7f, 0x7f, 0x14, // #
   0x00, 0x24, 0x2e, 0x6b, 0x6b, 0x3a, 0x12, 0x00, // $
   0x00, 0x63, 0x33, 0x18, 0x0c, 0x66, 0x63, 0x00, // %
   0x00, 0x32, 0x7f, 0x4d, 0x4d, 0x77, 0x72, 0x50, // &
   0x00, 0x00, 0x00, 0x04, 0x06, 0x03, 0x01, 0x00, // '
   0x00, 0x00, 0x1c, 0x3e, 0x63, 0x41, 0x00, 0x00, // (
   0x00, 0x00, 0x41, 0x63, 0x3e, 0x1c, 0x00, 0x00, // )
   0x08, 0x2a, 0x3e, 0x1c, 0x1c, 0x3e, 0x2a, 0x08, // *
   0x00, 0x08, 0x08, 0x3e, 0x3e, 0x08, 0x08, 0x00, // +
   0x00, 0x00, 0x80, 0xe0, 0x60, 0x00, 0x00, 0x00, // ,
   0x00, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x00, // -
   0x00, 0x00, 0x00, 0x60, 0x60, 0x00, 0x00, 0x00, // .
   0x00, 0x40, 0x60, 0x30, 0x18, 0x0c, 0x06, 0x02, // /
   0x00, 0x3e, 0x7f, 0x49, 0x45, 0x7f, 0x3e, 0x00, // 0
   0x00, 0x40, 0x44, 0x7f, 0x7f, 0x40, 0x40, 0x00, // 1
   0x00, 0x62, 0x73, 0x51, 0x49, 0x4f, 0x46, 0x00, // 2
   0x00, 0x22, 0x63, 0x49, 0x49, 0x7f, 0x36, 0x00, // 3
   0x00, 0x18, 0x18, 0x14, 0x16, 0x7f, 0x7f, 0x10, // 4
   0x00, 0x27, 0x67, 0x45, 0x45, 0x7d, 0x39, 0x00, // 5
   0x00, 0x3e, 0x7f, 0x49, 0x49, 0x7b, 0x32, 0x00, // 6
   0x00, 0x03, 0x03, 0x79, 0x7d, 0x07, 0x03, 0x00, // 7
   0x00, 0x36, 0x7f, 0x49, 0x49, 0x7f, 0x36, 0x00, // 8
   0x00, 0x26, 0x6f, 0x49, 0x49, 0x7f, 0x3e, 0x00, // 9
   0x00, 0x00, 0x00, 0x24, 0x24, 0x00, 0x00, 0x00, // :
   0x00, 0x00, 0x80, 0xe4, 0x64, 0x00, 0x00, 0x00, // ;
   0x00, 0x08, 0x1c, 0x36, 0x63, 0x41, 0x41, 0x00, // <
   0x00, 0x14, 0x14, 0x14, 0x14, 0x14, 0x14, 0x00, // =
   0x00, 0x41, 0x41, 0x63, 0x36, 0x1c, 0x08, 0x00, // >
   0x00, 0x02, 0x03, 0x51, 0x59, 0x0f, 0x06, 0x00, // ?
   0x00, 0x3e, 0x7f, 0x41, 0x4d, 0x4f, 0x2e, 0x00, // @
   0x00, 0x7c, 0x7e, 0x0b, 0x0b, 0x7e, 0x7c, 0x00, // A
   0x00, 0x7f, 0x7f, 0x49, 0x49, 0x7f, 0x36, 0x00, // B
   0x00, 0x3e, 0x7f, 0x41, 0x41, 0x63, 0x22, 0x00, // C
   0x00, 0x7f, 0x7f, 0x41, 0x63, 0x3e, 0x1c, 0x00, // D
   0x00, 0x7f, 0x7f, 0x49, 0x49, 0x41, 0x41, 0x00, // E
   0x00, 0x7f, 0x7f, 0x09, 0x09, 0x01, 0x01, 0x00, // F
   0x00, 0x3e, 0x7f, 0x41, 0x49, 0x7b, 0x3a, 0x00, // G
   0x00, 0x7f, 0x7f, 0x08, 0x08, 0x7f, 0x7f, 0x00, // H
   0x00, 0x00, 0x41, 0x7f, 0x7f, 0x41, 0x00, 0x00, // I
   0x00, 0x20, 0x60, 0x41, 0x7f, 0x3f, 0x01, 0x00, // J
   0x00, 0x7f, 0x7f, 0x1c, 0x36, 0x63, 0x41, 0x00, // K
   0x00, 0x7f, 0x7f, 0x40, 0x40, 0x40, 0x40, 0x00, // L
   0x00, 0x7f, 0x7f, 0x06, 0x0c, 0x06, 0x7f, 0x7f, // M
   0x00, 0x7f, 0x7f, 0x0e, 0x1c, 0x7f, 0x7f, 0x00, // N
   0x00, 0x3e, 0x7f, 0x41, 0x41, 0x7f, 0x3e, 0x00, // O
   0x00, 0x7f, 0x7f, 0x09, 0x09, 0x0f, 0x06, 0x00, // P
   0x00, 0x1e, 0x3f, 0x21, 0x61, 0x7f, 0x5e, 0x00, // Q
   0x00, 0x7f, 0x7f, 0x19, 0x39, 0x6f, 0x46, 0x00, // R
   0x00, 0x26, 0x6f, 0x49, 0x49, 0x7b, 0x32, 0x00, // S
   0x00, 0x01, 0x01, 0x7f, 0x7f, 0x01, 0x01, 0x00, // T
   0x00, 0x3f, 0x7f, 0x40, 0x40, 0x7f, 0x3f, 0x00, // U
   0x00, 0x1f, 0x3f, 0x60, 0x60, 0x3f, 0x1f, 0x00, // V
   0x00, 0x7f, 0x7f, 0x30, 0x18, 0x30, 0x7f, 0x7f, // W
   0x00, 0x63, 0x77, 0x1c, 0x1c, 0x77, 0x63, 0x00, // X
   0x00, 0x07, 0x0f, 0x78, 0x78, 0x0f, 0x07, 0x00, // Y
   0x00, 0x61, 0x71, 0x59, 0x4d, 0x47, 0x43, 0x00, // Z
   0x00, 0x00, 0x7f, 0x7f, 0x41, 0x41, 0x00, 0x00, // [
   0x00, 0x02, 0x06, 0x0c, 0x18, 0x30, 0x60, 0x40,
   0x00, 0x00, 0x41, 0x41, 0x7f, 0x7f, 0x00, 0x00, // ]
   0x00, 0x08, 0x0c, 0xfe, 0xfe, 0x0c, 0x08, 0x00, // ^
   0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, // _
   0x00, 0x01, 0x03, 0x06, 0x04, 0x00, 0x00, 0x00, // '
   0x00, 0x20, 0x74, 0x54, 0x54, 0x7c, 0x78, 0x00, // a
   0x00, 0x7e, 0x7e, 0x48, 0x48, 0x78, 0x30, 0x00, // b
   0x00, 0x38, 0x7c, 0x44, 0x44, 0x44, 0x00, 0x00, // c
   0x00, 0x30, 0x78, 0x48, 0x48, 0x7e, 0x7e, 0x00, // d
   0x00, 0x38, 0x7c, 0x54, 0x54, 0x5c, 0x18, 0x00, // e
   0x00, 0x00, 0x08, 0x7c, 0x7e, 0x0a, 0x0a, 0x00, // f
   0x00, 0x98, 0xbc, 0xa4, 0xa4, 0xfc, 0x7c, 0x00, // g
   0x00, 0x7e, 0x7e, 0x08, 0x08, 0x78, 0x70, 0x00, // h
   0x00, 0x00, 0x48, 0x7a, 0x7a, 0x40, 0x00, 0x00, // i
   0x00, 0x00, 0x80, 0x80, 0x80, 0xfa, 0x7a, 0x00, // j
   0x00, 0x7e, 0x7e, 0x10, 0x38, 0x68, 0x40, 0x00, // k
   0x00, 0x00, 0x42, 0x7e, 0x7e, 0x40, 0x00, 0x00, // l
   0x00, 0x7c, 0x7c, 0x18, 0x38, 0x1c, 0x7c, 0x78, // m
   0x00, 0x7c, 0x7c, 0x04, 0x04, 0x7c, 0x78, 0x00, // n
   0x00, 0x38, 0x7c, 0x44, 0x44, 0x7c, 0x38, 0x00, // o
   0x00, 0xfc, 0xfc, 0x24, 0x24, 0x3c, 0x18, 0x00, // p
   0x00, 0x18, 0x3c, 0x24, 0x24, 0xfc, 0xfc, 0x00, // q
   0x00, 0x7c, 0x7c, 0x04, 0x04, 0x0c, 0x08, 0x00, // r
   0x00, 0x48, 0x5c, 0x54, 0x54, 0x74, 0x24, 0x00, // s
   0x00, 0x04, 0x04, 0x3e, 0x7e, 0x44, 0x44, 0x00, // t
   0x00, 0x3c, 0x7c, 0x40, 0x40, 0x7c, 0x7c, 0x00, // u
   0x00, 0x1c, 0x3c, 0x60, 0x60, 0x3c, 0x1c, 0x00, // v
   0x00, 0x1c, 0x7c, 0x70, 0x38, 0x70, 0x7c, 0x1c, // w
   0x00, 0x44, 0x6c, 0x38, 0x38, 0x6c, 0x44, 0x00, // x
   0x00, 0x9c, 0xbc, 0xa0, 0xe0, 0x7c, 0x3c, 0x00, // y
   0x00, 0x44, 0x64, 0x74, 0x5c, 0x4c, 0x44, 0x00, // z
   0x00, 0x08, 0x3e, 0x77, 0x41, 0x41, 0x00, 0x00, // {
   0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0x00, // |
   0x00, 0x00, 0x41, 0x41, 0x77, 0x3e, 0x08, 0x00, // }
   0x00, 0x04, 0x02, 0x02, 0x04, 0x04, 0x02, 0x00, // ~

  0b0000000,  // 126+1; logo
  0b1010101,
  0b0101010,
  0b0101010,
  0b0010100,
  0b0010100,
  0b0001000,
  0b0001000,
  0b00000,  // 126+2; s-meter, 0 bars
  0b00000,
  0b00000,
  0b00000,
  0b00000,
  0b00000,
  0b00000,
  0b00000,
  0b00000,  // 126+3; s-meter, 1 bars
  0b00000,
  0b00000,
  0b11111,
  0b00000,
  0b00000,
  0b00000,
  0b00000,
  0b00000,  // 126+4; s-meter, 2 bars
  0b00000,
  0b00000,
  0b11111,
  0b00000,
  0b11111,
  0b00000,
  0b00000,
  0b00000,  // 126+5; s-meter, 3 bars
  0b00000,
  0b00000,
  0b11111,
  0b00000,
  0b11111,
  0b00000,
  0b11111,
  0b00000,  // 126+6; vfo-a
  0b11100,
  0b11110,
  0b00101,
  0b00101,
  0b11110,
  0b11100,
  0b00000,
  0b00000,  // 126+7; vfo-b
  0b11111,
  0b11111,
  0b10101,
  0b10101,
  0b01010,
  0b01010,
  0b00000 };
#endif

#define FONT_W 8
#define FONT_H 2
#define FONT_STRETCHV 1
#define FONT_STRETCHH 0

/*  //16x8 C-64 kind of
const uint8_t font[]PROGMEM = {
   0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,  // 0x20
   0x00,0x06,0x5F,0x5F,0x06,0x00,0x00,0x00, // 0x21
   0x00,0x07,0x07,0x00,0x07,0x07,0x00,0x00, // 0x22
   0x14,0x7F,0x7F,0x14,0x7F,0x7F,0x14,0x00, // 0x23
   0x24,0x2E,0x6B,0x6B,0x3A,0x12,0x00,0x00, // 0x24
   0x46,0x66,0x30,0x18,0x0C,0x66,0x62,0x00, // 0x25
   0x30,0x7A,0x4F,0x5D,0x37,0x7A,0x48,0x00, // 0x26
   0x04,0x07,0x03,0x00,0x00,0x00,0x00,0x00, // 0x27
   0x00,0x1C,0x3E,0x63,0x41,0x00,0x00,0x00, // 0x28
   0x00,0x41,0x63,0x3E,0x1C,0x00,0x00,0x00, // 0x29
   0x08,0x2A,0x3E,0x1C,0x1C,0x3E,0x2A,0x08, // 0x2A
   0x08,0x08,0x3E,0x3E,0x08,0x08,0x00,0x00, // 0x2B
   0x00,0xA0,0xE0,0x60,0x00,0x00,0x00,0x00, // 0x2C
   0x08,0x08,0x08,0x08,0x08,0x08,0x00,0x00, // 0x2D
   0x00,0x00,0x60,0x60,0x00,0x00,0x00,0x00, // 0x2E
   0x60,0x30,0x18,0x0C,0x06,0x03,0x01,0x00, // 0x2F
   0x3E,0x7F,0x59,0x4D,0x7F,0x3E,0x00,0x00, // 0x30
   0x42,0x42,0x7F,0x7F,0x40,0x40,0x00,0x00, // 0x31
   0x62,0x73,0x59,0x49,0x6F,0x66,0x00,0x00, // 0x32
   0x22,0x63,0x49,0x49,0x7F,0x36,0x00,0x00, // 0x33
   0x18,0x1C,0x16,0x13,0x7F,0x7F,0x10,0x00, // 0x34
   0x27,0x67,0x45,0x45,0x7D,0x39,0x00,0x00, // 0x35
   0x3C,0x7E,0x4B,0x49,0x79,0x30,0x00,0x00, // 0x36
   0x03,0x63,0x71,0x19,0x0F,0x07,0x00,0x00, // 0x37
   0x36,0x7F,0x49,0x49,0x7F,0x36,0x00,0x00, // 0x38
   0x06,0x4F,0x49,0x69,0x3F,0x1E,0x00,0x00, // 0x39
   0x00,0x00,0x6C,0x6C,0x00,0x00,0x00,0x00, // 0x3A
   0x00,0xA0,0xEC,0x6C,0x00,0x00,0x00,0x00, // 0x3B
   0x08,0x1C,0x36,0x63,0x41,0x00,0x00,0x00, // 0x3C
   0x14,0x14,0x14,0x14,0x14,0x14,0x00,0x00, // 0x3D
   0x00,0x41,0x63,0x36,0x1C,0x08,0x00,0x00, // 0x3E
   0x02,0x03,0x51,0x59,0x0F,0x06,0x00,0x00, // 0x3F
   0x3E,0x7F,0x41,0x5D,0x5D,0x1F,0x1E,0x00, // 0x40
   0x7C,0x7E,0x13,0x13,0x7E,0x7C,0x00,0x00, // 0x41
   0x41,0x7F,0x7F,0x49,0x49,0x7F,0x36,0x00, // 0x42
   0x1C,0x3E,0x63,0x41,0x41,0x63,0x22,0x00, // 0x43
   0x41,0x7F,0x7F,0x41,0x63,0x7F,0x1C,0x00, // 0x44
   0x41,0x7F,0x7F,0x49,0x5D,0x41,0x63,0x00, // 0x45
   0x41,0x7F,0x7F,0x49,0x1D,0x01,0x03,0x00, // 0x46
   0x1C,0x3E,0x63,0x41,0x51,0x73,0x72,0x00, // 0x47
   0x7F,0x7F,0x08,0x08,0x7F,0x7F,0x00,0x00, // 0x48
   0x00,0x41,0x7F,0x7F,0x41,0x00,0x00,0x00, // 0x49
   0x30,0x70,0x40,0x41,0x7F,0x3F,0x01,0x00, // 0x4A
   0x41,0x7F,0x7F,0x08,0x1C,0x77,0x63,0x00, // 0x4B
   0x41,0x7F,0x7F,0x41,0x40,0x60,0x70,0x00, // 0x4C
   0x7F,0x7F,0x06,0x0C,0x06,0x7F,0x7F,0x00, // 0x4D
   0x7F,0x7F,0x06,0x0C,0x18,0x7F,0x7F,0x00, // 0x4E
   0x1C,0x3E,0x63,0x41,0x63,0x3E,0x1C,0x00, // 0x4F
   0x41,0x7F,0x7F,0x49,0x09,0x0F,0x06,0x00, // 0x50
   0x1E,0x3F,0x21,0x71,0x7F,0x5E,0x00,0x00, // 0x51
   0x41,0x7F,0x7F,0x19,0x39,0x6F,0x46,0x00, // 0x52
   0x26,0x67,0x4D,0x59,0x7B,0x32,0x00,0x00, // 0x53
   0x03,0x41,0x7F,0x7F,0x41,0x03,0x00,0x00, // 0x54
   0x7F,0x7F,0x40,0x40,0x7F,0x7F,0x00,0x00, // 0x55
   0x1F,0x3F,0x60,0x60,0x3F,0x1F,0x00,0x00, // 0x56
   0x7F,0x7F,0x30,0x18,0x30,0x7F,0x7F,0x00, // 0x57
   0x63,0x77,0x1C,0x08,0x1C,0x77,0x63,0x00, // 0x58
   0x07,0x4F,0x78,0x78,0x4F,0x07,0x00,0x00, // 0x59
   0x67,0x73,0x59,0x4D,0x47,0x63,0x71,0x00, // 0x5A
   0x00,0x7F,0x7F,0x41,0x41,0x00,0x00,0x00, // 0x5B
   0x01,0x03,0x06,0x0C,0x18,0x30,0x60,0x00, // 0x5C
   0x00,0x41,0x41,0x7F,0x7F,0x00,0x00,0x00, // 0x5D
   0x08,0x0C,0x06,0x03,0x06,0x0C,0x08,0x00, // 0x5E
   0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80, // 0x5F
   0x00,0x00,0x03,0x07,0x04,0x00,0x00,0x00, // 0x60
   0x20,0x74,0x54,0x54,0x3C,0x78,0x40,0x00, // 0x61
   0x41,0x3F,0x7F,0x44,0x44,0x7C,0x38,0x00, // 0x62
   0x38,0x7C,0x44,0x44,0x6C,0x28,0x00,0x00, // 0x63
   0x30,0x78,0x48,0x49,0x3F,0x7F,0x40,0x00, // 0x64
   0x38,0x7C,0x54,0x54,0x5C,0x18,0x00,0x00, // 0x65
   0x48,0x7E,0x7F,0x49,0x03,0x02,0x00,0x00, // 0x66
   0x98,0xBC,0xA4,0xA4,0xF8,0x7C,0x04,0x00, // 0x67
   0x41,0x7F,0x7F,0x08,0x04,0x7C,0x78,0x00, // 0x68
   0x00,0x44,0x7D,0x7D,0x40,0x00,0x00,0x00, // 0x69
   0x40,0xC4,0x84,0xFD,0x7D,0x00,0x00,0x00, // 0x6A
   0x41,0x7F,0x7F,0x10,0x38,0x6C,0x44,0x00, // 0x6B
   0x00,0x41,0x7F,0x7F,0x40,0x00,0x00,0x00, // 0x6C
   0x7C,0x7C,0x0C,0x18,0x0C,0x7C,0x78,0x00, // 0x6D
   0x7C,0x7C,0x04,0x04,0x7C,0x78,0x00,0x00, // 0x6E
   0x38,0x7C,0x44,0x44,0x7C,0x38,0x00,0x00, // 0x6F
   0x84,0xFC,0xF8,0xA4,0x24,0x3C,0x18,0x00, // 0x70
   0x18,0x3C,0x24,0xA4,0xF8,0xFC,0x84,0x00, // 0x71
   0x44,0x7C,0x78,0x44,0x1C,0x18,0x00,0x00, // 0x72
   0x48,0x5C,0x54,0x54,0x74,0x24,0x00,0x00, // 0x73
   0x00,0x04,0x3E,0x7F,0x44,0x24,0x00,0x00, // 0x74
   0x3C,0x7C,0x40,0x40,0x3C,0x7C,0x40,0x00, // 0x75
   0x1C,0x3C,0x60,0x60,0x3C,0x1C,0x00,0x00, // 0x76
   0x3C,0x7C,0x60,0x30,0x60,0x7C,0x3C,0x00, // 0x77
   0x44,0x6C,0x38,0x10,0x38,0x6C,0x44,0x00, // 0x78
   0x9C,0xBC,0xA0,0xA0,0xFC,0x7C,0x00,0x00, // 0x79
   0x4C,0x64,0x74,0x5C,0x4C,0x64,0x00,0x00, // 0x7A
   0x08,0x08,0x3E,0x77,0x41,0x41,0x00,0x00, // 0x7B
   0x00,0x00,0x00,0x77,0x77,0x00,0x00,0x00, // 0x7C
   0x41,0x41,0x77,0x3E,0x08,0x08,0x00,0x00, // 0x7D
   0x02,0x03,0x01,0x03,0x02,0x03,0x01,0x00, // 0x7E
   0x78,0x7C,0x46,0x43,0x46,0x7C,0x78,0x00}; // 0x7F

#define FONT_W 8
#define FONT_H 2
#define FONT_STRETCHV 1
#define FONT_STRETCHH 0
*/

//#define INVERSE  1
static const uint8_t oled_init_sequence[] PROGMEM = {  // Initialization Sequence  https://cdn-shop.adafruit.com/datasheets/SSD1306.pdf
	//0xAE,       // Display OFF (sleep mode)
	0xD5, 0x80,   // 0x01--set display clock divide ratio/oscillator frequency   OK? (0x80 (or >=0x10) needed when multiplex ration set to 0x3F)
  #ifdef CONDENSED
	0xA8, 0x3F,   // Set multiplex ratio(1 to 64)   128x64
  #else
	0xA8, 0x1F,   // Set multiplex ratio(1 to 64)   128x32
  #endif
	0xD3, 0x00,   // Set display offset. 00 = no offset
  #ifndef OLED_SH1106  // for SSD1306 only:
	0x40,         // Set display start line address
	0x8D, 0x14,   // Set charge pump, internal VCC
	0x20, 0x02,   // Set Memory Addressing; 0=Horizontal Mode; 1=Vertical Mode; 2=Page Mode
	0xA4,     // Output RAM to Display  (display all on resume)   0xA4=Output follows RAM content; 0xA5,Output ignores RAM content
  #endif //!OLED_SH1106
	0xA1,         // Set Segment Re-map. A0=column 0 mapped to SEG0; A1=column 127 mapped to SEG0. Flip Horizontally
	0xC8,         // Set COM Output Scan Direction.  Flip Veritically.
  #ifdef CONDENSED
	0xDA, 0x12,   // Set com pins hardware configuration  128x64
  #else
	0xDA, 0x02,   // Set com pins hardware configuration  128x32
  #endif
	0x81, 0x80,   // Set contrast control register
	0xDB, 0x40,   // Set vcomh 0x20 = 0.77xVcc
	0xD9, 0xF1,   // 0xF1=brighter //0x22 Set pre-charge period
	0xB0 | 0x0,   // Set page address, 0-7
  #ifdef OLED_SH1106
	0xAD, 0x8B,   // SH1106 Set pump mode: pump ON
	0x30 | 0x2,   // SH1106 Pump voltage 8.0V
  #endif //OLED_SH1106
  #ifdef INVERSE
	0xA7,         // Set display mode: Inverse
  #else
	0xA6,         // Set display mode: Normal
  #endif
	//0x00,       // Set low nibble of column address
	//0x10,       // Set high nibble of column address
	0xAF,         // Display ON
};

class OLEDDevice : public Print {  // https://www.buydisplay.com/download/manual/ER-OLED0.91-3_Series_Datasheet.pdf
public:
#define OLED_ADDR    0x3C  // Slave address
#define OLED_PAGES   4
#define OLED_COMMAND 0x00
#define OLED_DATA    0x40
	uint8_t oledX = 0, oledY = 0;
	uint8_t renderingFrame = 0xB0;
	bool wrap = false;
	void cmd(uint8_t b) {
		Wire.beginTransmission(OLED_ADDR); Wire.write(OLED_COMMAND);
		Wire.write(b);
		Wire.endTransmission();
	}
	void begin(uint8_t cols, uint8_t rows, uint8_t charsize = 0) {
		Wire.begin();
		Wire.beginTransmission(OLED_ADDR); Wire.write(OLED_COMMAND);
		for (uint8_t i = 0; i < sizeof(oled_init_sequence); i++) {
			Wire.write(pgm_read_byte(&oled_init_sequence[i]));
		}
		Wire.endTransmission();
		delayMicroseconds(100);
#ifdef CONDENSED
		for (uint8_t y = 0; y != rows; y++) for (uint8_t x = 0; x != cols; x++) { setCursor(x, y); write(' '); }  // clear
#endif
	}
	bool curs = false;
	void noCursor() { curs = false; }
	void cursor() { curs = true; }
	void noDisplay() { cmd(0xAE); }
	void createChar(uint8_t l, uint8_t glyph[]) {}

	void _setCursor(uint8_t x, uint8_t y) {
		oledX = x; oledY = y;
		Wire.beginTransmission(OLED_ADDR); Wire.write(OLED_COMMAND);
		Wire.write(renderingFrame | (oledY & 0x07));
		uint8_t _oledX = oledX;
#ifdef OLED_SH1106
		_oledX += 2; // SH1106 is a 132x64 controller.  Use middle 128 columns.
#endif
		Wire.write(0x10 | ((_oledX & 0xf0) >> 4));
		Wire.write(_oledX & 0x0f);
		Wire.endTransmission();
	}
	void drawCursor(bool en) {
		//_setCursor(oledX, oledY + (FONT_W/(FONT_STRETCHH+1)));
		Wire.beginTransmission(OLED_ADDR); Wire.write(OLED_DATA);
		Wire.write((en) ? 0xf0 : 0x00);  // horizontal line
		Wire.endTransmission();
	}
	void setCursor(uint8_t x, uint8_t y) {
		if (curs) { drawCursor(false); } _setCursor(x * FONT_W, y * FONT_H); if (curs) { drawCursor(true); _setCursor(oledX, oledY); }
	}

	void newLine() {
		oledY += FONT_H;
		if (oledY > OLED_PAGES - FONT_H) {
			oledY = OLED_PAGES - FONT_H;
		}
		_setCursor(0, oledY);
	}

	size_t write(byte c) {
		if ((c == '\n') || (oledX > ((uint8_t)128 - FONT_W))) {
			if (wrap)  newLine();
			return 1;
		}
		//if(oledY > OLED_PAGES - FONT_H) return; //needed?
		c = ((c < 9) ? (c + '~') : c) - ' ';

		uint16_t offset = ((uint16_t)c) * FONT_W / (FONT_STRETCHH + 1) * FONT_H;
		uint8_t line = FONT_H;
		do
		{
			if (FONT_STRETCHV) offset = ((uint16_t)c) * FONT_W / (FONT_STRETCHH + 1) * FONT_H / (2 * FONT_STRETCHV);
			Wire.beginTransmission(OLED_ADDR); Wire.write(OLED_DATA);
			for (uint8_t i = 0; i < (FONT_W / (FONT_STRETCHH + 1)); i++) {
				uint8_t b = pgm_read_byte(&(font[offset++]));
				if (FONT_STRETCHV) {
					uint8_t b2 = 0;
					if (line > 1) for (int i = 0; i != 4; i++) b2 |=/* ! */(b & (1 << i)) ? (1 << (i * 2)) | (1 << ((i * 2) + 1)) : 0x00;
					else         for (int i = 0; i != 4; i++) b2 |=/* ! */(b & (1 << (i + 4))) ? (1 << (i * 2)) | (1 << ((i * 2) + 1)) : 0x00;
					Wire.write(b2);
					if (FONT_STRETCHH) Wire.write(b2);
				}
				else { Wire.write(b); if (FONT_STRETCHH) Wire.write(b); }
			}
			Wire.endTransmission();
			if (FONT_H == 1) {
				oledX += FONT_W;
			}
			else {
				if (line > 1) {
					_setCursor(oledX, oledY + 1);
				}
				else {
					_setCursor(oledX + FONT_W, oledY - (FONT_H - 1));
				}
			}
		} while (--line);
		return 1;
	}

	void bitmap(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1, const uint8_t bitmap[]) {
		uint16_t j = 0;
		for (uint8_t y = y0; y < y1; y++) {
			_setCursor(x0, y);
			Wire.beginTransmission(OLED_ADDR); Wire.write(OLED_DATA);
			for (uint8_t x = x0; x < x1; x++) {
				Wire.write(pgm_read_byte(&bitmap[j++]));
			}
			Wire.endTransmission();
		}
		setCursor(0, 0);
	}
};

template<class parent>class Display : public parent {  // This class spoofs display contents and cursor state
public:
#ifdef CAT_EXT
	uint8_t x, y;
	bool curs;
	char text[2 * 16 + 1];
	Display() : parent() { clear(); };
	size_t write(uint8_t b) { if ((x < 16) && (y < 2)) { text[y * 16 + x] = ((b < 9) ? "> :*#AB"[b - 1] /*(b + 0x80 - 1)*/ /*(uint8_t[]){ 0xAF, 0x20, 0xB0, 0xB1, 0xB2, 0xA6, 0xE1, 0x20 }[b-1]*/ : b); x++; } return parent::write(b); }
	void setCursor(uint8_t _x, uint8_t _y) { x = _x; y = _y; parent::setCursor(_x, _y); }
	void cursor() { curs = true; parent::cursor(); }
	void noCursor() { curs = false; parent::noCursor(); }
	void clear() { for (uint8_t i = 0; i != 2 * 16; i++) text[i] = ' '; text[2 * 16] = '\0'; x = 0; y = 0; }
#endif //CAT_EXT
};
//#define BLIND 1             // uSDX in head-less operation
#ifdef BLIND
class Blind : public Print {  // This class is a dummy LCD replacement
public:
	size_t write(uint8_t b) {}
	void setCursor(uint8_t _x, uint8_t _y) {}
	void cursor() {}
	void noCursor() {}
	void begin(uint8_t x = 0, uint8_t y = 0) {}
	void noDisplay() {}
	void createChar(uint8_t l, uint8_t glyph[]) {}
};
Display<Blind> lcd;
#else
#ifdef OLED
Display<OLEDDevice> lcd;
#else
Display<LCD> lcd;     // highly-optimized LCD driver, OK for QCX supplied displays
//LCD_ lcd;  // slower LCD, suitable for non-QCX supplied displays
//#include <LiquidCrystal.h>
//LiquidCrystal lcd(LCD_RS, LCD_EN, LCD_D4, LCD_D5, LCD_D6, LCD_D7);
#endif
#endif

#ifdef DEBUG_G8RDI
int fir_value = 0;
#endif

volatile int8_t encoder_val = 0;
volatile int8_t encoder_step = 0;
static uint8_t last_state;
ISR(PCINT2_vect) {  // Interrupt on rotary encoder turn
	//noInterrupts();
	//PCMSK2 &= ~((1 << PCINT22) | (1 << PCINT23));  // mask ROT_A, ROT_B interrupts
	switch (last_state = (last_state << 4) | (_digitalRead(ROT_B) << 1) | _digitalRead(ROT_A)) { //transition  (see: https://www.allaboutcircuits.com/projects/how-to-use-a-rotary-encoder-in-a-mcu-based-project/  )
		//#define ENCODER_ENHANCED_RESOLUTION  1
#ifdef ENCODER_ENHANCED_RESOLUTION // Option: enhance encoder from 24 to 96 steps/revolution, see: appendix 1, https://www.sdr-kits.net/documents/PA0KLT_Manual.pdf
	case 0x31: case 0x10: case 0x02: case 0x23: encoder_val++; break;
	case 0x32: case 0x20: case 0x01: case 0x13: encoder_val--; break;
#else
//    case 0x31: case 0x10: case 0x02: case 0x23: if(encoder_step < 0) encoder_step = 0; encoder_step++; if(encoder_step >  3){ encoder_step = 0; encoder_val++; } break;  // encoder processing for additional immunity to weared-out rotary encoders
//    case 0x32: case 0x20: case 0x01: case 0x13: if(encoder_step > 0) encoder_step = 0; encoder_step--; if(encoder_step < -3){ encoder_step = 0; encoder_val--; } break;
	case 0x23:  encoder_val++; break;
	case 0x32:  encoder_val--; break;
#endif
	}
	//PCMSK2 |= (1 << PCINT22) | (1 << PCINT23);  // allow ROT_A, ROT_B interrupts
	//interrupts();
}
void encoder_setup()
{
	pinMode(ROT_A, INPUT_PULLUP);
	pinMode(ROT_B, INPUT_PULLUP);
	PCMSK2 |= (1 << PCINT22) | (1 << PCINT23); // interrupt-enable for ROT_A, ROT_B pin changes; see https://github.com/EnviroDIY/Arduino-SDI-12/wiki/2b.-Overview-of-Interrupts
	PCICR |= (1 << PCIE2);
	last_state = (_digitalRead(ROT_B) << 1) | _digitalRead(ROT_A);
	interrupts();
}
/*
class Encoder {
public:
  volatile int8_t val = 0;
  volatile int8_t step = 0;
  uint8_t last_state;
  Encoder(){
  pinMode(ROT_A, INPUT_PULLUP);
  pinMode(ROT_B, INPUT_PULLUP);
  PCMSK2 |= (1 << PCINT22) | (1 << PCINT23); // interrupt-enable for ROT_A, ROT_B pin changes; see https://github.com/EnviroDIY/Arduino-SDI-12/wiki/2b.-Overview-of-Interrupts
  PCICR |= (1 << PCIE2);
  last_state = (_digitalRead(ROT_B) << 1) | _digitalRead(ROT_A);
  sei();
  }
  void event(){
  switch(last_state = (last_state << 4) | (_digitalRead(ROT_B) << 1) | _digitalRead(ROT_A)){ //transition  (see: https://www.allaboutcircuits.com/projects/how-to-use-a-rotary-encoder-in-a-mcu-based-project/  )
	case 0x31: case 0x10: case 0x02: case 0x23: if(step < 0) step = 0; step++; if(step >  3){ step = 0; val++; } break;
	case 0x32: case 0x20: case 0x01: case 0x13: if(step > 0) step = 0; step--; if(step < -3){ step = 0; val--; } break;
  }
  }
};
Encoder enc;
ISR(PCINT2_vect){  // Interrupt on rotary encoder turn
  enc.event();
}*/

// I2C communication starts with a START condition, multiple single byte-transfers (MSB first) followed by an ACK/NACK and stops with a STOP condition;
// during data-transfer SDA may only change when SCL is LOW, during a START/STOP condition SCL is HIGH and SDA goes DOWN for a START and UP for a STOP.
// https://www.ti.com/lit/an/slva704/slva704.pdf
class I2C {
public:
#if(F_MCU > 20900000)
#define I2C_DELAY   6
#else
#define I2C_DELAY   4    // Determines I2C Speed (2=939kb/s (too fast!!); 3=822kb/s; 4=731kb/s; 5=658kb/s; 6=598kb/s). Increase this value when you get I2C tx errors (E05); decrease this value when you get a CPU overload (E01). An increment eats ~3.5% CPU load; minimum value is 3 on my QCX, resulting in 84.5% CPU load
#endif
#define I2C_DDR DDRC     // Pins for the I2C bit banging
#define I2C_PIN PINC
#define I2C_PORT PORTC
#define I2C_SDA (1 << 4) // PC4
#define I2C_SCL (1 << 5) // PC5
#define DELAY(n) for(uint8_t i = 0; i != n; i++) asm("nop");
#define I2C_SDA_GET() I2C_PIN & I2C_SDA
#define I2C_SCL_GET() I2C_PIN & I2C_SCL
#define I2C_SDA_HI() I2C_DDR &= ~I2C_SDA;
#define I2C_SDA_LO() I2C_DDR |=  I2C_SDA;
#define I2C_SCL_HI() I2C_DDR &= ~I2C_SCL; DELAY(I2C_DELAY);
#define I2C_SCL_LO() I2C_DDR |=  I2C_SCL; DELAY(I2C_DELAY);

	I2C() {
		I2C_PORT &= ~(I2C_SDA | I2C_SCL);
		I2C_SCL_HI();
		I2C_SDA_HI();
#ifndef RS_HIGH_ON_IDLE
		suspend();
#endif
	}
	~I2C() {
		I2C_PORT &= ~(I2C_SDA | I2C_SCL);
		I2C_DDR &= ~(I2C_SDA | I2C_SCL);
	}
	inline void start() {
#ifdef RS_HIGH_ON_IDLE
		I2C_SDA_LO();
#else
		resume();  //prepare for I2C
#endif
		I2C_SCL_LO();
		I2C_SDA_HI();
	}
	inline void stop() {
		I2C_SDA_LO();   // ensure SDA is LO so STOP-condition can be initiated by pulling SCL HI (in case of ACK it SDA was already LO, but for a delayed ACK or NACK it is not!)
		I2C_SCL_HI();
		I2C_SDA_HI();
		I2C_DDR &= ~(I2C_SDA | I2C_SCL); // prepare for a start: pull-up both SDA, SCL
#ifndef RS_HIGH_ON_IDLE
		suspend();
#endif
	}
#define SendBit(data, mask) \
    if(data & mask){ \
      I2C_SDA_HI();  \
    } else {         \
      I2C_SDA_LO();  \
    }                \
    I2C_SCL_HI();    \
    I2C_SCL_LO();
	/*#define SendByte(data) \
	  SendBit(data, 1 << 7) \
	  SendBit(data, 1 << 6) \
	  SendBit(data, 1 << 5) \
	  SendBit(data, 1 << 4) \
	  SendBit(data, 1 << 3) \
	  SendBit(data, 1 << 2) \
	  SendBit(data, 1 << 1) \
	  SendBit(data, 1 << 0) \
	  I2C_SDA_HI();  // recv ACK \
	  DELAY(I2C_DELAY);     \
	  I2C_SCL_HI();         \
	  I2C_SCL_LO();*/
	inline void SendByte(uint8_t data) {
		SendBit(data, 1 << 7);
		SendBit(data, 1 << 6);
		SendBit(data, 1 << 5);
		SendBit(data, 1 << 4);
		SendBit(data, 1 << 3);
		SendBit(data, 1 << 2);
		SendBit(data, 1 << 1);
		SendBit(data, 1 << 0);
		I2C_SDA_HI();  // recv ACK
		DELAY(I2C_DELAY);
		I2C_SCL_HI();
		I2C_SCL_LO();
	}
	inline uint8_t RecvBit(uint8_t mask) {
		I2C_SCL_HI();
		uint16_t i = 60000;
		for (; !(I2C_SCL_GET()) && i; i--);  // wait util slave release SCL to HIGH (meaning data valid), or timeout at 3ms
		//if(!i){ lcd.setCursor(0, 1); lcd.print(F("E07 I2C timeout")); }
		uint8_t data = I2C_SDA_GET();
		I2C_SCL_LO();
		return (data) ? mask : 0;
	}
	inline uint8_t RecvByte(uint8_t last) {
		uint8_t data = 0;
		data |= RecvBit(1 << 7);
		data |= RecvBit(1 << 6);
		data |= RecvBit(1 << 5);
		data |= RecvBit(1 << 4);
		data |= RecvBit(1 << 3);
		data |= RecvBit(1 << 2);
		data |= RecvBit(1 << 1);
		data |= RecvBit(1 << 0);
		if (last) {
			I2C_SDA_HI();  // NACK
		}
		else {
			I2C_SDA_LO();  // ACK
		}
		DELAY(I2C_DELAY);
		I2C_SCL_HI();
		I2C_SDA_HI();    // restore SDA for read
		I2C_SCL_LO();
		return data;
	}
	inline void resume() {
#ifdef LCD_RS_PORTIO
		I2C_PORT &= ~I2C_SDA; // pin sharing SDA/LCD_RS mitigation
#endif
	}
	inline void suspend() {
		I2C_SDA_LO();         // pin sharing SDA/LCD_RS: pull-down LCD_RS; QCXLiquidCrystal require this for any operation
	}

	void begin() {};
	void beginTransmission(uint8_t addr) { start(); SendByte(addr << 1); };
	bool write(uint8_t byte) { SendByte(byte); return 1; };
	uint8_t endTransmission() { stop(); return 0; };
};

//#define log2(n) (log(n) / log(2))
uint8_t log2(uint16_t x) {
	uint8_t y = 0;
	for (; x >>= 1;) y++;
	return y;
}

// /*
I2C i2c;
class SI5351 {
public:
	volatile int32_t _fout;
	volatile uint8_t _div;  // note: uint8_t asserts fout > 3.5MHz with R_DIV=1
	volatile uint16_t _msa128min512;
	volatile uint32_t _msb128;
	//volatile uint32_t _mod;
	volatile uint8_t pll_regs[8];

#define BB0(x) ((uint8_t)(x))           // Bash byte x of int32_t
#define BB1(x) ((uint8_t)((x)>>8))
#define BB2(x) ((uint8_t)((x)>>16))

#define FAST __attribute__((optimize("Ofast")))

	volatile uint32_t fxtal = F_XTAL;

#define NEW_TX 1
#ifdef NEW_TX

	inline void FAST freq_calc_fast(int32_t df)  // note: relies on cached variables: _msb128, _msa128min512, _div, _fout, fxtal
	{
#define _MSC  0x10000
		uint32_t msb128 = _msb128 + ((int64_t)(_div * (int32_t)df) * _MSC * 128) / fxtal;

		uint16_t msp1 = _msa128min512 + msb128 / _MSC; // = 128 * _msa + msb128 / _MSC - 512;
		uint16_t msp2 = msb128; // = msb128 % _MSC;  assuming MSC is covering exact uint16_t so the mod operation can dissapear (and the upper BB2 byte) // = msb128 - msb128/_MSC * _MSC;

		//pll_regs[0] = BB1(msc);  // 3 regs are constant
		//pll_regs[1] = BB0(msc);
		//pll_regs[2] = BB2(msp1);
		//pll_regs[3] = BB1(msp1);
		pll_regs[4] = BB0(msp1);
		pll_regs[5] = ((_MSC & 0xF0000) >> (16 - 4))/*|BB2(msp2)*/; // top nibble MUST be same as top nibble of _MSC !  assuming that BB2(msp2) is always 0 -> so reg is constant
		pll_regs[6] = BB1(msp2);
		pll_regs[7] = BB0(msp2);
	}

	inline void SendPLLRegisterBulk() {
		i2c.start();
		i2c.SendByte(SI5351_ADDR << 1);
		i2c.SendByte(26 + 0 * 8 + 4);  // Write to PLLA
		//i2c.SendByte(26+1*8 + 4);  // Write to PLLB
		i2c.SendByte(pll_regs[4]);
		i2c.SendByte(pll_regs[5]);
		i2c.SendByte(pll_regs[6]);
		i2c.SendByte(pll_regs[7]);
		i2c.stop();
	}
#else  // !NEW_TX
	inline void FAST freq_calc_fast(int32_t df)  // note: relies on cached variables: _msb128, _msa128min512, _div, _fout, fxtal
	{
#define _MSC  0x80000  //0x80000: 98% CPU load   0xFFFFF: 114% CPU load
		uint32_t msb128 = _msb128 + ((int64_t)(_div * (int32_t)df) * _MSC * 128) / fxtal;
		//uint32_t msb128 = ((int64_t)(_div * (int32_t)df + _mod) * _MSC * 128) / fxtal; // @pre: 14<=_div<=144, |df|<=5000, _mod<=1800e3 (for fout<30M), _MSC=524288

		//#define _MSC  (F_XTAL/128)   // MSC exact multiple of F_XTAL (and maximized to fit in max. span 1048575)
		//uint32_t msb128 = (_div * (int32_t)df + _mod);

		//#define _MSC  0xFFFFF  // Old algorithm 114% CPU load, shortcut for a fixed fxtal=27e6
		//register uint32_t xmsb = (_div * (_fout + (int32_t)df)) % fxtal;  // xmsb = msb * fxtal/(128 * _MSC);
		//uint32_t msb128 = xmsb * 5*(32/32) - (xmsb/32);  // msb128 = xmsb * 159/32, where 159/32 = 128 * 0xFFFFF / fxtal; fxtal=27e6

		//#define _MSC  (F_XTAL/128)  // 114% CPU load  perfect alignment
		//uint32_t msb128 = (_div * (_fout + (int32_t)df)) % fxtal;

		uint32_t msp1 = _msa128min512 + msb128 / _MSC;  // = 128 * _msa + msb128 / _MSC - 512;
		uint32_t msp2 = msb128 % _MSC;  // = msb128 - msb128/_MSC * _MSC;
		//uint32_t msp1 = _msa128min512;  // = 128 * _msa + msb128 / _MSC - 512;  assuming msb128 < _MSC, so that msp1 is constant
		//uint32_t msp2 = msb128;  // = msb128 - msb128/_MSC * _MSC, assuming msb128 < _MSC

		//pll_regs[0] = BB1(msc);  // 3 regs are constant
		//pll_regs[1] = BB0(msc);
		//pll_regs[2] = BB2(msp1);
		pll_regs[3] = BB1(msp1);
		pll_regs[4] = BB0(msp1);
		pll_regs[5] = ((_MSC & 0xF0000) >> (16 - 4)) | BB2(msp2); // top nibble MUST be same as top nibble of _MSC !
		pll_regs[6] = BB1(msp2);
		pll_regs[7] = BB0(msp2);
	}

	inline void SendPLLRegisterBulk() {
		i2c.start();
		i2c.SendByte(SI5351_ADDR << 1);
		i2c.SendByte(26 + 0 * 8 + 3);  // Write to PLLA
		//i2c.SendByte(26+1*8 + 3);  // Write to PLLB
		i2c.SendByte(pll_regs[3]);
		i2c.SendByte(pll_regs[4]);
		i2c.SendByte(pll_regs[5]);
		i2c.SendByte(pll_regs[6]);
		i2c.SendByte(pll_regs[7]);
		i2c.stop();
	}
#endif // !NEW_TX

	void SendRegister(uint8_t reg, uint8_t* data, uint8_t n) {
		i2c.start();
		i2c.SendByte(SI5351_ADDR << 1);
		i2c.SendByte(reg);
		while (n--) i2c.SendByte(*data++);
		i2c.stop();
	}
	void SendRegister(uint8_t reg, uint8_t val) { SendRegister(reg, &val, 1); }
	int16_t iqmsa; // to detect a need for a PLL reset
	///*
	enum ms_t { PLLA = 0, PLLB = 1, MSNA = -2, MSNB = -1, MS0 = 0, MS1 = 1, MS2 = 2, MS3 = 3, MS4 = 4, MS5 = 5 };

	void ms(int8_t n, uint32_t div_nom, uint32_t div_denom, uint8_t pll = PLLA, uint8_t _int = 0, uint16_t phase = 0, uint8_t rdiv = 0) {
		uint16_t msa; uint32_t msb, msc, msp1, msp2, msp3;
		msa = div_nom / div_denom;     // integer part: msa must be in range 15..90 for PLL, 8+1/1048575..900 for MS
		if (msa == 4) _int = 1;  // To satisfy the MSx_INT=1 requirement of AN619, section 4.1.3 which basically says that for MS divider a value of 4 and integer mode must be used
		msb = (_int) ? 0 : (((uint64_t)(div_nom % div_denom) * _MSC) / div_denom); // fractional part
		msc = (_int) ? 1 : _MSC;
		//lcd.setCursor(0, 0); lcd.print(n); lcd.print(":"); lcd.print(msa); lcd.print(" "); lcd.print(msb); lcd.print(" "); lcd.print(msc); lcd.print(F("    ")); delay(500);
		msp1 = 128 * msa + 128 * msb / msc - 512;
		msp2 = 128 * msb - 128 * msb / msc * msc;
		msp3 = msc;
		uint8_t ms_reg2 = BB2(msp1) | (rdiv << 4) | ((msa == 4) * 0x0C);
		uint8_t ms_regs[8] = { BB1(msp3), BB0(msp3), ms_reg2, BB1(msp1), BB0(msp1), BB2(((msp3 & 0x0F0000) << 4) | msp2), BB1(msp2), BB0(msp2) };

		SendRegister(n * 8 + 42, ms_regs, 8); // Write to MSx
		if (n < 0) {
			SendRegister(n + 16 + 8, 0x80 | (0x40 * _int)); // MSNx PLLn: 0x40=FBA_INT; 0x80=CLKn_PDN
		}
		else {
			//SendRegister(n+16, ((pll)*0x20)|0x0C|0|(0x40*_int));  // MSx CLKn: 0x0C=PLLA,0x2C=PLLB local msynth; 0=2mA; 0x40=MSx_INT; 0x80=CLKx_PDN
			SendRegister(n + 16, ((pll) * 0x20) | 0x0C | 3 | (0x40 * _int));  // MSx CLKn: 0x0C=PLLA,0x2C=PLLB local msynth; 3=8mA; 0x40=MSx_INT; 0x80=CLKx_PDN
			SendRegister(n + 165, (!_int) * phase * msa / 90);      // when using: make sure to configure MS in fractional-mode, perform reset afterwards
		}
	}

	void phase(int8_t n, uint32_t div_nom, uint32_t div_denom, uint16_t phase) { SendRegister(n + 165, phase * (div_nom / div_denom) / 90); }  // when using: make sure to configure MS in fractional-mode!, perform reset afterwards

	void reset() { SendRegister(177, 0xA0); } // 0x20 reset PLLA; 0x80 reset PLLB

	void oe(uint8_t mask) { SendRegister(3, ~mask); } // output-enable mask: CLK2=4; CLK1=2; CLK0=1

	void freq(int32_t fout, uint16_t i, uint16_t q) {  // Set a CLK0,1,2 to fout Hz with phase i, q (on PLLA)
		uint8_t rdiv = 0; // CLK pin sees fout/(2^rdiv)
		if (fout > 300000000) { i /= 3; q /= 3; fout /= 3; }  // for higher freqs, use 3rd harmonic
		if (fout < 500000) { rdiv = 7; fout *= 128; } // Divide by 128 for fout 4..500kHz
		uint16_t d; if (fout < 30000000) d = (16 * fxtal) / fout; else d = (32 * fxtal) / fout;  // Integer part  .. maybe 44?
		if (fout < 3500000) d = (7 * fxtal) / fout;  // PLL at 189MHz to cover 160m (freq>1.48MHz) when using 27MHz crystal
		if (fout > 140000000) d = 4; // for f=140..300MHz; AN619; 4.1.3, this implies integer mode
		if (d % 2) d++;  // even numbers preferred for divider (AN619 p.4 and p.6)
		if ((d * (fout - 5000) / fxtal) != (d * (fout + 5000) / fxtal)) d += 2; // Test if multiplier remains same for freq deviation +/- 5kHz, if not use different divider to make same
		uint32_t fvcoa = d * fout;  // Variable PLLA VCO frequency at integer multiple of fout at around 27MHz*16 = 432MHz
		// si5351 spectral purity considerations: https://groups.io/g/QRPLabs/message/42662

		ms(MSNA, fvcoa, fxtal);                   // PLLA in fractional mode
		//ms(MSNB, fvcoa, fxtal);
		ms(MS0, fvcoa, fout, PLLA, 0, i, rdiv);  // Multisynth stage with integer divider but in frac mode due to phase setting
		ms(MS1, fvcoa, fout, PLLA, 0, q, rdiv);
#ifdef F_CLK2
		freqb(F_CLK2);
#else
		ms(MS2, fvcoa, fout, PLLA, 0, 0, rdiv);
#endif
		if (iqmsa != (((int8_t)i - (int8_t)q) * ((int16_t)(fvcoa / fout)) / 90)) { iqmsa = ((int8_t)i - (int8_t)q) * ((int16_t)(fvcoa / fout)) / 90; reset(); }
		oe(0b00000011);  // output enable CLK0, CLK1

#ifdef x
		ms(MSNA, fvcoa, fxtal);
		ms(MSNB, fvcoa, fxtal);
#define F_DEV 4
		ms(MS0, fvcoa, (fout + F_DEV), PLLA, 0, 0, rdiv);
		ms(MS1, fvcoa, (fout + F_DEV), PLLA, 0, 0, rdiv);
		ms(MS2, fvcoa, fout, PLLA, 0, 0, rdiv);
		reset();
		ms(MS0, fvcoa, fout, PLLA, 0, 0, rdiv);
		delayMicroseconds(F_MCU / 16000000 * 1000000UL / F_DEV);  // Td = 1/(4 * Fdev) phase-shift   https://tj-lab.org/2020/08/27/si5351%e5%8d%98%e4%bd%93%e3%81%a73mhz%e4%bb%a5%e4%b8%8b%e3%81%ae%e7%9b%b4%e4%ba%a4%e4%bf%a1%e5%8f%b7%e3%82%92%e5%87%ba%e5%8a%9b%e3%81%99%e3%82%8b/
		ms(MS1, fvcoa, fout, PLLA, 0, 0, rdiv);
		oe(0b00000011);  // output enable CLK0, CLK1
#endif
		_fout = fout;  // cache
		_div = d;
		_msa128min512 = fvcoa / fxtal * 128 - 512;
		_msb128 = ((uint64_t)(fvcoa % fxtal) * _MSC * 128) / fxtal;
		//_mod = fvcoa % fxtal;
	}

	void freqb(uint32_t fout) {  // Set a CLK2 to fout Hz (on PLLB)
		uint16_t d = (16 * fxtal) / fout;
		if (d % 2) d++;  // even numbers preferred for divider (AN619 p.4 and p.6)
		uint32_t fvcoa = d * fout;  // Variable PLLA VCO frequency at integer multiple of fout at around 27MHz*16 = 432MHz

		ms(MSNB, fvcoa, fxtal);
		ms(MS2, fvcoa, fout, PLLB, 0, 0, 0);
	}

	//*/
	/*
	  void freq(uint32_t fout, uint16_t i, uint16_t q){  // Set a CLK0,1 to fout Hz with phase i, q
		uint16_t msa; uint32_t msb, msc, msp1, msp2, msp3;
		uint8_t rdiv = 0;             // CLK pin sees fout/(2^rdiv)
		if(fout > 300000000){ i/=3; q/=3; fout/=3; }  // for higher freqs, use 3rd harmonic
		if(fout < 500000){ rdiv = 7; fout *= 128; } // Divide by 128 for fout 4..500kHz

		uint16_t d = (16 * fxtal) / fout;  // Integer part
		//if(fout > 7000000) d = (33 * fxtal) / fout;
		if(fout < 3500000) d = (7 * fxtal) / fout;  // PLL at 189MHz to cover 160m (freq>1.48MHz) when using 27MHz crystal

		if( (d * (fout - 5000) / fxtal) != (d * (fout + 5000) / fxtal) ) d++; // Test if multiplier remains same for freq deviation +/- 5kHz, if not use different divider to make same
		if(d % 2) d++;  // even numbers preferred for divider (AN619 p.4 and p.6)
		bool divby4 = 0; if(fout > 140000000){ d = 4; divby4 = 1; } // for f=140..300MHz; AN619; 4.1.3
		uint32_t fvcoa = d * fout;  // Variable PLLA VCO frequency at integer multiple of fout at around 27MHz*16 = 432MHz
		msa = fvcoa / fxtal;     // Integer part of vco/fxtal. msa must be in range 15..90
		msb = ((uint64_t)(fvcoa % fxtal)*_MSC) / fxtal; // fractional part
		msc = _MSC;

		msp1 = 128*msa + 128*msb/msc - 512;
		msp2 = 128*msb - 128*msb/msc * msc;
		msp3 = msc;
		uint8_t pll_regs[8] = { BB1(msp3), BB0(msp3), BB2(msp1), BB1(msp1), BB0(msp1), BB2(((msp3 & 0x0F0000)<<4) | msp2), BB1(msp2), BB0(msp2) };
		SendRegister(26+0*8, pll_regs, 8); // Write to PLLA
		SendRegister(26+1*8, pll_regs, 8); // Write to PLLB
		SendRegister(16+6, 0x80); // PLLA in fractional mode; 0x40=FBA_INT; 0x80=CLK6_PDN
		SendRegister(16+7, 0x80); // PLLB in fractional mode; 0x40=FBB_INT; 0x80=CLK7_PDN

		msa = fvcoa / fout;     // Integer part of vco/fout. msa must be in range 6..127 (support for integer and initial phase offset)
		//lcd.setCursor(0, 0); lcd.print(fvcoa/fxtal); lcd.print(" "); lcd.print(msb); lcd.print(" "); lcd.print(msa); lcd.print(F("     "));
		msp1 = (divby4) ? 0 : (128*msa - 512);     // msp1 and msp2=0, msp3=1, integer division
		msp2 = 0;
		msp3 = 1;
		uint8_t ms_regs[8] = { BB1(msp3), BB0(msp3), BB2(msp1) | (rdiv<<4) | (divby4*0x0C), BB1(msp1), BB0(msp1), BB2(((msp3 & 0x0F0000)<<4) | msp2), BB1(msp2), BB0(msp2) };
		SendRegister(42+0*8, ms_regs, 8); // Write to MS0
		SendRegister(42+1*8, ms_regs, 8); // Write to MS1
		SendRegister(42+2*8, ms_regs, 8); // Write to MS2
		SendRegister(16+0, 0x0C|3|(0x40*divby4));  // CLK0: 0x0C=PLLA local msynth; 3=8mA; 0x40=MS0_INT; 0x80=CLK0_PDN
		SendRegister(16+1, 0x0C|3|(0x40*divby4));  // CLK1: 0x0C=PLLA local msynth; 3=8mA; 0x40=MS1_INT; 0x80=CLK1_PDN
		SendRegister(16+2, 0x2C|3|(0x40*divby4));  // CLK2: 0x2C=PLLB local msynth; 3=8mA; 0x40=MS2_INT; 0x80=CLK2_PDN
		SendRegister(165, i * msa / 90);  // CLK0: I-phase (on change -> Reset PLL)
		SendRegister(166, q * msa / 90);  // CLK1: Q-phase (on change -> Reset PLL)
		if(iqmsa != ((i-q)*msa/90)){ iqmsa = (i-q)*msa/90; SendRegister(177, 0xA0); } // 0x20 reset PLLA; 0x80 reset PLLB
		SendRegister(3, 0b11111100);      // Enable/disable clock

		_fout = fout;  // cache
		_div = d;
		_msa128min512 = fvcoa / fxtal * 128 - 512;
		_msb128=((uint64_t)(fvcoa % fxtal)*_MSC*128) / fxtal;
	  }
	*/
	uint8_t RecvRegister(uint8_t reg) {
		i2c.start();  // Data write to set the register address
		i2c.SendByte(SI5351_ADDR << 1);
		i2c.SendByte(reg);
		i2c.stop();
		i2c.start(); // Data read to retrieve the data from the set address
		i2c.SendByte((SI5351_ADDR << 1) | 1);
		uint8_t data = i2c.RecvByte(true);
		i2c.stop();
		return data;
	}
	void powerDown() {
		SendRegister(3, 0b11111111); // Disable all CLK outputs
		SendRegister(24, 0b00000000); // Disable state: LOW state when disabled
		SendRegister(25, 0b00000000); // Disable state: LOW state when disabled
		for (int addr = 16; addr != 24; addr++) SendRegister(addr, 0b10000000);  // Conserve power when output is disabled
		SendRegister(187, 0);        // Disable fanout (power-safe)
		// To initialise things as they should:
		SendRegister(149, 0);        // Disable spread spectrum enable
		SendRegister(183, 0b11010010);  // Internal CL = 10 pF (default)
	}
#define SI_CLK_OE 3

};
static SI5351 si5351;
// */

 /*
class SI5351 : public I2C {
public:
  #define SI_I2C_ADDR 0x60  // SI5351A I2C address: 0x60 for SI5351A-B-GT; 0x62 for SI5351A-B-04486-GT; 0x6F for SI5351A-B02075-GT; see here for other variants: https://www.silabs.com/TimingUtility/timing-download-document.aspx?OPN=Si5351A-B02075-GT&OPNRevision=0&FileType=PublicAddendum
  #define SI_CLK_OE 3     // Register definitions
  #define SI_CLK0_CONTROL 16
  #define SI_CLK1_CONTROL 17
  #define SI_CLK2_CONTROL 18
  #define SI_SYNTH_PLL_A 26
  #define SI_SYNTH_PLL_B 34
  #define SI_SYNTH_MS_0 42
  #define SI_SYNTH_MS_1 50
  #define SI_SYNTH_MS_2 58
  #define SI_CLK0_PHOFF 165
  #define SI_CLK1_PHOFF 166
  #define SI_CLK2_PHOFF 167
  #define SI_PLL_RESET 177
  #define SI_MS_INT 0b01000000  // Clock control
  #define SI_CLK_SRC_PLL_A 0b00000000
  #define SI_CLK_SRC_PLL_B 0b00100000
  #define SI_CLK_SRC_MS 0b00001100
  #define SI_CLK_IDRV_8MA 0b00000011
  #define SI_CLK_INV 0b00010000
  volatile uint32_t fxtal = 27004300;  //myqcx1:27003980 myqcx2:27004900  Actual crystal frequency of 27MHz XTAL2 for CL = 10pF (default), calibrate your QCX 27MHz crystal frequency here
  #define SI_PLL_FREQ (16*fxtal)  //900000000, with 432MHz(=16*27M) PLL freq, usable range is 3.46..100MHz

  volatile uint8_t prev_divider;
  volatile int32_t raw_freq;
  volatile uint8_t divider;  // note: because of int8 only freq > 3.6MHz can be covered for R_DIV=1
  volatile uint8_t mult;
  volatile uint8_t pll_regs[8];
  volatile int32_t iqmsa;
  volatile int32_t pll_freq;   // temporary

  SI5351(){
  init();
  iqmsa = 0;
  }
  uint8_t RecvRegister(uint8_t reg)
  {
  // Data write to set the register address
  start();
  SendByte(SI_I2C_ADDR << 1);
  SendByte(reg);
  stop();
  // Data read to retrieve the data from the set address
  start();
  SendByte((SI_I2C_ADDR << 1) | 1);
  uint8_t data = RecvByte(true);
  stop();
  return data;
  }
  void SendRegister(uint8_t reg, uint8_t data)
  {
  start();
  SendByte(SI_I2C_ADDR << 1);
  SendByte(reg);
  SendByte(data);
  stop();
  }
  // Set up MultiSynth for register reg=MSNA, MNSB, MS0-5 with fractional divider, num and denom and R divider (for MSn, not for MSNA, MSNB)
  // divider is 15..90 for MSNA, MSNB,  divider is 8..900 (and in addition 4,6 for integer mode) for MS[0-5]
  // num is 0..1,048,575 (0xFFFFF)
  // denom is 0..1,048,575 (0xFFFFF)
  // num = 0, denom = 1 forces an integer value for the divider
  // r_div = 1..128 (1,2,4,8,16,32,64,128)
  void SetupMultisynth(uint8_t reg, uint8_t divider, uint32_t num, uint32_t denom, uint8_t r_div)
  {
  uint32_t P1; // Synth config register P1
  uint32_t P2; // Synth config register P2
  uint32_t P3; // Synth config register P3

  P1 = (uint32_t)(128 * ((float)num / (float)denom));
  P1 = (uint32_t)(128 * (uint32_t)(divider) + P1 - 512);
  P2 = (uint32_t)(128 * ((float)num / (float)denom));
  P2 = (uint32_t)(128 * num - denom * P2);
  P3 = denom;

  SendRegister(reg + 0, (P3 & 0x0000FF00) >> 8);
  SendRegister(reg + 1, (P3 & 0x000000FF));
  SendRegister(reg + 2, (P1 & 0x00030000) >> 16 | ((int)log2(r_div) << 4) );
  SendRegister(reg + 3, (P1 & 0x0000FF00) >> 8);
  SendRegister(reg + 4, (P1 & 0x000000FF));
  SendRegister(reg + 5, ((P3 & 0x000F0000) >> 12) | ((P2 & 0x000F0000) >> 16));
  SendRegister(reg + 6, (P2 & 0x0000FF00) >> 8);
  SendRegister(reg + 7, (P2 & 0x000000FF));
  }
  inline void SendPLLRegisterBulk()  // fast freq change of PLLB, takes about [ 2 + 7*(8+1) + 2 ] / 840000 = 80 uS
  {
  start();
  SendByte(SI_I2C_ADDR << 1);
  SendByte(SI_SYNTH_PLL_A + 3);  // Skip the first three pll_regs bytes (first two always 0xFF and third not often changing
  //SendByte(SI_SYNTH_PLL_B + 3);  // Skip the first three pll_regs bytes (first two always 0xFF and third not often changing
  SendByte(pll_regs[3]);
  SendByte(pll_regs[4]);
  SendByte(pll_regs[5]);
  SendByte(pll_regs[6]);
  SendByte(pll_regs[7]);
  stop();
  }

  #define FAST __attribute__((optimize("Ofast")))

  // this function relies on cached (global) variables: divider, mult, raw_freq, pll_regs
  inline void FAST freq_calc_fast(int16_t freq_offset)
  { // freq_offset is relative to freq set in freq(freq)
  // uint32_t num128 = ((divider * (raw_freq + offset)) % fxtal) * (float)(0xFFFFF * 128) / fxtal;
  // Above definition (for fxtal=27.00491M) can be optimized by pre-calculating factor (0xFFFFF*128)/fxtal (=4.97) as integer constant (5) and
  // substracting the rest error factor (0.03). Note that the latter is shifted left (0.03<<6)=2, while the other term is shifted right (>>6)
  register int32_t z = ((divider * (raw_freq + freq_offset)) % fxtal);
  register int32_t z2 = -(z >> 5);
  int32_t num128 = (z * 5) + z2;

  // Set up specified PLL with mult, num and denom: mult is 15..90, num128 is 0..128*1,048,575 (128*0xFFFFF), denom is 0..1,048,575 (0xFFFFF)
  uint32_t P1 = 128 * mult + (num128 / 0xFFFFF) - 512;
  uint32_t P2 = num128 % 0xFFFFF;
  //pll_regs[0] = 0xFF;
  //pll_regs[1] = 0xFF;
  //pll_regs[2] = (P1 >> 14) & 0x0C;
  pll_regs[3] = P1 >> 8;
  pll_regs[4] = P1;
  pll_regs[5] = 0xF0 | (P2 >> 16);
  pll_regs[6] = P2 >> 8;
  pll_regs[7] = P2;
  }
  uint16_t div(uint32_t num, uint32_t denom, uint32_t* b, uint32_t* c)
  { // returns a + b / c = num / denom, where a is the integer part and b and c is the optional fractional part 20 bits each (range 0..1048575)
  uint16_t a = num / denom;
  if(b && c){
	uint64_t l = num % denom;
	l <<= 20; l--;  // l *= 1048575;
	l /= denom;     // normalize
	*b = l;
	*c = 0xFFFFF;    // for simplicity set c to the maximum 1048575
  }
  return a;
  }
  void freq(uint32_t freq, uint8_t i, uint8_t q)
  { // Fout = Fvco / (R * [MSx_a + MSx_b/MSx_c]),  Fvco = Fxtal * [MSPLLx_a + MSPLLx_b/MSPLLx_c]; MSx as integer reduce spur
  //uint8_t r_div = (freq > (SI_PLL_FREQ/256/1)) ? 1 : (freq > (SI_PLL_FREQ/256/32)) ? 32 : 128; // helps divider to be in range
  uint8_t r_div = (freq < 500000) ? 128 : 1;
  freq *= r_div;  // take r_div into account, now freq is in the range 1MHz to 150MHz
  raw_freq = freq;   // cache frequency generated by PLL and MS stages (excluding R divider stage); used by freq_calc_fast()

  divider = SI_PLL_FREQ / freq;  // Calculate the division ratio. 900,000,000 is the maximum internal PLL freq (official range 600..900MHz but can be pushed to 300MHz..~1200Mhz)
  if(divider % 2) divider--;  // divider in range 8.. 900 (including 4,6 for integer mode), even numbers preferred. Note that uint8 datatype is used, so 254 is upper limit
  if( (divider * (freq - 5000) / fxtal) != (divider * (freq + 5000) / fxtal) ) divider -= 2; // Test if multiplier remains same for freq deviation +/- 5kHz, if not use different divider to make same
  pll_freq = divider * freq; // Calculate the pll_freq: the divider * desired output freq
  uint32_t num, denom;
  mult = div(pll_freq, fxtal, &num, &denom); // Determine the mult to get to the required pll_freq (in the range 15..90)

  // Set up specified PLL with mult, num and denom: mult is 15..90, num is 0..1,048,575 (0xFFFFF), denom is 0..1,048,575 (0xFFFFF)
  // Set up PLL A and PLL B with the calculated  multiplication ratio
  SetupMultisynth(SI_SYNTH_PLL_A, mult, num, denom, 1);
  SetupMultisynth(SI_SYNTH_PLL_B, mult, num, denom, 1);
  //if(denom == 1) SendRegister(22, SI_MSx_INT); // FBA_INT: MSNA operates in integer mode
  //if(denom == 1) SendRegister(23, SI_MSx_INT); // FBB_INT: MSNB operates in integer mode
  // Set up MultiSynth 0,1,2 with the calculated divider, from 4, 6..1800.
  // The final R division stage can divide by a power of two, from 1..128
  // if you want to output frequencies below 1MHz, you have to use the final R division stage
  SetupMultisynth(SI_SYNTH_MS_0, divider, 0, 1, r_div);
  SetupMultisynth(SI_SYNTH_MS_1, divider, 0, 1, r_div);
  SetupMultisynth(SI_SYNTH_MS_2, divider, 0, 1, r_div);
  //if(prev_divider != divider){ lcd.setCursor(0, 0); lcd.print(divider); lcd.print(F("     "));
  // Set I/Q phase
  SendRegister(SI_CLK0_PHOFF, i * divider / 90); // one LSB equivalent to a time delay of Tvco/4 range 0..127
  SendRegister(SI_CLK1_PHOFF, q * divider / 90); // one LSB equivalent to a time delay of Tvco/4 range 0..127
  // Switch on the CLK0, CLK1 output to be PLL A and set multiSynth0, multiSynth1 input (0x0F = SI_CLK_SRC_MS | SI_CLK_IDRV_8MA)
  SendRegister(SI_CLK0_CONTROL, 0x0F | SI_MS_INT | SI_CLK_SRC_PLL_A);
  SendRegister(SI_CLK1_CONTROL, 0x0F | SI_MS_INT | SI_CLK_SRC_PLL_A);
  // Switch on the CLK2 output to be PLL B and set multiSynth2 input
  SendRegister(SI_CLK2_CONTROL, 0x0F | SI_MS_INT | SI_CLK_SRC_PLL_B);
  SendRegister(SI_CLK_OE, 0b11111100); // Enable CLK1|CLK0
  // Reset the PLL. This causes a glitch in the output. For small changes to
  // the parameters, you don't need to reset the PLL, and there is no glitch
  if((abs(pll_freq - iqmsa) > 16000000L) || divider != prev_divider){
	iqmsa = pll_freq;
	prev_divider = divider;
	SendRegister(SI_PLL_RESET, 0xA0);
  }
  //SendRegister(24, 0b00000000); // CLK3-0 Disable State: CLK2=0 (BE CAREFUL TO CHANGE THIS!!!), CLK1/0=00 -> IC4-X0 selected -> 2,5V on IC5A/3(+), when IC5/2(-) leaks down below 2.5V -> 12V on IC5A/1, IC6A/2(-) -> 0V on IC6A/1, AUDIO2
  }
  void alt_clk2(uint32_t freq)
  {
  uint32_t num, denom;
  uint16_t mult = div(pll_freq, freq, &num, &denom);

  SetupMultisynth(SI_SYNTH_MS_2, mult, num, denom, 1);

  // Switch on the CLK2 output to be PLL A and set multiSynth2 input
  SendRegister(SI_CLK2_CONTROL, 0x0F | SI_CLK_SRC_PLL_A);

  SendRegister(SI_CLK_OE, 0b11111000); // Enable CLK2|CLK1|CLK0

  //SendRegister(SI_CLK0_PHOFF, 0 * divider / 90); // one LSB equivalent to a time delay of Tvco/4 range 0..127
  //SendRegister(SI_CLK1_PHOFF, 90 * divider / 90); // one LSB equivalent to a time delay of Tvco/4 range 0..127
  //SendRegister(SI_CLK2_PHOFF, 45 * divider / 90); // one LSB equivalent to a time delay of Tvco/4 range 0..127
  //SendRegister(SI_PLL_RESET, 0xA0);
  }
  void powerDown()
  {
  SendRegister(SI_CLK0_CONTROL, 0b10000000);  // Conserve power when output is disabled
  SendRegister(SI_CLK1_CONTROL, 0b10000000);
  SendRegister(SI_CLK2_CONTROL, 0b10000000);
  SendRegister(19, 0b10000000);
  SendRegister(20, 0b10000000);
  SendRegister(21, 0b10000000);
  SendRegister(22, 0b10000000);
  SendRegister(23, 0b10000000);
  SendRegister(SI_CLK_OE, 0b11111111); // Disable all CLK outputs
  }
};
static SI5351 si5351;
 */

#ifdef LPF_SWITCHING_DL2MAN_USDX_REV1
class PCA9536 {
public:
#define PCA9536_ADDR  0x41  // PCA9536   https://www.ti.com/lit/ds/symlink/pca9536.pdf
	inline void SendRegister(uint8_t reg, uint8_t val) { i2c.begin(); i2c.beginTransmission(PCA9536_ADDR); i2c.write(reg); i2c.write(val); i2c.endTransmission(); }
	inline void init() { SendRegister(0x03, 0x00); } // configuration cmd: IO0-IO7 as output
	inline void write(uint8_t data) { init(); SendRegister(0x01, data); }  // output port cmd: write bits D7-D0 to IO7-IO0
};
PCA9536 ioext;

void set_latch(uint8_t io) { // reset all latches and set latch k to corresponding GPIO, all relays share a common (ground) GPIO
#define LATCH_TIME  15   // set/reset time latch relay
	for (int i = 0; i != 8; i++) { ioext.write((~(1 << i)) | 0x01); delay(LATCH_TIME); } ioext.write(0x00); // reset all latches
	ioext.write((1 << io) | 0x00); delay(LATCH_TIME); ioext.write(0x00); // set latch wired to io port
}

static uint8_t prev_lpf_io = 0xff;
inline void set_lpf(uint8_t f) {
	uint8_t lpf_io = (f > 8) ? 1 : (f > 4) ? 2 : /*(f <= 4)*/ 3; // cut-off freq in MHz to IO port of LPF relay
	if (prev_lpf_io != lpf_io) { prev_lpf_io = lpf_io; set_latch(lpf_io); };  // set relay
}
#endif  //LPF_SWITCHING_DL2MAN_USDX_REV1

#if defined(LPF_SWITCHING_DL2MAN_USDX_REV3) || defined(LPF_SWITCHING_DL2MAN_USDX_REV2) || defined(LPF_SWITCHING_DL2MAN_USDX_REV2_BETA)
class IOExpander16 {
public:
#ifdef LPF_SWITCHING_DL2MAN_USDX_REV2_BETA
#define IOEXP16_ADDR  0x74  // PCA9539 with A1..A0 set to 0     https://www.nxp.com/docs/en/data-sheet/PCA9539_PCA9539R.pdf
#endif
#ifdef LPF_SWITCHING_DL2MAN_USDX_REV2
#define IOEXP16_ADDR  0x24  // TCA/PCA9555 with A2=1 A1..A0=0   https://www.ti.com/lit/ds/symlink/tca9555.pdf
#endif
#ifdef LPF_SWITCHING_DL2MAN_USDX_REV3
#define IOEXP16_ADDR  0x20  // TCA/PCA9555 with A2=0 A1..A0=0   https://www.ti.com/lit/ds/symlink/tca9555.pdf
#endif
	inline void SendRegister(uint8_t reg, uint8_t val) { i2c.begin(); i2c.beginTransmission(IOEXP16_ADDR); i2c.write(reg); i2c.write(val); i2c.endTransmission(); }
	inline void init() { write(0U); } //IO0, IO1 as input, IO0 to 0, IO0 as output, IO1 to 0, IO1 as output
	inline void write(uint16_t data) { SendRegister(0x07, 0xff);  SendRegister(0x06, 0xff);/*Common last!*/ SendRegister(0x02, data); SendRegister(0x06, 0x00);/*Common first!*/ SendRegister(0x03, data >> 8); SendRegister(0x07, 0x00); }  // output port cmd: write bits D15-D0 to IO1.7-0.0;
};
IOExpander16 ioext;
enum gpioext_t { IO0_0, IO0_1, IO0_2, IO0_3, IO0_4, IO0_5, IO0_6, IO0_7, IO1_0, IO1_1, IO1_2, IO1_3, IO1_4, IO1_5, IO1_6, IO1_7 };

void set_latch(uint8_t io, uint8_t common_io, bool latch = true) { // reset all latches and set latch k to corresponding GPIO, all relays share a common (ground) GPIO
#define LATCH_TIME  30   // set/reset time latch relay
	if (latch) {
		ioext.write((1U << io) | 0x0000); delay(LATCH_TIME); ioext.write(0x0000); // set latch wired to io port
	}
	else {
		if (io == 0xff) { ioext.init(); for (int io = 0; io != 16; io++) set_latch(io, common_io, latch); } // reset all latches
		else { ioext.write((~(1U << io)) | (1U << common_io)); delay(LATCH_TIME); ioext.write(0x0000); } // reset latch wired to io port
	}
}

static uint8_t prev_lpf_io = 0xff; // inits and resets all latches
inline void set_lpf(uint8_t f) {
#ifdef LPF_SWITCHING_DL2MAN_USDX_REV3
	uint8_t lpf_io = (f > 26) ? IO1_3 : (f > 20) ? IO1_4 : (f > 17) ? IO1_2 : (f > 12) ? IO1_5 : (f > 8) ? IO1_1 : (f > 5) ? IO1_6 : (f > 4) ? IO1_0 : /*(f <= 4)*/ IO1_7; // cut-off freq in MHz to IO port of LPF relay
#ifndef LPF_SWITCHING_DL2MAN_USDX_REV3_NOLATCH
	if (prev_lpf_io != lpf_io) { set_latch(prev_lpf_io, IO0_0, false); set_latch(lpf_io, IO0_0); prev_lpf_io = lpf_io; };  // set relay (latched)
#else
	if (prev_lpf_io != lpf_io) { ioext.write(1U << lpf_io); prev_lpf_io = lpf_io; };  // set relay (non-latched)
#endif //LPF_SWITCHING_DL2MAN_USDX_REV3_NOLATCH
#else //LPF_SWITCHING_DL2MAN_USDX_REV2 LPF_SWITCHING_DL2MAN_USDX_REV2_BETA
	uint8_t lpf_io = (f > 12) ? IO0_3 : (f > 8) ? IO0_5 : (f > 5) ? IO0_7 : (f > 4) ? IO1_1 : /*(f <= 4)*/ IO1_3; // cut-off freq in MHz to IO port of LPF relay
	if (prev_lpf_io != lpf_io) { set_latch(prev_lpf_io, IO0_1, false); set_latch(lpf_io, IO0_1); prev_lpf_io = lpf_io; };  // set relay

#endif
}
#endif  //LPF_SWITCHING_DL2MAN_USDX_REV3 LPF_SWITCHING_DL2MAN_USDX_REV2 REV2_BETA

#ifdef LPF_SWITCHING_WB2CBA_USDX_OCTOBAND
class MCP23008 {
public:
#define MCP23008_ADDR  0x20  // MCP23008 with A1..A0 set to 0   https://ww1.microchip.com/downloads/en/DeviceDoc/21919e.pdf
	inline void SendRegister(uint8_t reg, uint8_t val) { i2c.begin(); i2c.beginTransmission(MCP23008_ADDR); i2c.write(reg); i2c.write(val); i2c.endTransmission(); }
	inline void init() { SendRegister(0x09, 0x00); SendRegister(0x00, 0x00); } //GP0-7 to 0, GP0-7 as output
	inline void write(uint16_t data) { SendRegister(0x09, data); }  // output port cmd: write bits D7-D0 to GP7-GP0
};
MCP23008 ioext;

static uint8_t prev_lpf_io = 0xff; // inits and resets all latches
inline void set_lpf(uint8_t f) {
	uint8_t lpf_io = (f > 26) ? 7 : (f > 20) ? 6 : (f > 17) ? 5 : (f > 12) ? 4 : (f > 8) ? 3 : (f > 6) ? 2 : (f > 4) ? 1 : /*(f <= 4)*/ 0; // cut-off freq in MHz to IO port of LPF relay
	if (prev_lpf_io == 0xff) { ioext.init(); }
	if (prev_lpf_io != lpf_io) { ioext.write(1U << lpf_io); prev_lpf_io = lpf_io; };  // set relay (non-latched)
}
#endif  //LPF_SWITCHING_WB2CBA_USDX_OCTOBAND

#if defined(LPF_SWITCHING_PE1DDA_USDXDUO)
inline void set_lpf(uint8_t f) {
	pinMode(PD5, OUTPUT);
	digitalWrite(PD5, (f >= LPF_SWITCHING_PE1DDA_USDXDUO));
}
#endif  //LPF_SWITCHING_PE1DDA_USDXDUO

#if !defined(LPF_SWITCHING_DL2MAN_USDX_REV1) && !defined(LPF_SWITCHING_DL2MAN_USDX_REV2_BETA) && !defined(LPF_SWITCHING_DL2MAN_USDX_REV2) && !defined(LPF_SWITCHING_DL2MAN_USDX_REV3) && !defined(LPF_SWITCHING_WB2CBA_USDX_OCTOBAND) && !defined(LPF_SWITCHING_PE1DDA_USDXDUO)
inline void set_lpf(uint8_t f) {} // dummy
#endif

#ifdef DEBUG
static uint32_t sr = 0;
static uint32_t cpu_load = 0;
volatile uint16_t param_a = 0;  // registers for debugging, testing and experimental purposes
volatile int16_t param_b = 0;
volatile int16_t param_c = 0;
#endif

enum dsp_cap_t { ANALOG, DSP, SDR };
#ifdef QCX
uint8_t dsp_cap = 0;
uint8_t ssb_cap = 0;
#else
// force SSB and SDR capability
const uint8_t ssb_cap = 1;
const uint8_t dsp_cap = SDR;
#endif

enum mode_t { LSB, USB, CW, FM, AM };
volatile uint8_t mode = USB;
volatile uint16_t numSamples = 0;

volatile uint8_t tx = 0;
volatile uint8_t filt = 0;

inline void _vox(bool trigger)
{
	if (trigger) {
		tx = (tx) ? 254 : 255; // hangtime = 255 / 4402 = 58ms (the time that TX at least stays on when not triggered again). tx == 255 when triggered first, 254 follows for subsequent triggers, until tx is off.
	}
	else {
		if (tx) tx--;
	}
}

#define F_SAMP_TX 4800 //4810 //4805 // 4402 // (Design) ADC sample-rate; is best a multiple of _UA and fits exactly in OCR2A = ((F_CPU / 64) / F_SAMP_TX) - 1 , should not exceed CPU utilization
#if(F_MCU != 20000000)
const int16_t _F_SAMP_TX = (F_MCU * 4800LL / 20000000);  // Actual ADC sample-rate; used for phase calculations
#else
#define _F_SAMP_TX  F_SAMP_TX
#endif
#define _UA  600 //=(_FSAMP_TX)/8 //(_F_SAMP_TX)      //360  // unit angle; integer representation of one full circle turn or 2pi radials or 360 degrees, should be a integer divider of F_SAMP_TX and maximized to have higest precision
#define MAX_DP  ((filt == 0) ? _UA : (filt == 3) ? _UA/4 : _UA/2)     //(_UA/2) // the occupied SSB bandwidth can be further reduced by restricting the maximum phase change (set MAX_DP to _UA/2).
#define CARRIER_COMPLETELY_OFF_ON_LOW  1    // disable oscillator on low amplitudes, to prevent potential unwanted biasing/leakage through PA circuit
#define MULTI_ADC  1  // multiple ADC conversions for more sensitive (+12dB) microphone input
#define QUAD  1       // invert TX signal for phase changes > 180

inline int16_t arctan3(int16_t q, int16_t i)  // error ~ 0.8 degree
{ // source: [1] http://www-labs.iro.umontreal.ca/~mignotte/IFT2425/Documents/EfficientApproximationArctgFunction.pdf
//#define _atan2(z)  (_UA/8 + _UA/44) * z  // very much of a simplification...not accurate at all, but fast
#define _atan2(z)  (_UA/8 + _UA/22 - _UA/22 * z) * z  //derived from (5) [1]   note that atan2 can overflow easily so keep _UA low
//#define _atan2(z)  (_UA/8 + _UA/24 - _UA/24 * z) * z  //derived from (7) [1]
	int16_t r;
	if (abs(q) > abs(i))
		r = _UA / 4 - _atan2(abs(i) / abs(q));        // arctan(z) = 90-arctan(1/z)
	else
		r = (i == 0) ? 0 : _atan2(abs(q) / abs(i));   // arctan(z)
	r = (i < 0) ? _UA / 2 - r : r;                  // arctan(-z) = -arctan(z)
	return (q < 0) ? -r : r;                        // arctan(-z) = -arctan(z)
}

#define magn(i, q) (abs(i) > abs(q) ? abs(i) + (abs(q) / 4) : abs(q) + (abs(i) / 4)) // approximation of: magnitude = sqrt(i*i + q*q); error 0.95dB

uint8_t lut[256];
volatile uint8_t amp;
#define MORE_MIC_GAIN   1       // 1.02w adds more microphone gain, improving overall SSB quality (when speaking further away from microphone)
#ifdef MORE_MIC_GAIN
volatile uint8_t vox_thresh = (1 << 2);
#else
volatile uint8_t vox_thresh = (1 << 1); //(1 << 2);
#endif
volatile uint8_t drive = 2;   // hmm.. drive>2 impacts cpu load..why?

static uint8_t cat_enabled = false;  // G8RDI mod - added
static uint8_t quad_enabled = false;  // G8RDI mod - added run time enabling

static uint8_t error_code = 0;      // G8RDI mod - added LCD error code

volatile uint8_t quad = 0;

inline int16_t ssb(int16_t in)
{
	static int16_t dc, z1;

	int16_t i, q;
	uint8_t j;
	static int16_t v[16];
	for (j = 0; j != 15; j++) v[j] = v[j + 1];
#ifdef MORE_MIC_GAIN
	//#define DIG_MODE  // optimization for digital modes: for super flat TX spectrum, (only down < 100Hz to cut-off DC components)
#ifdef DIG_MODE
	int16_t ac = in;
	dc = (ac + (7) * dc) / (7 + 1);  // hpf: slow average
	v[15] = (ac - dc) / 2;           // hpf (dc decoupling)  (-6dB gain to compensate for DC-noise)
#else
	int16_t ac = in * 2;             //   6dB gain (justified since lpf/hpf is losing -3dB)
	ac = ac + z1;                    // lpf
	z1 = (in - (2) * z1) / (2 + 1);  // lpf: notch at Fs/2 (alias rejecting)
	dc = (ac + (2) * dc) / (2 + 1);  // hpf: slow average
	v[15] = (ac - dc);               // hpf (dc decoupling)
#endif //DIG_MODE
	i = v[7] * 2;  // 6dB gain for i, q  (to prevent quanitization issues in hilbert transformer and phase calculation, corrected for magnitude calc)
	q = ((v[0] - v[14]) * 2 + (v[2] - v[12]) * 8 + (v[4] - v[10]) * 21 + (v[6] - v[8]) * 16) / 64 + (v[6] - v[8]); // Hilbert transform, 40dB side-band rejection in 400..1900Hz (@4kSPS) when used in image-rejection scenario; (Hilbert transform require 5 additional bits)

	uint16_t _amp = magn(i / 2, q / 2);  // -6dB gain (correction)
#else  // !MORE_MIC_GAIN
	//dc += (in - dc) / 2;       // fast moving average
	dc = (in + dc) / 2;        // average
	int16_t ac = (in - dc);   // DC decoupling
	//v[15] = ac;// - z1;        // high-pass (emphasis) filter
	v[15] = (ac + z1);// / 2;           // low-pass filter with notch at Fs/2
	z1 = ac;

	i = v[7];
	q = ((v[0] - v[14]) * 2 + (v[2] - v[12]) * 8 + (v[4] - v[10]) * 21 + (v[6] - v[8]) * 15) / 128 + (v[6] - v[8]) / 2; // Hilbert transform, 40dB side-band rejection in 400..1900Hz (@4kSPS) when used in image-rejection scenario; (Hilbert transform require 5 additional bits)

	uint16_t _amp = magn(i, q);
#endif  // MORE_MIC_GAIN

#ifdef CARRIER_COMPLETELY_OFF_ON_LOW
	_vox(_amp > vox_thresh);
#else
	if (vox) _vox(_amp > vox_thresh);
#endif
	//_amp = (_amp > vox_thresh) ? _amp : 0;   // vox_thresh = 4 is a good setting
	//if(!(_amp > vox_thresh)) return 0;

	_amp = _amp << (drive);
	_amp = ((_amp > 255) || (drive == 8)) ? 255 : _amp; // clip or when drive=8 use max output
	amp = (tx) ? lut[_amp] : 0;

	static int16_t prev_phase;
	int16_t phase = arctan3(q, i);

	int16_t dp = phase - prev_phase;  // phase difference and restriction
	//dp = (amp) ? dp : 0;  // dp = 0 when amp = 0
	prev_phase = phase;

	if (dp < 0)
		dp = dp + _UA; // make negative phase shifts positive: prevents negative frequencies and will reduce spurs on other sideband
#ifdef QUAD       // G8RDI: This worsens TX SSB voice quality, more Dalex sounding.
	if (dp >= (_UA / 2))
	{
		if (quad_enabled)  // G8RDI mod - added
		{
			dp = dp - _UA / 2;
			quad = !quad;
		}
	}
#endif

#ifdef MAX_DP
	if (dp > MAX_DP) { // dp should be less than half unit-angle in order to keep frequencies below F_SAMP_TX/2
		prev_phase = phase - (dp - MAX_DP);  // substract restdp
		dp = MAX_DP;
	}
#endif
	if (mode == USB)
		return dp * (_F_SAMP_TX / _UA); // calculate frequency-difference based on phase-difference
	else
		return dp * (-_F_SAMP_TX / _UA);
}

#define MIC_ATTEN  0  // 0*6dB attenuation (note that the LSB bits are quite noisy)
volatile int8_t mox = 0;
volatile int8_t volume = 12;

// This is the ADC ISR, issued with sample-rate via timer1 compb interrupt.
// It performs in real-time the ADC sampling, calculation of SSB phase-differences, calculation of SI5351 frequency registers and send the registers to SI5351 over I2C.
static int16_t _adc;
void dsp_tx()
{ // jitter dependent things first
#ifdef MULTI_ADC  // SSB with multiple ADC conversions:
	int16_t adc;                         // current ADC sample 10-bits analog input, NOTE: first ADCL, then ADCH
	adc = ADC;
	ADCSRA |= (1 << ADSC);
	//OCR1BL = amp;                        // submit amplitude to PWM register (actually this is done in advance (about 140us) of phase-change, so that phase-delays in key-shaping circuit filter can settle)
	si5351.SendPLLRegisterBulk();       // submit frequency registers to SI5351 over 731kbit/s I2C (transfer takes 64/731 = 88us, then PLL-loopfilter probably needs 50us to stabalize)
#ifdef QUAD
	if (quad_enabled)  // G8RDI mod - added
	{
#ifdef TX_CLK0_CLK1
		si5351.SendRegister(16, (quad) ? 0x1f : 0x0f);  // Invert/non-invert CLK0 in case of a huge phase-change
		si5351.SendRegister(17, (quad) ? 0x1f : 0x0f);  // Invert/non-invert CLK1 in case of a huge phase-change
#else
		si5351.SendRegister(18, (quad) ? 0x1f : 0x0f);  // Invert/non-invert CLK2 in case of a huge phase-change
#endif
	}
#endif //QUAD
	OCR1BL = amp;                      // submit amplitude to PWM register (takes about 1/32125 = 31us+/-31us to propagate) -> amplitude-phase-alignment error is about 30-50us
	adc += ADC;
	ADCSRA |= (1 << ADSC);  // causes RFI on QCX-SSB units (not on units with direct biasing); ENABLE this line when using direct biasing!!
	int16_t df = ssb(_adc >> MIC_ATTEN); // convert analog input into phase-shifts (carrier out by periodic frequency shifts)
	adc += ADC;
	ADCSRA |= (1 << ADSC);
	si5351.freq_calc_fast(df);           // calculate SI5351 registers based on frequency shift and carrier frequency
	adc += ADC;
	ADCSRA |= (1 << ADSC);
	//_adc = (adc/4 - 512);
#define AF_BIAS   32
	_adc = (adc / 4 - (512 - AF_BIAS));        // now make sure that we keep a postive bias offset (to prevent the phase swapping 180 degrees and potentially causing negative feedback (RFI)
#else  // SSB with single ADC conversion:
	ADCSRA |= (1 << ADSC);    // start next ADC conversion (trigger ADC interrupt if ADIE flag is set)
	//OCR1BL = amp;                        // submit amplitude to PWM register (actually this is done in advance (about 140us) of phase-change, so that phase-delays in key-shaping circuit filter can settle)
	si5351.SendPLLRegisterBulk();       // submit frequency registers to SI5351 over 731kbit/s I2C (transfer takes 64/731 = 88us, then PLL-loopfilter probably needs 50us to stabalize)
	OCR1BL = amp;                        // submit amplitude to PWM register (takes about 1/32125 = 31us+/-31us to propagate) -> amplitude-phase-alignment error is about 30-50us
	int16_t adc = ADC - 512; // current ADC sample 10-bits analog input, NOTE: first ADCL, then ADCH
	int16_t df = ssb(adc >> MIC_ATTEN);  // convert analog input into phase-shifts (carrier out by periodic frequency shifts)
	si5351.freq_calc_fast(df);           // calculate SI5351 registers based on frequency shift and carrier frequency
#endif

#ifdef CARRIER_COMPLETELY_OFF_ON_LOW
	if (tx == 1) { OCR1BL = 0; si5351.SendRegister(SI_CLK_OE, TX0RX0); }   // disable carrier
	if (tx == 255) { si5351.SendRegister(SI_CLK_OE, TX1RX0); } // enable carrier
#endif

#ifdef MOX_ENABLE
	if (!mox) return;
	OCR1AL = (adc << (mox - 1)) + 128;  // TX audio monitoring
#endif
}

volatile uint16_t acc;
volatile uint32_t cw_offset;
volatile uint8_t tone_vol = 12;
volatile uint8_t cw_tone = 1;
const uint32_t tones[] = { F_MCU * 700ULL / 20000000, F_MCU * 600ULL / 20000000, F_MCU * 700ULL / 20000000 };  // G8RDI todo ULL to divisor?

volatile int8_t p_sin = 0;     // initialized with A*sin(0) = 0
volatile int8_t n_cos = 448 / 4; // initialized with A*cos(t) = A
inline void process_minsky() // Minsky circle sample [source: https://www.cl.cam.ac.uk/~am21/hakmemc.html, ITEM 149]: p_sin+=n_cos*2*PI*f/fs; n_cos-=p_sin*2*PI*f/fs;
{
	int8_t alpha127 = tones[cw_tone]/*cw_offset*/ * 798 / _F_SAMP_TX;  // alpha = f_tone * 2 * pi / fs
	p_sin += alpha127 * n_cos / 127;
	n_cos -= alpha127 * p_sin / 127;
}

// CW Key-click shaping, ramping up/down amplitude with sample-interval of 60us. Tnx: Yves HB9EWY https://groups.io/g/ucx/message/5107
const uint8_t ramp[] PROGMEM = { 255, 254, 252, 249, 245, 239, 233, 226, 217, 208, 198, 187, 176, 164, 152, 139, 127, 115, 102, 90, 78, 67, 56, 46, 37, 28, 21, 15, 9, 5, 2 }; // raised-cosine(i) = 255 * sq(cos(HALF_PI * i/32))

void dummy()
{
}

void dsp_tx_cw()
{ // jitter dependent things first
#ifdef KEY_CLICK
	if (OCR1BL < lut[255]) { //check if already ramped up: ramp up of amplitude 
		for (uint16_t i = 31; i != 0; i--) {   // soft rising slope against key-clicks
			OCR1BL = lut[pgm_read_byte_near(ramp[i])];
			delayMicroseconds(60);
		}
	}
#endif // KEY_CLICK
	OCR1BL = lut[255];

	process_minsky();

#ifdef CW_VOLUME
	OCR1AL = (tone_vol ? (p_sin >> (16 - tone_vol)) : 0) + 128;  // xyzzy G8RDI mod - added for CW tone volume
#else
	OCR1AL = (p_sin >> (16 - volume)) + 128;
#endif
}

void dsp_tx_am()
{ // jitter dependent things first
	ADCSRA |= (1 << ADSC);    // start next ADC conversion (trigger ADC interrupt if ADIE flag is set)
	OCR1BL = amp;                        // submit amplitude to PWM register (actually this is done in advance (about 140us) of phase-change, so that phase-delays in key-shaping circuit filter can settle)
	int16_t adc = ADC - 512; // current ADC sample 10-bits analog input, NOTE: first ADCL, then ADCH
	int16_t in = (adc >> MIC_ATTEN);
	in = in << (drive - 4);
	//static int16_t dc;
	//dc += (in - dc) / 2;
	//in = in - dc;     // DC decoupling
#define AM_BASE 32
	in = max(0, min(255, (in + AM_BASE)));
	amp = in;// lut[in];
}

void dsp_tx_fm()
{ // jitter dependent things first
	ADCSRA |= (1 << ADSC);    // start next ADC conversion (trigger ADC interrupt if ADIE flag is set)
	OCR1BL = lut[255];                   // submit amplitude to PWM register (actually this is done in advance (about 140us) of phase-change, so that phase-delays in key-shaping circuit filter can settle)
	si5351.SendPLLRegisterBulk();       // submit frequency registers to SI5351 over 731kbit/s I2C (transfer takes 64/731 = 88us, then PLL-loopfilter probably needs 50us to stabalize)
	int16_t adc = ADC - 512; // current ADC sample 10-bits analog input, NOTE: first ADCL, then ADCH
	int16_t in = (adc >> MIC_ATTEN);
	in = in << (drive);
	int16_t df = in;
	si5351.freq_calc_fast(df);           // calculate SI5351 registers based on frequency shift and carrier frequency
}

#define EA(y, x, one_over_alpha)  (y) = (y) + ((x) - (y)) / (one_over_alpha); // exponental averaging [Lyons 13.33.1]
#define MLEA(y, x, L, M)  (y)  = (y) + ((((x) - (y)) >> (L)) - (((x) - (y)) >> (M))); // multiplierless exponental averaging [Lyons 13.33.1], with alpha=1/2^L - 1/2^M

#ifdef SWR_METER
volatile uint8_t swrmeter = 1;
#endif

const char m2c[] PROGMEM = "~ ETIANMSURWDKGOHVF*L*PJBXCYZQ**54S3***2**+***J16=/***H*7*G*8*90************?_****\"**.****@***'**-********;!*)*****,****:****";

#ifdef CW_MESSAGE
#define MENU_STR  1

uint8_t delayWithKeySense(uint32_t ms) {
	uint32_t event = millis() + ms;
	for (; millis() < event;) {
		wdt_reset();
		if (inv ^ digitalRead(BUTTONS) || !digitalRead(DAH) || !digitalRead(DIT)) {
			for (; inv ^ digitalRead(BUTTONS);) wdt_reset();  // wait until buttons released  
			return 1;  // stop when button/key pressed
		}
	}
	return 0;
}

// CW Messages can be used edited so allow size.
#ifdef CW_MESSAGE_EXT
char cw_msg[6][CW_MESSAGE_LENGTH] = { CW_MSG1, CW_MSG2, CW_MSG3, CW_MSG4, CW_MSG5, CW_MSG6 };
#else
char cw_msg[1][16] = { CW_STD_MSG };
#endif
uint8_t cw_msg_interval = 5; // number of seconds CW message is repeated
uint32_t cw_msg_event = 0;
uint8_t cw_msg_id = 0; // selected message

int cw_tx(char ch) {    // *** CW Transmit msg ***
	char sym;
	for (uint8_t j = 0; (sym = pgm_read_byte_near(m2c + j)); j++) {  // lookup msg[i] in m2c, skip if not found
		if (sym == ch) {  // found -> transmit CW character j
			wdt_reset();
			uint8_t k = 0x80; for (; !(j & k); k >>= 1); k >>= 1; // shift start of cw code to MSB
			if (k == 0) delay(ditTime * 4); // space -> add word space
			else {
				for (; k; k >>= 1) { // send dit/dah one by one, until everythng is sent
					switch_rxtx(1);  // key-on  tx
					if (delayWithKeySense(ditTime * ((j & k) ? 3 : 1))) { switch_rxtx(0); return 1; } // symbol: dah or dih length
					switch_rxtx(0);  // key-off tx
					if (delayWithKeySense(ditTime)) return 1;   // add symbol space
				}
				if (delayWithKeySense(ditTime * 2)) return 1; // add letter space
			}
			break; // next character
		}
	}
	return 0;
}

int cw_tx(char* msg) {
	for (uint8_t i = 0; msg[i]; i++) {  // loop over message
		lcd.setCursor(0, 0); lcd.print(i); lcd.print("    ");
		if (cw_tx(msg[i]))  // Sent morse char
			return 1;
	}
	return 0;
}
#endif // CW_MESSAGE

volatile uint8_t menumode = 0;  // 0=not in menu, 1=selects menu item, 2=selects parameter value

#ifdef CW_DECODER
volatile uint8_t cwdec = 1;
static int32_t avg = 256;
static uint8_t sym;
static uint32_t amp32 = 0;
volatile uint32_t _amp32 = 0;
static char out[] = "                ";
volatile uint8_t cw_event = false;

void printsym(bool submit = true) {
	if (sym < 128) {
		char ch = pgm_read_byte_near(m2c + sym); if (ch != '*') {
#ifdef CW_INTERMEDIATE
			out[15] = ch; cw_event = true; if (submit) { for (int i = 0; i != 15; i++) { out[i] = out[i + 1]; } out[15] = ' '; }   // update LCD, only shift when submit is true, otherwise update last char only
#else
			for (int i = 0; i != 15; i++) out[i] = out[i + 1]; out[15] = ch; cw_event = true;   // update LCD
#endif
		}
	}
	if (submit) sym = 1;
}

bool realstate = LOW;
bool realstatebefore = LOW;
bool filteredstate = LOW;
bool filteredstatebefore = LOW;
uint8_t nbtime = 16;  // 6 // ms noise blanker
uint32_t starttimehigh;
uint32_t highduration;
uint32_t hightimesavg;
uint32_t lowtimesavg;
uint32_t startttimelow;
uint32_t lowduration;
uint32_t laststarttime = 0;
uint8_t wpm = 25;

inline void cw_decode()
{
	int32_t in = _amp32;
	EA(avg, in, (1 << 8));
	realstate = (in > (avg * 1 / 2));  // threshold

	// here we clean up the state with a noise blanker
	if (realstate != realstatebefore) {
		laststarttime = millis();
	}
	//#define NB_SCALED_TO_WPM    1   // Scales noise-blanker timing the actual CW speed; this should reduce errors from noise at low speeds; this may have side-effect with fast speed changes that fast CW will be filtered out
#ifdef NB_SCALED_TO_WPM
	if ((millis() - laststarttime) > min(1200 / (20 * 2), max(1200 / (40 * 2), hightimesavg / 6))) {
#else
	if ((millis() - laststarttime) > nbtime) {
#endif
		if (realstate != filteredstate) {
			filteredstate = realstate;
			//dec2();
		}
	}
	else avg += avg / 100; // keep threshold above noise spikes (increase threshold with 1%)

	dec2();
	realstatebefore = realstate;
	}

//#define NEW_CW  1   // CW decoder portions from by Hjalmar Skovholm Hansen OZ1JHM, source: http://www.skovholm.com/decoder11.ino
#ifdef NEW_CW
void dec2()
{
	// Then we do want to have some durations on high and low
	if (filteredstate != filteredstatebefore) {
		if (menumode == 0) { lcd.noCursor(); lcd.setCursor(15, 1); lcd.print((filteredstate) ? 'R' : ' '); stepsize_showcursor(); }

		if (filteredstate == HIGH) {
			starttimehigh = millis();
			lowduration = (millis() - startttimelow);
		}

		if (filteredstate == LOW) {
			startttimelow = millis();
			highduration = (millis() - starttimehigh);
			if (highduration < (2 * hightimesavg) || hightimesavg == 0) {
				hightimesavg = (highduration + hightimesavg + hightimesavg) / 3;     // now we know avg dit time ( rolling 3 avg)
			}
			if (highduration > (5 * hightimesavg)) {
				hightimesavg = highduration / 3;     // if speed decrease fast ..      
				//hightimesavg = highduration+hightimesavg;     // if speed decrease fast ..
			}
		}
	}

	// now we will check which kind of baud we have - dit or dah, and what kind of pause we do have 1 - 3 or 7 pause, we think that hightimeavg = 1 bit
	if (filteredstate != filteredstatebefore) {
		if (filteredstate == LOW) {  //// we did end a HIGH
#define FAIR_WEIGHTING    1
#ifdef FAIR_WEIGHTING
			if (highduration < (hightimesavg + hightimesavg / 2) && highduration >(hightimesavg * 6 / 10)) { /// 0.6 filter out false dits
#else
			if (highduration < (hightimesavg * 2) && highduration >(hightimesavg * 6 / 10)) { /// 0.6 filter out false dits
#endif
				sym = (sym << 1) | (0);        // insert dit (0)
			}
#ifdef FAIR_WEIGHTING
			if (highduration > (hightimesavg + hightimesavg / 2) && highduration < (hightimesavg * 6)) {
#else
			if (highduration > (hightimesavg * 2) && highduration < (hightimesavg * 6)) {
#endif
				sym = (sym << 1) | (1);        // insert dah (1)
				wpm = (wpm + (1200 / ((highduration) / 3) * 4 / 3)) / 2;
			}
			}

		if (filteredstate == HIGH) {  // we did end a LOW 
			uint16_t lacktime = 10;
			if (wpm > 25)lacktime = 10; // when high speeds we have to have a little more pause before new letter or new word 
			if (wpm > 30)lacktime = 12;
			if (wpm > 35)lacktime = 15;

#ifdef FAIR_WEIGHTING
			if (lowduration > (hightimesavg * (lacktime * 1 / 10)) && lowduration < hightimesavg * (lacktime * 5 / 10)) { // letter space
#else
			if (lowduration > (hightimesavg * (lacktime * 7 / 80)) && lowduration < hightimesavg * (lacktime * 5 / 10)) { // letter space
				//if(lowduration > (hightimesavg*(lacktime*2/10)) && lowduration < hightimesavg*(lacktime*5/10)){ // letter space
#endif
				printsym();
			}
			if (lowduration >= hightimesavg * (lacktime * 5 / 10)) { // word space
				printsym();
				printsym();  // print space
			}
			}
		}

	// write if no more letters
	if ((millis() - startttimelow) > (highduration * 6) && (sym > 1)) {
		printsym();
	}

	filteredstatebefore = filteredstate;
			}

#else // OLD_CW

void dec2()
{
	if (filteredstate != filteredstatebefore) { // then we do want to have some durations on high and low
		if (menumode == 0) { lcd.noCursor(); lcd.setCursor(15, 1); lcd.print((filteredstate) ? 'R' : ' '); stepsize_showcursor(); }

		if (filteredstate == HIGH) {
			starttimehigh = millis();
			lowduration = (millis() - startttimelow);
			//highduration = 0;

			if ((sym > 1) && lowduration > (hightimesavg * 2)/* && lowduration < hightimesavg*(5*lacktime)*/) { // letter space
				printsym();
				wpm = (1200 / hightimesavg * 4 / 3);
				//if(lowduration >= hightimesavg*(5)){ sym=1; printsym(); } // (print additional space) word space
			}
			if (lowduration >= hightimesavg * (5)) { sym = 1; printsym(); } // (print additional space) word space
		}

		if (filteredstate == LOW) {
			startttimelow = millis();
			highduration = (millis() - starttimehigh);
			//lowduration = 0;
			if (highduration < (2 * hightimesavg) || hightimesavg == 0) {
				hightimesavg = (highduration + hightimesavg + hightimesavg) / 3;     // now we know avg dit time (rolling 3 avg)
			}
			if (highduration > (5 * hightimesavg)) {
				hightimesavg = highduration / 3;     // if speed decrease fast ..      
				//hightimesavg = highduration+hightimesavg;     // if speed decrease fast ..
			}
			if (highduration > (hightimesavg / 2)) {
				sym = (sym << 1) | (highduration > (hightimesavg * 2));       // dit (0) or dash (1)
#if defined(CW_INTERMEDIATE) && !defined(OLED) && !defined(LCD_I2C) && (F_MCU >= 20000000)
				printsym(false);
#endif
			}
		}
	}

	if (((millis() - startttimelow) > hightimesavg * (6)) && (sym > 1)) {
		//if(((millis() - startttimelow) > hightimesavg*(12)) && (sym > 1)){
		  //if(sym == 2) sym = 1; else // skip E E E E E
		printsym();  // write if no more letters
		//sym=0; printsym(); // print special char
		//startttimelow = millis();
	}

	filteredstatebefore = filteredstate;
}
#endif //OLD_CW
#endif  //CW_DECODER

#define F_SAMP_PWM (78125/1)
//#define F_SAMP_RX 78125  // overrun, do not use
#define F_SAMP_RX 62500
//#define F_SAMP_RX 52083
//#define F_SAMP_RX 44643
//#define F_SAMP_RX 39062
//#define F_SAMP_RX 34722
//#define F_SAMP_RX 31250
//#define F_SAMP_RX 28409
#define F_ADC_CONV (192307/2)  //was 192307/1, but as noted this produces clicks in audio stream. Slower ADC clock cures this (but is a problem for VOX when sampling mic-input simulatanously).

#ifdef FAST_AGC
volatile uint8_t agc = 2;
#else
volatile uint8_t agc = 1;
#endif
volatile uint8_t nr = 2;    // G8RDI mod
volatile uint8_t att = 0;
volatile uint8_t att2 = 2;  // Minimum att2 increased, to prevent numeric overflow on strong signals
volatile uint8_t _init = 0;

// Old AGC algorithm which only increases gain, but does not decrease it for very strong signals.
// Maximum possible gain is x32 (in practice, x31) so AGC range is x1 to x31 = 30dB approx.
// Decay time is fine (about 1s) but attack time is much slower than I like. 
// For weak/medium signals it aims to keep the sample value between 1024 and 2048. 
static int16_t gain = 1024;
inline int16_t process_agc_fast(int16_t in)
{
	int16_t out = (gain >= 1024) ? (gain >> 10) * in : in;
	int16_t accum = (1 - abs(out >> 10));
	if ((INT16_MAX - gain) > accum) gain = gain + accum;
	if (gain < 1) gain = 1;
	return out;
}

// Contribution by Alan, M0PUB: Experimental new AGC algorithm.
// ASSUMES: Input sample values are constrained to a maximum of +/-4096 to avoid integer overflow in earlier
// calculations.
//
// This algorithm aims to keep signals between a peak sample value of 1024 - 1536, with fast attack but slow
// decay.
//
// The variable centiGain actually represents the applied gain x 128 - i.e. the numeric gain applied is centiGain/128
//
// Since the largest valid input sample has a value of +/- 4096, centiGain should never be less than 32 (i.e.
// a 'gain' of 0.25). The maximum value for centiGain is 32767, and hence a gain of 255. So the AGC range
// is 0.25:255, or approx. 60dB.
//
// Variable 'slowdown' allows the decay time to be slowed down so that it is not directly related to the value
// of centiCount.

static int16_t centiGain = 128;
#define DECAY_FACTOR 400      // AGC decay occurs <DECAY_FACTOR> slower than attack.
static uint16_t decayCount = DECAY_FACTOR;
#define HI(x)  ((x) >> 8)
#define LO(x)  ((x) & 0xFF)

inline int16_t process_agc(int16_t in)
{
	static bool small = true;
	int16_t out;

	if (centiGain >= 128)
		out = (centiGain >> 5) * in;         // net gain >= 1
	else
		out = (centiGain >> 2) * (in >> 3);  // net gain < 1
	out >>= 2;

	if (HI(abs(out)) > HI(1536)) {
		centiGain -= (centiGain >> 4);       // Fast attack time when big signal encountered (relies on CentiGain >= 16)
	}
	else {
		if (HI(abs(out)) > HI(1024))
			small = false;
		if (--decayCount == 0) {               // But slow ramp up of gain when signal disappears
			if (small) {                         // 400 samples below lower threshold - increase gain
				if (centiGain < (INT16_MAX - (INT16_MAX >> 4)))
					centiGain += (centiGain >> 4);
				else
					centiGain = INT16_MAX;
			}
			decayCount = DECAY_FACTOR;
			small = true;
		}
	}
	return out;
}

inline int16_t process_nr_old(int16_t ac)
{
	ac = ac >> (6 - abs(ac));  // non-linear below amp of 6; to reduce noise (switchoff agc and tune-up volume until noise dissapears, todo:extra volume control needed)
	ac = ac << 3;
	return ac;
}

inline int16_t process_nr_old2(int16_t ac)
{
	static int16_t ea1;
	//ea1 = MLEA(ea1, ac, 5, 6); // alpha=0.0156
	ea1 = EA(ea1, ac, 64); // alpha=1/64=0.0156
	//static int16_t ea2;
	//ea2 = EA(ea2, ea1, 64); // alpha=1/64=0.0156

	return ea1;
}

/*
FIR Low Pass Filter

Fast Finite Impulse Response Low Pass Filter library for the Arduino 8-bit AVR processors

Written by Phillip Schmidt.  Patched into uSDX by Rob GW8RDI

Based on: http://www.labbookpages.co.uk/audio/firWindowing.html
*/
#ifdef NR_FIR
#define MAXfilterWindowSize 13  // window size must be odd, 7(nr=1), 9(2), 11(3), 13(4), 21(5)
static int16_t filterWindowSize;
static int16_t DataList[MAXfilterWindowSize];
static int16_t CoefList[MAXfilterWindowSize];

//byte filterWindowSize;  // number of samples window 
static byte OldestDataPoint = 0;  // oldest data point location 

void FirFilterSetup(byte filterSize, int cutOffFreq, int sampleFreq) // initialize filter
{
#ifdef DEBUG_G8RDI
	fir_value = filterSize;
	//fir_value = cutOffFreq;
#endif

	// Fixing best levels here to be used to Full b/w and Att2 = 2
	if (nr == 4)
	{
		filterSize = 7;
		cutOffFreq = 2700;
	}
	else
		if (nr == 5)
		{
			filterSize = 7;
			cutOffFreq = 2200;
		}
		else
			if (nr == 6)
			{
				filterSize = 7;
				cutOffFreq = 1800;
			}
			else
				if (nr == 7)
				{
					filterSize = 7;
					cutOffFreq = 1600;
				}
				else
					if (nr > 7)
					{
						filterSize = 7;
						cutOffFreq = 500;
					}

	if (!(B00000001 & filterSize))
		filterSize++; // window size must be odd
	if (filterSize > MAXfilterWindowSize)
		filterWindowSize = MAXfilterWindowSize;
	else
		filterWindowSize = filterSize;

	if (cutOffFreq > (sampleFreq / 2) - 1)
		cutOffFreq = (sampleFreq / 2) - 1;  // cut off must be less than half sampling freq

	//DataList = (int*) calloc (filterWindowSize, sizeof(int));   // create array for data
	//CoefList = (int*) calloc (filterWindowSize, sizeof(int));   // create array for coefficients
	float normalizedCutoff = float(cutOffFreq) / float(sampleFreq);

	int filterOrderHalf = (filterWindowSize - 1) / 2;
	for (int n = 0; n < filterWindowSize; n++) // Generate weight values
	{
		float stepsFromCenter;
		float weight;

		stepsFromCenter = float(n - filterOrderHalf);

		if (n != filterOrderHalf)  // either side of center
		{
			weight = sin(6.2831853f * normalizedCutoff * stepsFromCenter) / (3.14159265f * stepsFromCenter);
		}
		else            // center point
		{
			weight = 2.0f * normalizedCutoff;
		}

		weight *= 0.54f - 0.46f * cos(6.2832f * float(n) / float(filterWindowSize - 1)); // apply Hamming window to reduce ringing 
		CoefList[n] = int(weight * 32767.0f); // offset by 15bits, convert to INT
	}
}

#ifdef AM_MOD_MAGN_SQRT
// **** Special Integer Multiplication ****
// signed16 * signed16 to signed32
// 22 cycles
// http://mekonik.wordpress.com/2009/03/18/arduino-avr-gcc-multiplication/
#define MultiS16X16to32(longRes, intIn1, intIn2) \
asm volatile ( \
"clr r26 \n\t" \
"mul %A1, %A2 \n\t" \
"movw %A0, r0 \n\t" \
"muls %B1, %B2 \n\t" \
"movw %C0, r0 \n\t" \
"mulsu %B2, %A1 \n\t" \
"sbc %D0, r26 \n\t" \
"add %B0, r0 \n\t" \
"adc %C0, r1 \n\t" \
"adc %D0, r26 \n\t" \
"mulsu %B1, %A2 \n\t" \
"sbc %D0, r26 \n\t" \
"add %B0, r0 \n\t" \
"adc %C0, r1 \n\t" \
"adc %D0, r26 \n\t" \
"clr r1 \n\t" \
: \
"=&r" (longRes) \
: \
"a" (intIn1), \
"a" (intIn2) \
: \
"r26" \
)

// multiplication wrapper function
int32_t inline amul(int16_t a, int16_t b) {
	int32_t r;
	MultiS16X16to32(r, a, b);
	return r;
}
#endif

union TypeConverter
{
	long L;
	int16_t  I[2];
} result;

inline int16_t process_nr(int16_t in)   // G8RDI mod - added DSP FIR filte after NR = 3
{
	if (nr < 3)     // NR 1-2 use 1.02x 2-sample method
	{
		static int16_t ea1;
		ea1 = EA(ea1, in, 1 << (nr - 1));
		//static int16_t ea2;
		//ea2 = EA(ea2, ea1, inv_gain);
		return ea1;
	}

	/*  https://github.com/daPhoosa/FirFilter
	Object Creation:

	FirFilter myFilter( filter_Size, cut_Off_Frequency, sample_Frequency );
	filter_Size: Number of taps in the filter, should be an odd number. Larger sizes improve filter smoothness but increase computational time.

	Processing Time on a 16Mhz 8-bit Arduino 7 window = 48us
	9 window = 61us
	11 Window = 73us
	13 Window = 86us
	21 Window = 137us

	cut_Off_Frequency: Frequency at 3db point of transistion.

	sample_Frequency: Rate at which samples are submitted to the filter. This should be consistant.
	*/

	DataList[OldestDataPoint] = in; // replace oldest data in list

	// multiply sample by coefficient
	result.L = amul(DataList[OldestDataPoint], CoefList[0]); // accumulate data in a Long to eliminate rounding errors

	for (int i = 1; i < filterWindowSize; i++) // iterate through the rest of the data (stopping one short of start point)
	{
		OldestDataPoint++;  // increment and wrap pointer
		if (OldestDataPoint >= filterWindowSize)
			OldestDataPoint = 0;

		// multiply sample by coefficient 
		result.L += amul(DataList[OldestDataPoint], CoefList[i]); // ASM 32bit = 16bit x 16bit
	}

	result.L = result.L << 1; // bitshift to make up for non 16bit coefficient offset

	return result.I[1]; // the H16 part of the Long is the result
}
#else
inline int16_t process_nr(int16_t in)
{
	/*
	  static int16_t avg;
	  avg = EA(avg, abs(in), 64); // alpha=1/64=0.0156
	param_c = avg;
	*/

	/*
	  int32_t _avg = 64 * avg;
	//  if(_avg > 4) _avg = 4;  // clip
	//  uint16_t brs_avgsq = 1 << (_avg * _avg);
	  if(_avg > 14) _avg = 14;  // clip
	  uint16_t brs_avgsq = 1 << (_avg);


	  int16_t inv_gain;
	  if(brs_avgsq > 1) inv_gain = brs_avgsq / (brs_avgsq - 1);  // = 1 / (1 - 1/(1 << (1*avg*avg)) );
	  else inv_gain = 32768;*/

	static int16_t ea1;
	ea1 = EA(ea1, in, 1 << (nr - 1));
	//static int16_t ea2;
	//ea2 = EA(ea2, ea1, inv_gain);

	return ea1;
}
#endif

/*
inline int16_t process_nr(int16_t in)
{
  // Exponential moving average and variance (Lyons 13.36.2)
  param_b = EA(param_b, in, 1 << 4);  // avg
  param_c = EA(param_c, (in - param_b) * (in - param_b), 1 << 4);  // variance
}
*/

#define N_FILT 7
//volatile uint8_t filt = 0;
uint8_t prev_filt[] = { 0 , 4 }; // default filter for modes resp. CW, SSB

/* basicdsp filter simulation:
  samplerate=7812
  za0=in
  p1=slider1*10
  p2=slider2*10
  p3=slider3*10
  p4=slider4*10
  zb0=(za0+2*za1+za2)/2-(p1*zb1+p2*zb2)/16
  zc0=(zb0+2*zb1+zb2)/4-(p3*zc1+p4*zc2)/16
  zc2=zc1
  zc1=zc0
  zb2=zb1
  zb1=zb0
  za2=za1
  za1=za0
  out=zc0

  samplerate=7812
  za0=in
  p1=slider1*100+100
  p2=slider2*100
  p3=slider3*100+100
  p4=slider4*100
  zb0=(za0+2*za1+za2)-(-p1*zb1+p2*zb2)/64
  zc0=(zb0-2*zb1+zb2)/8-(-p3*zc1+p4*zc2)/64
  zc2=zc1
  zc1=zc0
  zb2=zb1
  zb1=zb0
  za2=za1
  za1=za0
  out=zc0/8
*/
inline int16_t filt_var(int16_t za0)  //filters build with www.micromodeler.com
{
	static int16_t za1, za2;
	static int16_t zb0, zb1, zb2;
	static int16_t zc0, zc1, zc2;

	if (filt < 4)
	{  // for SSB filters
	  // 1st Order (SR=8kHz) IIR in Direct Form I, 8x8:16
	  // M0PUB: There was a bug here, since za1 == zz1 at this point in the code, and the old algorithm for the 300Hz high-pass was:
	  //    za0=(29*(za0-zz1)+50*za1)/64;
	  //    zz2=zz1;
	  //    zz1=za0;
	  // After correction, this filter still introduced almost 6dB attenuation, so I adjusted the coefficients
		static int16_t zz1, zz2;
		//za0=(29*(za0-zz1)+50*za1)/64;                                //300-Hz
		zz2 = zz1;
		zz1 = za0;
		//za0=(30*(za0-zz2)+0*zz1)/32;                                 //300-Hz with very steep roll-off down to 0 Hz
		za0 = (30 * (za0 - zz2) + 25 * zz1) / 32;                                  //300-Hz

		// 4th Order (SR=8kHz) IIR in Direct Form I, 8x8:16
		switch (filt) {
		case 1: zb0 = (za0 + 2 * za1 + za2) / 2 - (13 * zb1 + 11 * zb2) / 16; break;   // 0-2900Hz filter, first biquad section
		case 2: zb0 = (za0 + 2 * za1 + za2) / 2 - (2 * zb1 + 8 * zb2) / 16; break;     // 0-2400Hz filter, first biquad section
			//case 3: zb0=(za0+2*za1+za2)/2-(4*zb1+2*zb2)/16; break;     // 0-2400Hz filter, first biquad section
		case 3: zb0 = (za0 + 2 * za1 + za2) / 2 - (0 * zb1 + 4 * zb2) / 16; break;     //0-1800Hz  elliptic
			//case 3: zb0=(za0+7*za1+za2)/16-(-24*zb1+9*zb2)/16; break;  //0-1700Hz  elliptic with slope
		}

		switch (filt) {
		case 1: zc0 = (zb0 + 2 * zb1 + zb2) / 2 - (18 * zc1 + 11 * zc2) / 16; break;     // 0-2900Hz filter, second biquad section
		case 2: zc0 = (zb0 + 2 * zb1 + zb2) / 4 - (4 * zc1 + 8 * zc2) / 16; break;       // 0-2400Hz filter, second biquad section
			//case 3: zc0=(zb0+2*zb1+zb2)/4-(1*zc1+9*zc2)/16; break;       // 0-2400Hz filter, second biquad section
		case 3: zc0 = (zb0 + 2 * zb1 + zb2) / 4 - (0 * zc1 + 4 * zc2) / 16; break;       //0-1800Hz  elliptic
			//case 3: zc0=(zb0+zb1+zb2)/16-(-22*zc1+47*zc2)/64; break;   //0-1700Hz  elliptic with slope
		}
		/*switch(filt){
		   case 1: zb0=za0; break; //0-4000Hz (pass-through)
		   case 2: zb0=(10*(za0+2*za1+za2)+16*zb1-17*zb2)/32; break;    //0-2500Hz  elliptic -60dB@3kHz
		   case 3: zb0=(7*(za0+2*za1+za2)+48*zb1-18*zb2)/32; break;     //0-1700Hz  elliptic
		 }

		 switch(filt){
		   case 1: zc0=zb0; break; //0-4000Hz (pass-through)
		   case 2: zc0=(8*(zb0+zb2)+13*zb1-43*zc1-52*zc2)/64; break;   //0-2500Hz  elliptic -60dB@3kHz
		   case 3: zc0=(4*(zb0+zb1+zb2)+22*zc1-47*zc2)/64; break;   //0-1700Hz  elliptic
		 }*/

		zc2 = zc1;
		zc1 = zc0;

		zb2 = zb1;
		zb1 = zb0;

		za2 = za1;
		za1 = za0;

		return zc0;
	}
	else { // for CW filters
		//   (2nd Order (SR=4465Hz) IIR in Direct Form I, 8x8:16), adding 64x front-gain (to deal with later division)
#ifdef FILTER_700HZ
		if (cw_tone == 0) {
			switch (filt) {
				/// only line with / 32!
			case 4: zb0 = (za0 + 2 * za1 + za2) / 2 + (41L * zb1 - 23L * zb2) / 32; break;   //500-1000Hz       // FILTER_700HZ for 700 Hz CW tone
				/// to check case 4: zb0 = (za0 + 2 * za1 + za2) / 2 + (41L * zb1 - 23L * zb2) / 64; break;   //500-1000Hz        // G8RDI mod - todo, check the filter as gain drops when selected
			case 5: zb0 = 5 * (za0 - 2 * za1 + za2) + (105L * zb1 - 58L * zb2) / 64; break;   //650-840Hz
			case 6: zb0 = 3 * (za0 - 2 * za1 + za2) + (108L * zb1 - 61L * zb2) / 64; break;   //650-750Hz
			case 7: zb0 = (2 * za0 - 3 * za1 + 2 * za2) + (111L * zb1 - 62L * zb2) / 64; break; //630-680Hz       
				//case 4: zb0=(0*za0+1*za1+0*za2)+(28*zb1-14*zb2)/16; break; //600Hz+-250Hz
				//case 5: zb0=(0*za0+1*za1+0*za2)+(28*zb1-15*zb2)/16; break; //600Hz+-100Hz
				//case 6: zb0=(0*za0+1*za1+0*za2)+(27*zb1-15*zb2)/16; break; //600Hz+-50Hz
				//case 7: zb0=(0*za0+1*za1+0*za2)+(27*zb1-15*zb2)/16; break; //630Hz+-18Hz
			}
			// 2nd switch unnecessary, take lines and add to above. G8RDI
			switch (filt) {
			case 4: zc0 = (zb0 - 2 * zb1 + zb2) / 4 + (105L * zc1 - 52L * zc2) / 64; break;      //500-1000Hz
			case 5: zc0 = ((zb0 + 2 * zb1 + zb2) + 97L * zc1 - 57L * zc2) / 64; break;      //650-840Hz
			case 6: zc0 = ((zb0 + zb1 + zb2) + 104L * zc1 - 60L * zc2) / 64; break;       //650-750Hz
			case 7: zc0 = ((zb1)+109L * zc1 - 62L * zc2) / 64; break;               //630-680Hz
				//case 4: zc0=(zb0-2*zb1+zb2)/1+(24*zc1-13*zc2)/16; break; //600Hz+-250Hz
				//case 5: zc0=(zb0-2*zb1+zb2)/4+(26*zc1-14*zc2)/16; break; //600Hz+-100Hz
				//case 6: zc0=(zb0-2*zb1+zb2)/16+(28*zc1-15*zc2)/16; break; //600Hz+-50Hz
				//case 7: zc0=(zb0-2*zb1+zb2)/32+(27*zc1-15*zc2)/16; break; //630Hz+-18Hz
			}
		}
		if (cw_tone == 1)
#endif
		{
			switch (filt) {
				//case 4: zb0=(1*za0+2*za1+1*za2)+(90L*zb1-38L*zb2)/64; break; //600Hz+-250Hz
				//case 5: zb0=(1*za0+2*za1+1*za2)/2+(102L*zb1-52L*zb2)/64; break; //600Hz+-100Hz
				//case 6: zb0=(1*za0+2*za1+1*za2)/2+(107L*zb1-57L*zb2)/64; break; //600Hz+-50Hz
				//case 7: zb0=(0*za0+1*za1+0*za2)+(110L*zb1-61L*zb2)/64; break; //600Hz+-25Hz

			case 4: zb0 = (0 * za0 + 1 * za1 + 0 * za2) + (114L * zb1 - 57L * zb2) / 64; break; //600Hz+-250Hz
			case 5: zb0 = (0 * za0 + 1 * za1 + 0 * za2) + (113L * zb1 - 60L * zb2) / 64; break; //600Hz+-100Hz
			case 6: zb0 = (0 * za0 + 1 * za1 + 0 * za2) + (110L * zb1 - 62L * zb2) / 64; break; //600Hz+-50Hz
			case 7: zb0 = (0 * za0 + 1 * za1 + 0 * za2) + (110L * zb1 - 61L * zb2) / 64; break; //600Hz+-18Hz
				//case 8: zb0=(0*za0+1*za1+0*za2)+(110L*zb1-60L*zb2)/64; break; //591Hz+-12Hz

				/*case 4: zb0=(0*za0+1*za1+0*za2)+2*zb1-zb2+(-14L*zb1+7L*zb2)/64; break; //600Hz+-250Hz
				case 5: zb0=(0*za0+1*za1+0*za2)+2*zb1-zb2+(-15L*zb1+4L*zb2)/64; break; //600Hz+-100Hz
				case 6: zb0=(0*za0+1*za1+0*za2)+2*zb1-zb2+(-14L*zb1+2L*zb2)/64; break; //600Hz+-50Hz
				case 7: zb0=(0*za0+1*za1+0*za2)+2*zb1-zb2+(-14L*zb1+3L*zb2)/64; break; //600Hz+-18Hz*/
			}

			switch (filt) {
				//case 4: zc0=(zb0-2*zb1+zb2)/4+(95L*zc1-44L*zc2)/64; break; //600Hz+-250Hz
				//case 5: zc0=(zb0-2*zb1+zb2)/8+(104L*zc1-53L*zc2)/64; break; //600Hz+-100Hz
				//case 6: zc0=(zb0-2*zb1+zb2)/16+(106L*zc1-56L*zc2)/64; break; //600Hz+-50Hz
				//case 7: zc0=(zb0-2*zb1+zb2)/32+(112L*zc1-62L*zc2)/64; break; //600Hz+-25Hz

			case 4: zc0 = (zb0 - 2 * zb1 + zb2) / 1 + (95L * zc1 - 52L * zc2) / 64; break; //600Hz+-250Hz
			case 5: zc0 = (zb0 - 2 * zb1 + zb2) / 4 + (106L * zc1 - 59L * zc2) / 64; break; //600Hz+-100Hz
			case 6: zc0 = (zb0 - 2 * zb1 + zb2) / 16 + (113L * zc1 - 62L * zc2) / 64; break; //600Hz+-50Hz
			case 7: zc0 = (zb0 - 2 * zb1 + zb2) / 32 + (112L * zc1 - 62L * zc2) / 64; break; //600Hz+-18Hz
				//case 8: zc0=(zb0-2*zb1+zb2)/64+(113L*zc1-63L*zc2)/64; break; //591Hz+-12Hz

				/*case 4: zc0=(zb0-2*zb1+zb2)/1+zc1-zc2+(31L*zc1+12L*zc2)/64; break; //600Hz+-250Hz
				case 5: zc0=(zb0-2*zb1+zb2)/4+2*zc1-zc2+(-22L*zc1+5L*zc2)/64; break; //600Hz+-100Hz
				case 6: zc0=(zb0-2*zb1+zb2)/16+2*zc1-zc2+(-15L*zc1+2L*zc2)/64; break; //600Hz+-50Hz
				case 7: zc0=(zb0-2*zb1+zb2)/16+2*zc1-zc2+(-16L*zc1+2L*zc2)/64; break; //600Hz+-18Hz*/
			}
		}
		zc2 = zc1;
		zc1 = zc0;

		zb2 = zb1;
		zb1 = zb0;

		za2 = za1;
		za1 = za0;

		//return zc0 / 64; // compensate the 64x front-end gain
		return zc0 / 8; // compensate the front-end gain
	}
}

#define __UA   256
inline int16_t _arctan3(int16_t q, int16_t i)
{
#define __atan2(z)  (__UA/8  + __UA/22) * z  // very much of a simplification...not accurate at all, but fast
	//#define __atan2(z)  (__UA/8 - __UA/22 * z + __UA/22) * z  //derived from (5) [1]
	int16_t r;
	if (abs(q) > abs(i))
		r = __UA / 4 - __atan2(abs(i) / abs(q));        // arctan(z) = 90-arctan(1/z)
	else
		r = (i == 0) ? 0 : __atan2(abs(q) / abs(i));   // arctan(z)
	r = (i < 0) ? __UA / 2 - r : r;                  // arctan(-z) = -arctan(z)
	return (q < 0) ? -r : r;                        // arctan(-z) = -arctan(z)
}

static uint32_t absavg256 = 0;
volatile uint32_t _absavg256 = 0;
volatile int16_t i, q;
int16_t v[14];  // Process Q (down-sampled) samples
int16_t vi[7];  // G8RDI mod, global & renamed 2nd v array to avoid confusions

inline int16_t slow_dsp(int16_t i_ac2, int16_t q_ac2)
{
	// Post processing I and Q (down-sampled) results here - G8DI mod
	int16_t ac, qh, acm;

	q_ac2 >>= att2;  // digital gain control

	// Hilbert transform, BasicDSP model:  outi= fir(inl,  0, 0, 0, 0, 0,  0,  0, 1,   0, 0,   0, 0,  0, 0, 0, 0); outq = fir(inr, 2, 0, 8, 0, 21, 0, 79, 0, -79, 0, -21, 0, -8, 0, -2, 0) / 128;
	if (mode != AM && mode != FM)      // G8RDI AM/FM modd
	{   // !!!! NOTE: Keep this block separated as we have two different static v[] arrays here!!

		qh = ((v[0] - q_ac2) + (v[2] - v[12]) * 4) / 64 + ((v[4] - v[10]) + (v[6] - v[8])) / 8 + ((v[4] - v[10]) * 5 - (v[6] - v[8])) / 128 + (v[6] - v[8]) / 2;
		// Hilbert transform, 43dB side-band rejection in 650..3400Hz (@8kSPS) when used in image-rejection scenario; (Hilbert transform require 4 additional bits)

		//qh = ((v[0] - q_ac2) * 2 + (v[2] - v[12]) * 8 + (v[4] - v[10]) * 21 + (v[6] - v[8]) * 15) / 128 + (v[6] - v[8]) / 2;
		// Hilbert transform, 40dB side-band rejection in 400..1900Hz (@4kSPS) when used in image-rejection scenario; (Hilbert transform require 5 additional bits)

		// Shuffle Q sample along local v[13] -> v[0]:
		v[0] = v[1]; v[1] = v[2]; v[2] = v[3]; v[3] = v[4]; v[4] = v[5]; v[5] = v[6]; v[6] = v[7]; v[7] = v[8]; v[8] = v[9]; v[9] = v[10]; v[10] = v[11]; v[11] = v[12]; v[12] = v[13]; v[13] = q_ac2;
	}   // Do not remove, separate block for separate v[]!!!!

	i_ac2 >>= att2;  // digital gain control

	i = i_ac2; q = q_ac2;   // tbd: this can be more efficient (G8RDI, for what, for AM and FM modes that don't use Hilbert?)

	// Shuffle I sample from below v[6] to v[0]
	int16_t id = vi[0]; vi[0] = vi[1]; vi[1] = vi[2]; vi[2] = vi[3]; vi[3] = vi[4]; vi[4] = vi[5]; vi[5] = vi[6]; vi[6] = i_ac2;  // Delay to match Hilbert transform on Q branch
	// G8RDI mod - changed name from "i" to "id" as a global virtual with same name exists, likely to confuse!

	if (mode == AM)
	{
		acm = -i - q;  // S-Meter

#ifndef AM_MOD_MAGN_SQRT
		ac = magn(i, q);  // AM = SQRT (I * I + Q * Q)
#else
		int32_t iR, qR;
		int16_t i2 = i, q2 = q;
		MultiS16X16to32(iR, i, i2);
		MultiS16X16to32(qR, q, q2);
		ac = (int16_t)sqrt(iR + qR);
#endif
		/*  static int16_t dc = 0;	// LP filter
				dc += (ac - dc) / 2;		// Limit rate of change
				ac = ac - dc;
		*/
		static int16_t as_last;   // GW8RDI mod - replaced LP filter: DC removal done in sdr_rx()
		int16_t as = ac + (int16_t)((float)as_last * 0.9999f); // Reduce from 0.9999f for less bass response
		ac = as - as_last;
		as_last = as;

		/* FIR LP filter (must add separate filter setup call and coeffs for this alone) - removes carrier tone but does not increase AM quality
		DataList[OldestDataPoint] = ac; // replace oldest data in list

		// multiply sample by coefficient
		result.L = amul(ac, CoefList[0]); // accumulate data in a Long to eliminate rounding errors

		for (int i = 1; i < filterWindowSize; i++) // iterate through the rest of the data (stopping one short of start point)
		{
			OldestDataPoint++;  // increment and wrap pointer
			if (OldestDataPoint >= filterWindowSize)
				OldestDataPoint = 0;

			// multiply sample by coefficient
			result.L += amul(DataList[OldestDataPoint], CoefList[i]); // ASM 32bit = 16bit x 16bit
		}
		result.L = result.L << 1; // bitshift to make up for non 16bit coefficient offset

		ac = result.I[1]; // the H16 part of the Long is the result*/
	}
	else
		if (mode == FM)
		{
			acm = -i - q;  // S-Meter

			//ac = atan(q * i_old - i * q_old, i * i_old + q * q_old);  // Taken from the excellent description by Clint, KA7OEI
#ifdef FM_ARCTAN  // G8RDI mod - enabled FM differentiator
			int16_t z0 = _arctan3(q, i);
			static int16_t z1 = z0; // G8RDI mod - initialised static
			ac = z0 - z1; // Differentiator
			z1 = z0;
#else
			static int16_t zi = i; // G8RDI mod - Note, zi used without being initialised
			ac = ((ac + i) * zi);  // -qh = ac + i
			zi = i;
#endif
			/*static int16_t _q = 0;
			_q = (_q + q) / 2;
			ac = i * _q;  // quadrature detector */

			//ac = ((q > 0) == !(i > 0)) ? 128 : -128; // XOR I/Q zero-cross detector
		}  // needs: p.12 https://www.veron.nl/wp-content/uploads/2014/01/FmDemodulator.pdf
		else
		{  // USB, LSB, CW
			acm = -id - qh;  // SSB & CW: inverting I and Q helps dampening a feedback-loop between PWM out and ADC inputs
			ac = acm;
		}

	static uint8_t absavg256cnt;
	if (!(absavg256cnt--)) { _absavg256 = absavg256; absavg256 = 0; }   // Set S-Meter level
	else absavg256 += abs(acm); // G8RDI mod - acm

#ifdef FAST_AGC
	if (agc == 2) {
		ac = process_agc(ac);
		ac = ac >> (16 - volume);
	}
	else if (agc == 1) {
		ac = process_agc_fast(ac);
		ac = ac >> (16 - volume);
#else
	if (agc == 1) {
		ac = process_agc_fast(ac);
		ac = ac >> (16 - volume);
#endif //!FAST_AGC
	}
	else {
		//ac = ac >> (16-volume);
		if (volume <= 13)    // if no AGC allow volume control to boost weak signals
			ac = ac >> (13 - volume);
		else
			ac = ac << (volume - 13);
	}
	if (nr) ac = process_nr(ac);

	//  if(filt) ac = filt_var(ac) << 2;
	if (filt) ac = filt_var(ac);
	/*
	  if(mode == CW){
	  if(cwdec){  // CW decoder enabled?
		char ch = cw(ac >> 0);
		if(ch){
		for(int i=0; i!=15;i++) out[i]=out[i+1];
		out[15] = ch;
		cw_event = true;
		}
	  }
	  }*/
#ifdef CW_DECODER
	if (!(absavg256cnt % 64)) { _amp32 = amp32; amp32 = 0; }
	else amp32 += abs(acm);     // G8RDI mod - acm
#endif  //CW_DECODER
	//if(!(absavg256cnt--)){ _absavg256 = absavg256; absavg256 = 0; } else absavg256 += abs(ac);  //hack

	//static int16_t dc;
	//dc += (ac - dc) / 2;
	//dc = (15*dc + ac)/16;
	//dc = (15*dc + (ac - dc))/16;
	//ac = ac - dc;    // DC decoupling (DC removal)

	ac = min(max(ac, -512), 511);	// Limit to -511 to +511
	//ac = min(max(ac, -128), 127);
#ifdef QCX
	if (!dsp_cap)
		return 0;  // in QCX-SSB mode (no DSP), slow_dsp() should return 0 (in order to prevent upsampling filter to generate audio)
#endif
	return ac;
	}

#ifdef TESTBENCH
// Sine table with 72 entries results in 868Hz sine wave at effective sampling rate of 31250 SPS
// for each of I and Q, since thay are sampled alternately, and hence I (for example) only
// gets 36 samples from this table before looping.
const int8_t sine[] = {
  11, 22, 33, 43, 54, 64, 73, 82, 90, 97, 104, 110, 115, 119, 123, 125, 127, 127, 127, 125, 123, 119, 115, 110, 104, 97, 90, 82, 73, 64, 54, 43, 33, 22, 11, 0, -11, -22, -33, -43, -54, -64, -73, -82, -90, -97, -104, -110, -115, -119, -123, -125, -127, -127, -127, -125, -123, -119, -115, -110, -104, -97, -90, -82, -73, -64, -54, -43, -33, -22, -11, 0
};

// Short Sine table with 36 entries results in 1736Hz sine wave at effective sampling rate of 62500 SPS.
/* const int8_t sine[] = {
  22, 43, 64, 82, 97, 110, 119, 125, 127, 125, 119, 110, 97, 82, 64, 43, 22, 0, -22, -43, -64, -82, -97, -110, -119, -125, -127, -125, -119, -110, -97, -82, -64, -43, -22, 0
}; */

uint8_t ncoIdx = 0;
int16_t NCO_Q()
{
	ncoIdx++;
	if (ncoIdx >= sizeof(sine))
		ncoIdx = 0;
	return (int16_t(sine[ncoIdx])) << 2;
}

int16_t NCO_I()
{
	uint8_t i;

	ncoIdx++;
	if (ncoIdx >= sizeof(sine))
		ncoIdx = 0;

	i = ncoIdx + (sizeof(sine) / 4);  // Advance by 90 degrees
	if (i >= sizeof(sine))
		i -= sizeof(sine);
	return (int16_t(sine[i])) << 2;
}
#endif // TESTBENCH

volatile uint8_t cat_streaming = 0;
volatile uint8_t _cat_streaming = 0;

typedef void(*func_t)(void);
volatile func_t func_ptr;
#undef  R  // Decimating 2nd Order CIC filter
#define R 4  // Rate change from 62500/2 kSPS to 7812.5SPS, providing 12dB gain

//#define SIMPLE_RX  1
#ifndef SIMPLE_RX
volatile uint8_t admux[3];
volatile int16_t ocomb, qh;
volatile uint8_t rx_state = 0;

#pragma GCC push_options
#pragma GCC optimize ("Ofast")  // compiler-optimization for speed

// Non-recursive CIC Filter (M=2, R=4) implementation, so two-stages of (followed by down-sampling with factor 2):
// H1(z) = (1 + z^-1)^2 = 1 + 2*z^-1 + z^-2 = (1 + z^-2) + (2) * z^-1 = FA(z) + FB(z) * z^-1;
// with down-sampling before stage translates into poly-phase components: FA(z) = 1 + z^-1, FB(z) = 2
// Non-recursive CIC Filter (M=4) implementation (for second-stage only):
// H1(z) = (1 + z^-1)^4 = 1 + 4*z^-1 + 6*z^-2 + 4*z^-3 + z^-4 = 1 + 6*z^-2 + z^-4 + (4 + 4*z^-2) * z^-1 = FA(z) + FB(z) * z^-1;
// with down-sampling before stage translates into poly-phase components: FA(z) = 1 + 6*z^-1 + z^-2, FB(z) = 4 + 4*z^-1
// M=3 FA(z) = 1 + 3*z^-1, FB(z) = 3 + z^-1
// source: Lyons Understanding Digital Signal Processing 3rd edition 13.24.1

/* Basicdsp simulation:
# M=2 FA(z) = 1 + z^-1, FB(z) = 2
# M=3 FA(z) = 1 + 3*z^-1, FB(z) = 3 + z^-1
# M=4 FA(z) = 1 + 6*z^-1 + z^-2, FB(z) = 4 + 4*z^-1
samplerate=28000
x=x+1
clk1=mod1(x/2)*2
y=y+clk1
clk2=mod1(y/2)*2
#s1=clk1*fir(in, 1, 2, 1, 0)/16
#s2=clk2*fir(s1, 1, 0, 2, 0, 1, 0, 0)/16
#s1=clk1*fir(in, 1, 3, 3, 1, 0)/16
#s2=clk2*fir(s1, 1, 0, 3, 0, 3, 0, 1, 0, 0)/16
s1=clk1*fir(in, 1, 4, 6, 4, 1, 0)/16
s2=clk2*fir(s1, 1, 0, 4, 0, 6, 0, 4, 0, 1, 0, 0)/16
out=s2
 */

#define NEW_RX  1   // Faster (3rd-order) CIC stage, with simultanuous processing capability
#ifdef NEW_RX
#define AF_OUT  1   // Enables audio output stage (can be disabled in conjunction with CAT_STREAMING to save memory)

static uint8_t tc = 0;

void process(int16_t i_ac2, int16_t q_ac2)
{
	static int16_t ac3;
#ifdef CAT_STREAMING
	//UCSR0B &= ~(TXCIE0);  // disable USART TX interrupts
	//while (!( UCSR0A & (1<<UDRE0)));  // wait for empty buffer
	if (cat_enabled && cat_streaming) { uint8_t out = ac3 + 128; if (out == ';') out++; Serial.write(out); }  //UDR0 = (uint8_t)(ac3 + 128);   // from:  https://www.xanthium.in/how-to-avr-atmega328p-microcontroller-usart-uart-embedded-programming-avrgcc
#endif // CAT_STREAMING

#ifdef AF_OUT
	static int16_t ozd1, ozd2;  // Audio output stage
	if (_init)
	{
		ac3 = 0; ozd1 = 0; ozd2 = 0; _init = 0;       // G8RDI mod - todo move to Setup() // hack: on first sample init accumlators of further stages (to prevent instability)
	}

	int16_t od1 = ac3 - ozd1; // Comb section
	ocomb = od1 - ozd2;
#endif //AF_OUT

#define OUTLET  1
#ifdef OUTLET
	if (tc++ == 0)   // prevent recursion : If tc == 0 reneable interrupts, below we dec back to zero on exit, but if another interrupt enters b4, it will not reenable ints.
		//if(tc++ > 16)   // prevent recursion
#endif
		interrupts();  // hack, since slow_dsp process exceeds rx sample-time, allow subsequent 7 interrupts for further rx sampling while processing, prevent nested interrupts with tc

#ifdef AF_OUT
	ozd2 = od1;
	ozd1 = ac3;
#endif  //AF_OUT

	//int16_t qh, aco;
	//q_ac2 >>= att2;  // digital gain control

	//// Hilbert transform, BasicDSP model:  outi= fir(inl,  0, 0, 0, 0, 0,  0,  0, 1,   0, 0,   0, 0,  0, 0, 0, 0); outq = fir(inr, 2, 0, 8, 0, 21, 0, 79, 0, -79, 0, -21, 0, -8, 0, -2, 0) / 128;
	//if (mode != AM && mode != FM)      // G8RDI AM/FM modd
	//{   // !!!! NOTE: Keep this block separated as we have two different static v[] arrays here!!

	//    qh = ((v[0] - q_ac2) + (v[2] - v[12]) * 4) / 64 + ((v[4] - v[10]) + (v[6] - v[8])) / 8 + ((v[4] - v[10]) * 5 - (v[6] - v[8])) / 128 + (v[6] - v[8]) / 2;
	//    // Hilbert transform, 43dB side-band rejection in 650..3400Hz (@8kSPS) when used in image-rejection scenario; (Hilbert transform require 4 additional bits)

	//    //qh = ((v[0] - q_ac2) * 2 + (v[2] - v[12]) * 8 + (v[4] - v[10]) * 21 + (v[6] - v[8]) * 15) / 128 + (v[6] - v[8]) / 2;
	//    // Hilbert transform, 40dB side-band rejection in 400..1900Hz (@4kSPS) when used in image-rejection scenario; (Hilbert transform require 5 additional bits)

	//    // Shuffle Q sample along local v[13] -> v[0]:
	//    v[0] = v[1]; v[1] = v[2]; v[2] = v[3]; v[3] = v[4]; v[4] = v[5]; v[5] = v[6]; v[6] = v[7]; v[7] = v[8]; v[8] = v[9]; v[9] = v[10]; v[10] = v[11]; v[11] = v[12]; v[12] = v[13]; v[13] = q_ac2;
	//}   // Do not remove, separate block for separate v[]!!!!

	//i_ac2 >>= att2;  // digital gain control

	//// Shuffle I sample from below v[6] to v[0]
	//i = i_ac2; q = q_ac2;   // tbd: this can be more efficient (G8RDI, for what, for AM and FM modes that don't use Hilbert?)

	//int16_t id = vi[0]; vi[0] = vi[1]; vi[1] = vi[2]; vi[2] = vi[3]; vi[3] = vi[4]; vi[4] = vi[5]; vi[5] = vi[6]; vi[6] = i_ac2;  // Delay to match Hilbert transform on Q branch
	//// G8RDI mod - changed name from "i" to "id" as a global virtual with same name exists, likely to confuse!

	//if (mode == AM || mode == FM)      // G8RDI AM/FM mods:
	//{
	//    aco = -id - qh;  // SSB & CW: inverting I and Q helps dampening a feedback-loop between PWM out and ADC inputs
	//}
	//else
	//{
	//    aco = xyzzy
	//}

	ac3 = slow_dsp(i_ac2, q_ac2);  // G8RDI mod - SSB & CW: inverting I and Q helps dampening a feedback-loop between PWM out and ADC inputs
	///ac3 = slow_dsp(-id - qh);  // SSB & CW: inverting I and Q helps dampening a feedback-loop between PWM out and ADC inputs

#ifdef OUTLET
	tc--;
#endif
}

/*
// # M=3 .. = i0 + 3*(i2 + i3) + i1
int16_t i0, i1, i2, i3, i4, i5, i6, i7, i8;
int16_t q0, q1, q2, q3, q4, q5, q6, q7, q8;
#define M_SR  1

//#define EXPANDED_CIC
#ifdef EXPANDED_CIC
void sdr_rx_00(){         i0 = sdr_rx_common_i(); func_ptr = sdr_rx_01;   i4 = (i0 + (i2 + i3) * 3 + i1) >> M_SR; }
void sdr_rx_02(){         i1 = sdr_rx_common_i(); func_ptr = sdr_rx_03;   i8 = (i4 + (i6 + i7) * 3 + i5) >> M_SR; }
void sdr_rx_04(){         i2 = sdr_rx_common_i(); func_ptr = sdr_rx_05;   i5 = (i2 + (i0 + i1) * 3 + i3) >> M_SR; }
void sdr_rx_06(){         i3 = sdr_rx_common_i(); func_ptr = sdr_rx_07; }
void sdr_rx_08(){         i0 = sdr_rx_common_i(); func_ptr = sdr_rx_09;   i6 = (i0 + (i2 + i3) * 3 + i1) >> M_SR; }
void sdr_rx_10(){         i1 = sdr_rx_common_i(); func_ptr = sdr_rx_11;   i8 = (i6 + (i4 + i5) * 3 + i7) >> M_SR; }
void sdr_rx_12(){         i2 = sdr_rx_common_i(); func_ptr = sdr_rx_13;   i7 = (i2 + (i0 + i1) * 3 + i3) >> M_SR; }
void sdr_rx_14(){         i3 = sdr_rx_common_i(); func_ptr = sdr_rx_15; }
void sdr_rx_15(){         q0 = sdr_rx_common_q(); func_ptr = sdr_rx_00;   q4 = (q0 + (q2 + q3) * 3 + q1) >> M_SR; }
void sdr_rx_01(){         q1 = sdr_rx_common_q(); func_ptr = sdr_rx_02;   q8 = (q4 + (q6 + q7) * 3 + q5) >> M_SR; }
void sdr_rx_03(){         q2 = sdr_rx_common_q(); func_ptr = sdr_rx_04;   q5 = (q2 + (q0 + q1) * 3 + q3) >> M_SR; }
void sdr_rx_05(){         q3 = sdr_rx_common_q(); func_ptr = sdr_rx_06; process(i8, q8); }
void sdr_rx_07(){         q0 = sdr_rx_common_q(); func_ptr = sdr_rx_08;   q6 = (q0 + (q2 + q3) * 3 + q1) >> M_SR; }
void sdr_rx_09(){         q1 = sdr_rx_common_q(); func_ptr = sdr_rx_10;   q8 = (q6 + (q4 + q5) * 3 + q7) >> M_SR; }
void sdr_rx_11(){         q2 = sdr_rx_common_q(); func_ptr = sdr_rx_12;   q7 = (q2 + (q0 + q1) * 3 + q3) >> M_SR; }
void sdr_rx_13(){         q3 = sdr_rx_common_q(); func_ptr = sdr_rx_14; process(i8, q8); }
#else
void sdr_rx_00(){         i0 = sdr_rx_common_i(); func_ptr = sdr_rx_01;   i4 = (i0 + (i2 + i3) * 3 + i1) >> M_SR; }
void sdr_rx_02(){         i1 = sdr_rx_common_i(); func_ptr = sdr_rx_03;   i8 = (i4 + (i6 + i7) * 3 + i5) >> M_SR; }
void sdr_rx_04(){         i2 = sdr_rx_common_i(); func_ptr = sdr_rx_05;   i5 = (i2 + (i0 + i1) * 3 + i3) >> M_SR; }
void sdr_rx_06(){         i3 = sdr_rx_common_i(); func_ptr = sdr_rx_07;   i6 = i4; i7 = i5; q6 = q4; q7 = q5; }
void sdr_rx_07(){         q0 = sdr_rx_common_q(); func_ptr = sdr_rx_00;   q4 = (q0 + (q2 + q3) * 3 + q1) >> M_SR; }
void sdr_rx_01(){         q1 = sdr_rx_common_q(); func_ptr = sdr_rx_02;   q8 = (q4 + (q6 + q7) * 3 + q5) >> M_SR; }
void sdr_rx_03(){         q2 = sdr_rx_common_q(); func_ptr = sdr_rx_04;   q5 = (q2 + (q0 + q1) * 3 + q3) >> M_SR; }
void sdr_rx_05(){         q3 = sdr_rx_common_q(); func_ptr = sdr_rx_06; process(i8, q8); }
#endif
*/

// /*
static int16_t i_s0za1, i_s0zb0, i_s0zb1, i_s1za1, i_s1zb0, i_s1zb1;
static int16_t q_s0za1, q_s0zb0, q_s0zb1, q_s1za1, q_s1zb0, q_s1zb1, q_ac2;

// G8RDI mod - added pre-definitions for Visual Studio errors
void sdr_rx_03(); void sdr_rx_07();

#define M_SR  1  // CIC N=3
void sdr_rx_00() { int16_t ac = sdr_rx_common_i(); func_ptr = sdr_rx_01;  int16_t i_s1za0 = (ac + (i_s0za1 + i_s0zb0) * 3 + i_s0zb1) >> M_SR; i_s0za1 = ac; int16_t ac2 = (i_s1za0 + (i_s1za1 + i_s1zb0) * 3 + i_s1zb1); i_s1za1 = i_s1za0; process(ac2, q_ac2); }
void sdr_rx_02() { int16_t ac = sdr_rx_common_i(); func_ptr = sdr_rx_03;  i_s0zb1 = i_s0zb0; i_s0zb0 = ac; }
void sdr_rx_04() { int16_t ac = sdr_rx_common_i(); func_ptr = sdr_rx_05;  i_s1zb1 = i_s1zb0; i_s1zb0 = (ac + (i_s0za1 + i_s0zb0) * 3 + i_s0zb1) >> M_SR; i_s0za1 = ac; }
void sdr_rx_06() { int16_t ac = sdr_rx_common_i(); func_ptr = sdr_rx_07;  i_s0zb1 = i_s0zb0; i_s0zb0 = ac; }

void sdr_rx_01() { int16_t ac = sdr_rx_common_q(); func_ptr = sdr_rx_02;  q_s0zb1 = q_s0zb0; q_s0zb0 = ac; }
void sdr_rx_03() { int16_t ac = sdr_rx_common_q(); func_ptr = sdr_rx_04;  q_s1zb1 = q_s1zb0; q_s1zb0 = (ac + (q_s0za1 + q_s0zb0) * 3 + q_s0zb1) >> M_SR; q_s0za1 = ac; }
void sdr_rx_05() { int16_t ac = sdr_rx_common_q(); func_ptr = sdr_rx_06;  q_s0zb1 = q_s0zb0; q_s0zb0 = ac; }
void sdr_rx_07() { int16_t ac = sdr_rx_common_q(); func_ptr = sdr_rx_00;  int16_t q_s1za0 = (ac + (q_s0za1 + q_s0zb0) * 3 + q_s0zb1) >> M_SR; q_s0za1 = ac; q_ac2 = (q_s1za0 + (q_s1za1 + q_s1zb0) * 3 + q_s1zb1); q_s1za1 = q_s1za0; }
// */

/*
static int16_t i_s0za1, i_s0za2, i_s0zb0, i_s0zb1, i_s1za1, i_s1za2, i_s1zb0, i_s1zb1;
static int16_t q_s0za1, q_s0za2, q_s0zb0, q_s0zb1, q_s1za1, q_s1za2, q_s1zb0, q_s1zb1, q_ac2;

#define M_SR  0  // CIC N=2
void sdr_rx_00(){ int16_t ac = sdr_rx_common_i(); func_ptr = sdr_rx_01;  int16_t i_s1za0 = (ac + i_s0za1 + i_s0zb0 * 2 + i_s0zb1) >> M_SR; i_s0za1 = ac; int16_t ac2 = (i_s1za0 + i_s1za1 + i_s1zb0 * 2); i_s1za1 = i_s1za0; process(ac2, q_ac2); }
void sdr_rx_02(){ int16_t ac = sdr_rx_common_i(); func_ptr = sdr_rx_03;  i_s0zb0 = ac; }
void sdr_rx_04(){ int16_t ac = sdr_rx_common_i(); func_ptr = sdr_rx_05;  i_s1zb0 = (ac + i_s0za1 + i_s0zb0 * 2) >> M_SR; i_s0za1 = ac; }
void sdr_rx_06(){ int16_t ac = sdr_rx_common_i(); func_ptr = sdr_rx_07;  i_s0zb0 = ac; }
void sdr_rx_01(){ int16_t ac = sdr_rx_common_q(); func_ptr = sdr_rx_02;  q_s0zb0 = ac; }
void sdr_rx_03(){ int16_t ac = sdr_rx_common_q(); func_ptr = sdr_rx_04;  q_s1zb0 = (ac + q_s0za1 + q_s0zb0 * 2) >> M_SR; q_s0za1 = ac; }
void sdr_rx_05(){ int16_t ac = sdr_rx_common_q(); func_ptr = sdr_rx_06;  q_s0zb0 = ac; }
void sdr_rx_07(){ int16_t ac = sdr_rx_common_q(); func_ptr = sdr_rx_00;  int16_t q_s1za0 = (ac + q_s0za1 + q_s0zb0 * 2 + q_s0zb1) >> M_SR; q_s0za1 = ac; q_ac2 = (q_s1za0 + q_s1za1 + q_s1zb0 * 2); q_s1za1 = q_s1za0; }
*/

static int16_t ozi1, ozi2;

inline int16_t sdr_rx_common_q() {
	ADMUX = admux[0]; ADCSRA |= (1 << ADSC); int16_t ac = ADC - 511;
	/*ozi2 = ozi1 + ozi2;          // Integrator section - needed?
	  ozi1 = ocomb + ozi1;
	  OCR1AL = min(max(128 - (ozi2>>5) + 128, 0), 255); */
	return ac;
}

inline int16_t sdr_rx_common_i()    // Get RX AC samples
{
	ADMUX = admux[1]; ADCSRA |= (1 << ADSC);

	int16_t adc = ADC - 511;	// DC removal based on ADC input bias voltage

	static int16_t prev_adc;
	//GW8RDI int16_t ac = (prev_adc + adc) / 2;	// Average of 2 samples
	int16_t ac = (prev_adc + adc) >> 1;	// GW8RDI Shift faster for div
	prev_adc = adc;

#ifdef AF_OUT
	if (_init) { ocomb = 0; ozi1 = 0; ozi2 = 0; } // hack - G8RDI mod todo could go into Setup() ??????
	ozi2 = ozi1 + ozi2;          // Integrator section
	ozi1 = ocomb + ozi1;
	OCR1AL = min(max((ozi2 >> 5) + 128, 0), 255);   // Output to audio PWM port
#endif // AF_OUT
	return ac;
}

/*
#define M_SR  2  // CIC N=3
static uint8_t nested = false;

void sdr_rx()
{
#ifdef TESTBENCH
  int16_t adc = NCO_I();
#else
  ADMUX = admux[1];  // set MUX for next conversion
  ADCSRA |= (1 << ADSC);    // start next ADC conversion
  int16_t adc = ADC - 511; // current ADC sample 10-bits analog input, NOTE: first ADCL, then ADCH
#endif
  func_ptr = sdr_rx_q;    // processing function for next conversion
  sdr_rx_common();

  static int16_t prev_adc;
  int16_t corr_adc = (prev_adc + adc) / 2;  // Only for I: correct I/Q sample delay by means of linear interpolation
  prev_adc = adc;
  adc = corr_adc;
  //static int16_t dc;
  //dc += (adc - dc) / 2;  // we lose LSB with this method
  //dc = (3*dc + adc)/4;
  //int16_t ac = adc - dc;     // DC decoupling
  int16_t ac = adc;

  static int16_t s0zb0, s0zb1;
  if(rx_state == 0 || rx_state == 4){  // stage s0: down-sample by 2
  static int16_t s0za1, s0za2;
  int16_t s1za0 = (ac + (s0za1 + s0zb0) * 3 + s0zb1) >> M_SR;           // FA + FB
  //int16_t s1za0 = (ac + s0za1 * 6 + s0za2 + s0zb0 + s0zb1);
  //s0za2 = s0za1;
  s0za1 = ac;
  static int16_t s1zb0, s1zb1;
  if(rx_state == 0){                   // stage s1: down-sample by 2
	static int16_t s1za1, s1za2;
	int16_t ac2 = (s1za0 + (s1za1 + s1zb0) * 3 + s1zb1) >> M_SR; // FA + FB $
	//int16_t ac2 = (s1za0 + s1za1 * 6 + s1za2 + s1zb0 + s1zb1); // FA + FB $
	//s1za2 = s1za1; // $
	s1za1 = s1za0;
	{
	rx_state++;

	static int16_t ac3;
	static int16_t ozd1, ozd2;  // Output stage
	if(_init){ ac3 = 0; ozd1 = 0; ozd2 = 0; _init = 0; } // hack: on first sample init accumlators of further stages (to prevent instability)
	int16_t od1 = ac3 - ozd1; // Comb section
	ocomb = od1 - ozd2;
	interrupts();
	ozd2 = od1;
	ozd1 = ac3;

	//if(nested){ return; } // fuse for too many nested interrupts (prevent stack overflow)
	//nested++;
	//interrupts();  // hack: post processing may be extend until next sample time: allow next sample to be processed while postprocessing

	{
	  q_ac2 >>= att2;  // digital gain control
	  static int16_t v[14];  // Process Q (down-sampled) samples
	  q = v[7];
	  // Hilbert transform, BasicDSP model:  outi= fir(inl,  0, 0, 0, 0, 0,  0,  0, 1,   0, 0,   0, 0,  0, 0, 0, 0); outq = fir(inr, 2, 0, 8, 0, 21, 0, 79, 0, -79, 0, -21, 0, -8, 0, -2, 0) / 128;
	  qh = ((v[0] - q_ac2) + (v[2] - v[12]) * 4) / 64 + ((v[4] - v[10]) + (v[6] - v[8])) / 8 + ((v[4] - v[10]) * 5 - (v[6] - v[8]) ) / 128 + (v[6] - v[8]) / 2; // Hilbert transform, 43dB side-band rejection in 650..3400Hz (@8kSPS) when used in image-rejection scenario; (Hilbert transform require 4 additional bits)
	  //qh = ((v[0] - q_ac2) * 2 + (v[2] - v[12]) * 8 + (v[4] - v[10]) * 21 + (v[6] - v[8]) * 15) / 128 + (v[6] - v[8]) / 2; // Hilbert transform, 40dB side-band rejection in 400..1900Hz (@4kSPS) when used in image-rejection scenario; (Hilbert transform require 5 additional bits)
	  v[0] = v[1]; v[1] = v[2]; v[2] = v[3]; v[3] = v[4]; v[4] = v[5]; v[5] = v[6]; v[6] = v[7]; v[7] = v[8]; v[8] = v[9]; v[9] = v[10]; v[10] = v[11]; v[11] = v[12]; v[12] = v[13]; v[13] = q_ac2;
	}
	ac2 >>= att2;  // digital gain control
	static int16_t v[7];  // Post processing I and Q (down-sampled) results
	i = v[0]; v[0] = v[1]; v[1] = v[2]; v[2] = v[3]; v[3] = v[4]; v[4] = v[5]; v[5] = v[6]; v[6] = ac2;  // Delay to match Hilbert transform on Q branch
	ac3 = slow_dsp(i + qh);

	//nested--;
	return;
	}
  } else { s1zb1 = s1zb0; s1zb0 = s1za0; } // rx_state == 4 // *4
  } else { s0zb1 = s0zb0; s0zb0 = ac; }  // rx_state == 2 || rx_state == 6  // *4

  rx_state++;
}

void sdr_rx_q()
{
#ifdef TESTBENCH
  int16_t adc = NCO_Q();
#else
  ADMUX = admux[0];  // set MUX for next conversion
  ADCSRA |= (1 << ADSC);    // start next ADC conversion
  int16_t adc = ADC - 511; // current ADC sample 10-bits analog input, NOTE: first ADCL, then ADCH
#endif
  func_ptr = sdr_rx;    // processing function for next conversion
  //sdr_rx_common();  //necessary? YES!... Maybe NOT!

  //static int16_t dc;
  //dc += (adc - dc) / 2;  // we lose LSB with this method
  //dc = (3*dc + adc)/4;
  //int16_t ac = adc - dc;     // DC decoupling
  int16_t ac = adc;

  static int16_t s0zb0, s0zb1;
  if(rx_state == 3 || rx_state == 7){  // stage s0: down-sample by 2
  static int16_t s0za1, s0za2;
  int16_t s1za0 = (ac + (s0za1 + s0zb0) * 3 + s0zb1) >> M_SR;           // FA + FB
  //int16_t s1za0 = (ac + s0za1 * 6 + s0za2 + s0zb0 + s0zb1);
  //s0za2 = s0za1;
  s0za1 = ac;
  static int16_t s1zb0, s1zb1;
  if(rx_state == 7){                   // stage s1: down-sample by 2
	static int16_t s1za1, s1za2;
	q_ac2 = (s1za0 + (s1za1 + s1zb0) * 3 + s1zb1) >> M_SR; // FA + FB $
	//q_ac2 = (s1za0 + s1za1 * 6 + s1za2 + s1zb0 + s1zb1); // FA + FB $
	//s1za2 = s1za1; // $
	s1za1 = s1za0;
	rx_state = 0; return;
  } else { s1zb1 = s1zb0; s1zb0 = s1za0; } // rx_state == 3  // *4
  } else { s0zb1 = s0zb0; s0zb0 = ac; }  // rx_state == 1 || rx_state == 5  // *4

  rx_state++;
}

inline void sdr_rx_common()
{
  static int16_t ozi1, ozi2;
  if(_init){ ocomb=0; ozi1 = 0; ozi2 = 0; } // hack
  // Output stage [25% CPU@R=4;Fs=62.5k]
#ifdef SECOND_ORDER_DUC
  ozi2 = ozi1 + ozi2;          // Integrator section
#endif
  ozi1 = ocomb + ozi1;
#ifdef SECOND_ORDER_DUC
  OCR1AL = min(max((ozi2>>5) + 128, 0), 255);  // OCR1AL = min(max((ozi2>>5) + ICR1L/2, 0), ICR1L);  // center and clip wrt PWM working range
#else
  OCR1AL = (ozi1>>5) + 128;
  // OCR1AL = min(max((ozi1>>5) + 128, 0), 255);  // OCR1AL = min(max((ozi2>>5) + ICR1L/2, 0), ICR1L);  // center and clip wrt PWM working range
#endif
}
*/

#else // OLD_RX    //Orginal 2nd-order CIC:
 //#define M4  1  // Enable to enable M=4 on second-stage (better alias rejection)

void sdr_rx()
{
	// process I for even samples  [75% CPU@R=4;Fs=62.5k] (excluding the Comb branch and output stage)
	ADMUX = admux[1];  // set MUX for next conversion
	ADCSRA |= (1 << ADSC);    // start next ADC conversion
	int16_t adc = ADC - 511; // current ADC sample 10-bits analog input, NOTE: first ADCL, then ADCH
	func_ptr = sdr_rx_q;    // processing function for next conversion
	sdr_rx_common();

	// Only for I: correct I/Q sample delay by means of linear interpolation
	static int16_t prev_adc;
	//int16_t corr_adc = (prev_adc + adc) / 2;
	int16_t corr_adc = (prev_adc + adc) >> 1;	// GW8RDI faster
	prev_adc = adc;
	adc = corr_adc;

	//static int16_t dc;
	//dc += (adc - dc) / 2;  // we lose LSB with this method
	//dc = (3*dc + adc)/4;
	//int16_t ac = adc - dc;     // DC decoupling
	int16_t ac = adc;

	int16_t ac2;
	static int16_t z1;
	if (rx_state == 0 || rx_state == 4) {  // 1st stage: down-sample by 2
		static int16_t za1;
		int16_t _ac = ac + za1 + z1 * 2;           // 1st stage: FA + FB
		za1 = ac;
		static int16_t _z1;
		if (rx_state == 0) {                   // 2nd stage: down-sample by 2
			static int16_t _za1;
			ac2 = _ac + _za1 + _z1 * 2;              // 2nd stage: FA + FB
			_za1 = _ac;
			{
				ac2 >>= att2;  // digital gain control
				// post processing I and Q (down-sampled) results
				static int16_t v[7];
				i = v[0]; v[0] = v[1]; v[1] = v[2]; v[2] = v[3]; v[3] = v[4]; v[4] = v[5]; v[5] = v[6]; v[6] = ac2;  // Delay to match Hilbert transform on Q branch
				xx disabled
					int16_t ac = i + qh;
				ac = slow_dsp(ac);

				// Output stage
				static int16_t ozd1, ozd2;
				if (_init) { ac = 0; ozd1 = 0; ozd2 = 0; _init = 0; } // hack: on first sample init accumlators of further stages (to prevent instability)
#define SECOND_ORDER_DUC  1
#ifdef SECOND_ORDER_DUC
				int16_t od1 = ac - ozd1; // Comb section
				ocomb = od1 - ozd2;
				ozd2 = od1;
#else
				ocomb = ac - ozd1; // Comb section
#endif
				ozd1 = ac;
			}
		}
		else _z1 = _ac;
	}
	else z1 = ac;

	rx_state++;
}

void sdr_rx_q()
{
	// process Q for odd samples  [75% CPU@R=4;Fs=62.5k] (excluding the Comb branch and output stage)
#ifdef TESTBENCH
	int16_t adc = NCO_Q();
#else
	ADMUX = admux[0];  // set MUX for next conversion
	ADCSRA |= (1 << ADSC);    // start next ADC conversion
	int16_t adc = ADC - 511; // current ADC sample 10-bits analog input, NOTE: first ADCL, then ADCH
#endif
	func_ptr = sdr_rx;    // processing function for next conversion
#ifdef SECOND_ORDER_DUC
	//  sdr_rx_common();  //necessary? YES!... Maybe NOT!
#endif

  //static int16_t dc;
  //dc += (adc - dc) / 2;  // we lose LSB with this method
  //dc = (3*dc + adc)/4;
  //int16_t ac = adc - dc;     // DC decoupling
	int16_t ac = adc;

	int16_t ac2;
	static int16_t z1;
	if (rx_state == 3 || rx_state == 7) {  // 1st stage: down-sample by 2
		static int16_t za1;
		int16_t _ac = ac + za1 + z1 * 2;           // 1st stage: FA + FB
		za1 = ac;
		static int16_t _z1;
		if (rx_state == 7) {                   // 2nd stage: down-sample by 2
			static int16_t _za1;
			ac2 = _ac + _za1 + _z1 * 2;              // 2nd stage: FA + FB
			_za1 = _ac;
			{
				ac2 >>= att2;  // digital gain control
				// Process Q (down-sampled) samples
				static int16_t v[14];
				q = v[7];
				// Hilbert transform, BasicDSP model:  outi= fir(inl,  0, 0, 0, 0, 0,  0,  0, 1,   0, 0,   0, 0,  0, 0, 0, 0); outq = fir(inr, 2, 0, 8, 0, 21, 0, 79, 0, -79, 0, -21, 0, -8, 0, -2, 0) / 128;
				qh = ((v[0] - ac2) + (v[2] - v[12]) * 4) / 64 + ((v[4] - v[10]) + (v[6] - v[8])) / 8 + ((v[4] - v[10]) * 5 - (v[6] - v[8])) / 128 + (v[6] - v[8]) / 2; // Hilbert transform, 43dB side-band rejection in 650..3400Hz (@8kSPS) when used in image-rejection scenario; (Hilbert transform require 4 additional bits)
				//qh = ((v[0] - ac2) * 2 + (v[2] - v[12]) * 8 + (v[4] - v[10]) * 21 + (v[6] - v[8]) * 15) / 128 + (v[6] - v[8]) / 2; // Hilbert transform, 40dB side-band rejection in 400..1900Hz (@4kSPS) when used in image-rejection scenario; (Hilbert transform require 5 additional bits)
				for (uint8_t j = 0; j != 13; j++) v[j] = v[j + 1]; v[13] = ac2;
				//v[0] = v[1]; v[1] = v[2]; v[2] = v[3]; v[3] = v[4]; v[4] = v[5]; v[5] = v[6]; v[6] = v[7]; v[7] = v[8]; v[8] = v[9]; v[9] = v[10]; v[10] = v[11]; v[11] = v[12]; v[12] = v[13]; v[13] = ac2;
			}
			rx_state = 0; return;
		}
		else _z1 = _ac;
	}
	else z1 = ac;

	rx_state++;
}

inline void sdr_rx_common()
{
	static int16_t ozi1, ozi2;
	if (_init) { ocomb = 0; ozi1 = 0; ozi2 = 0; } // hack
	// Output stage [25% CPU@R=4;Fs=62.5k]
#ifdef SECOND_ORDER_DUC
	ozi2 = ozi1 + ozi2;          // Integrator section
#endif
	ozi1 = ocomb + ozi1;
#ifdef SECOND_ORDER_DUC
	OCR1AL = min(max((ozi2 >> 5) + 128, 0), 255);  // OCR1AL = min(max((ozi2>>5) + ICR1L/2, 0), ICR1L);  // center and clip wrt PWM working range
#else
	OCR1AL = (ozi1 >> 5) + 128;
	OCR1AL = min(max((ozi1 >> 5) + 128, 0), 255);  // OCR1AL = min(max((ozi2>>5) + ICR1L/2, 0), ICR1L);  // center and clip wrt PWM working range
#endif
}
#endif //OLD_RX

#endif  //!SIMPLE_RX

#ifdef SIMPLE_RX
volatile uint8_t admux[3];
static uint8_t rx_state = 0;

static struct rx {
	int16_t z1;
	int16_t za1;
	int16_t _z1;
	int16_t _za1;
} rx_inst[2];

void sdr_rx()
{
	static int16_t ocomb;
	static int16_t qh;

	uint8_t b = !(rx_state & 0x01);
	rx* p = &rx_inst[b];
	uint8_t _rx_state;
	int16_t ac;
	if (b) {  // rx_state == 0, 2, 4, 6 -> I-stage
		ADMUX = admux[1];  // set MUX for next conversion
		ADCSRA |= (1 << ADSC);    // start next ADC conversion
		ac = ADC - 512; // current ADC sample 10-bits analog input, NOTE: first ADCL, then ADCH (-512 fo DC removal)

		//sdr_common
		static int16_t ozi1, ozi2;
		if (_init) { ocomb = 0; ozi1 = 0; ozi2 = 0; } // hack
		// Output stage [25% CPU@R=4;Fs=62.5k]
#define SECOND_ORDER_DUC 1
#ifdef SECOND_ORDER_DUC
		ozi2 = ozi1 + ozi2;          // Integrator section
#endif
		ozi1 = ocomb + ozi1;
#ifdef SECOND_ORDER_DUC
		OCR1AL = min(max((ozi2 >> 5) + 128, 0), 255);  // OCR1AL = min(max((ozi2>>5) + ICR1L/2, 0), ICR1L);  // center and clip wrt PWM working range
#else
		OCR1AL = (ozi1 >> 5) + 128;
		//OCR1AL = min(max((ozi1>>5) + 128, 0), 255);  // OCR1AL = min(max((ozi2>>5) + ICR1L/2, 0), ICR1L);  // center and clip wrt PWM working range
#endif
// Only for I: correct I/Q sample delay by means of linear interpolation
		static int16_t prev_adc;
		//int16_t corr_adc = (prev_adc + ac) / 2;
		int16_t corr_adc = (prev_adc + ac) >> 1;	// GW8RDI faster
		prev_adc = ac;
		ac = corr_adc;
		_rx_state = ~rx_state;
	}
	else {
		ADMUX = admux[0];  // set MUX for next conversion
		ADCSRA |= (1 << ADSC);    // start next ADC conversion
		ac = ADC - 512; // current ADC sample 10-bits analog input, NOTE: first ADCL, then ADCH
		_rx_state = rx_state;
	}

	if (_rx_state & 0x02) {  // rx_state == I: 0, 4  Q: 3, 7  1st stage: down-sample by 2
		int16_t _ac = ac + p->za1 + p->z1 * 2;           // 1st stage: FA + FB
		p->za1 = ac;
		if (_rx_state & 0x04) {                   // rx_state == I: 0  Q:7   2nd stage: down-sample by 2
			int16_t ac2 = _ac + p->_za1 + p->_z1 * 2;              // 2nd stage: FA + FB
			p->_za1 = _ac;
			if (b) {
				// post processing I and Q (down-sampled) results
				ac2 >>= att2;  // digital gain control
				// post processing I and Q (down-sampled) results
				static int16_t v[7];
				i = v[0]; v[0] = v[1]; v[1] = v[2]; v[2] = v[3]; v[3] = v[4]; v[4] = v[5]; v[5] = v[6]; v[6] = ac2;  // Delay to match Hilbert transform on Q branch

				int16_t ac = i + qh;
				ac = slow_dsp(ac);

				// Output stage
				static int16_t ozd1, ozd2;
				if (_init) { ac = 0; ozd1 = 0; ozd2 = 0; _init = 0; } // hack: on first sample init accumlators of further stages (to prevent instability)
#ifdef SECOND_ORDER_DUC
				int16_t od1 = ac - ozd1; // Comb section
				ocomb = od1 - ozd2;
				ozd2 = od1;
#else
				ocomb = ac - ozd1; // Comb section
#endif
				ozd1 = ac;
			}
			else {
				ac2 >>= att2;  // digital gain control
				// Process Q (down-sampled) samples
				static int16_t v[14];
				q = v[7];
				qh = ((v[0] - ac2) * 2 + (v[2] - v[12]) * 8 + (v[4] - v[10]) * 21 + (v[6] - v[8]) * 15) / 128 + (v[6] - v[8]) / 2; // Hilbert transform, 40dB side-band rejection in 400..1900Hz (@4kSPS) when used in image-rejection scenario; (Hilbert transform require 5 additional bits)
				for (uint8_t j = 0; j != 13; j++) v[j] = v[j + 1]; v[13] = ac2;
			}
		}
		else p->_z1 = _ac;
	}
	else p->z1 = ac;  // rx_state == I: 2, 6  Q: 1, 5

	rx_state++;
}
//#pragma GCC push_options
//#pragma GCC optimize ("Ofast")  // compiler-optimization for speed
//#pragma GCC pop_options  // end of DSP section
// */
#endif //SIMPLE_RX

ISR(TIMER2_COMPA_vect)  // Timer2 COMPA interrupt
{
	func_ptr();
#ifdef DEBUG
	numSamples++;
#endif
}

#pragma GCC pop_options  // end of DSP section

/*ISR (TIMER2_COMPA_vect  ,ISR_NAKED) {
asm("push r24         \n\t"
  "lds r24,  0\n\t"
  "sts 0xB4, r24    \n\t"
  "pop r24          \n\t"
  "reti             \n\t");
}*/

void adc_start(uint8_t adcpin, bool ref1v1, uint32_t fs)
{
	DIDR0 |= (1 << adcpin); // disable digital input 
	ADCSRA = 0;             // clear ADCSRA register
	ADCSRB = 0;             // clear ADCSRB register
	ADMUX = 0;              // clear ADMUX register
	ADMUX |= (adcpin & 0x0f);    // set analog input pin
	ADMUX |= ((ref1v1) ? (1 << REFS1) : 0) | (1 << REFS0);  // If reflvl == true, set AREF=1.1V (Internal ref); otherwise AREF=AVCC=(5V)
	ADCSRA |= ((uint8_t)log2((uint8_t)(F_CPU / 13 / fs))) & 0x07;  // ADC Prescaler (for normal conversions non-auto-triggered): ADPS = log2(F_CPU / 13 / Fs) - 1; ADSP=0..7 resulting in resp. conversion rate of 1536, 768, 384, 192, 96, 48, 24, 12 kHz
	//ADCSRA |= (1 << ADIE);  // enable interrupts when measurement complete
	ADCSRA |= (1 << ADEN);  // enable ADC
	//ADCSRA |= (1 << ADSC);  // start ADC measurements
#ifdef ADC_NR
//  set_sleep_mode(SLEEP_MODE_ADC);  // ADC NR sleep destroys the timer2 integrity, therefore Idle sleep is better alternative (keeping clkIO as an active clock domain)
	set_sleep_mode(SLEEP_MODE_IDLE);
	sleep_enable();
#endif
}

void adc_stop()
{
	//ADCSRA &= ~(1 << ADATE); // disable auto trigger
	ADCSRA &= ~(1 << ADIE);  // disable interrupts when measurement complete
	ADCSRA |= (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);    // 128 prescaler for 9.6kHz
#ifdef ADC_NR
	sleep_disable();
#endif
	ADMUX = (1 << REFS0);  // restore reference voltage AREF (5V)
}

void timer1_start(uint32_t fs)
{  // Timer 1: OC1A and OC1B in PWM mode
	TCCR1A = 0;
	TCCR1B = 0;
	TCCR1A |= (1 << COM1A1) | (1 << COM1B1) | (1 << WGM11); // Clear OC1A/OC1B on compare match, set OC1A/OC1B at BOTTOM (non-inverting mode)
	TCCR1B |= (1 << CS10) | (1 << WGM13) | (1 << WGM12); // Mode 14 - Fast PWM;  CS10: clkI/O/1 (No prescaling)
	ICR1H = 0x00;
	ICR1L = min(255, F_CPU / fs);  // PWM value range (fs>78431):  Fpwm = F_CPU / [Prescaler * (1 + TOP)]
	//TCCR1A |= (1 << COM1A1) | (1 << COM1B1) | (1 << WGM10); // Clear OC1A/OC1B on compare match, set OC1A/OC1B at BOTTOM (non-inverting mode)
	//TCCR1B |= (1 << CS10) | (1 << WGM12); // Mode 5 - Fast PWM, 8-bit;  CS10: clkI/O/1 (No prescaling)
	OCR1AH = 0x00;
	OCR1AL = 0x00;  // OC1A (SIDETONE) PWM duty-cycle (span defined by ICR).
	OCR1BH = 0x00;
	OCR1BL = 0x00;  // OC1B (KEY_OUT) PWM duty-cycle (span defined by ICR).
}

void timer1_stop()
{
	OCR1AL = 0x00;
	OCR1BL = 0x00;
}

void timer2_start(uint32_t fs)
{  // Timer 2: interrupt mode
	ASSR &= ~(1 << AS2);  // Timer 2 clocked from CLK I/O (like Timer 0 and 1)
	TCCR2A = 0;
	TCCR2B = 0;
	TCNT2 = 0;
	TCCR2A |= (1 << WGM21); // WGM21: Mode 2 - CTC (Clear Timer on Compare Match)
	TCCR2B |= (1 << CS22);  // Set C22 bits for 64 prescaler
	TIMSK2 |= (1 << OCIE2A);  // enable timer compare interrupt TIMER2_COMPA_vect
	uint8_t ocr = ((F_CPU / 64) / fs) - 1;   // OCRn = (F_CPU / pre-scaler / fs) - 1;
	OCR2A = ocr;
}

void timer2_stop()
{ // Stop Timer 2 interrupt
	TIMSK2 &= ~(1 << OCIE2A);  // disable timer compare interrupt
	delay(1);  // wait until potential in-flight interrupts are finished
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Below a radio-specific implementation based on the above components (seperation of concerns)
//
// Feel free to replace it with your own custom radio implementation :-)

void inline lcd_blanks() { lcd.print(F("        ")); }

#define N_FONTS  8
const byte fonts[N_FONTS][8] PROGMEM = {
{ 0b01000,  // 1; logo
  0b00100,
  0b01010,
  0b00101,
  0b01010,
  0b00100,
  0b01000,
  0b00000 },
{ 0b00000,  // 2; s-meter, 0 bars
  0b00000,
  0b00000,
  0b00000,
  0b00000,
  0b00000,
  0b00000,
  0b00000 },
{ 0b10000,  // 3; s-meter, 1 bars
  0b10000,
  0b10000,
  0b10000,
  0b10000,
  0b10000,
  0b10000,
  0b10000 },
{ 0b10000,  // 4; s-meter, 2 bars
  0b10000,
  0b10100,
  0b10100,
  0b10100,
  0b10100,
  0b10100,
  0b10100 },
{ 0b10000,  // 5; s-meter, 3 bars
  0b10000,
  0b10101,
  0b10101,
  0b10101,
  0b10101,
  0b10101,
  0b10101 },
{ 0b01100,  // 6; vfo-a
  0b10010,
  0b11110,
  0b10010,
  0b10010,
  0b00000,
  0b00000,
  0b00000 },
{ 0b11100,  // 7; vfo-b
  0b10010,
  0b11100,
  0b10010,
  0b11100,
  0b00000,
  0b00000,
  0b00000 },
{ 0b00000,  // 8; TBD
  0b00000,
  0b00000,
  0b00000,
  0b00000,
  0b00000,
  0b00000,
  0b00000 }
};

#ifndef VSS_METER
int analogSafeRead(uint8_t pin, bool ref1v1 = false) {  // performs classical analogRead with default Arduino sample-rate and analog reference setting; restores previous settings
	noInterrupts();
	for (; !(ADCSRA & (1 << ADIF)););  // wait until (a potential previous) ADC conversion is completed
	uint8_t adcsra = ADCSRA;
	uint8_t admux = ADMUX;
	ADCSRA &= ~(1 << ADIE);  // disable interrupts when measurement complete
	ADCSRA |= (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);    // 128 prescaler for 9.6kHz
	if (ref1v1) ADMUX &= ~(1 << REFS0);  // restore reference voltage AREF (1V1)
	else ADMUX = (1 << REFS0);  // restore reference voltage AREF (5V)
	delay(1);  // settle
	int val = analogRead(pin);
	ADCSRA = adcsra;
	ADMUX = admux;
	interrupts();
	return val;
}
#else //VSS_METER
uint16_t analogSafeRead(uint8_t adcpin, bool ref1v1 = false) {
	noInterrupts();
	uint8_t oldmux = ADMUX;
	ADMUX = (3 & 0x0f) | ((ref1v1) ? (1 << REFS1) : 0) | (1 << REFS0);  // set MUX for next conversion   note: hardcoded for BUTTONS adcpin
	for (; !(ADCSRA & (1 << ADIF)););  // wait until (a potential previous) ADC conversion is completed
	delayMicroseconds(16);  // settle
	ADCSRA |= (1 << ADSC);    // start next ADC conversion
	for (; !(ADCSRA & (1 << ADIF)););  // wait until ADC conversion is completed
	ADMUX = oldmux;
	uint16_t adc = ADC;
	interrupts();
	return adc;
}
#endif

uint16_t analogSampleMic()
{
	uint16_t adc;
	noInterrupts();
	ADCSRA = (1 << ADEN) | (((uint8_t)log2((uint8_t)(F_CPU / 13 / (192307 / 1)))) & 0x07);  // hack: faster conversion rate necessary for VOX

	if ((dsp_cap == SDR) && (vox_thresh >= 32)) digitalWrite(RX, LOW);  // disable RF input, only for SDR mod and with low VOX threshold
	//si5351.SendRegister(SI_CLK_OE, TX0RX0);
	uint8_t oldmux = ADMUX;
	for (; !(ADCSRA & (1 << ADIF)););  // wait until (a potential previous) ADC conversion is completed
	ADMUX = admux[2];  // set MUX for next conversion
	ADCSRA |= (1 << ADSC);    // start next ADC conversion
	for (; !(ADCSRA & (1 << ADIF)););  // wait until ADC conversion is completed
	ADMUX = oldmux;
	if ((dsp_cap == SDR) && (vox_thresh >= 32)) digitalWrite(RX, HIGH);  // enable RF input, only for SDR mod and with low VOX threshold
	//si5351.SendRegister(SI_CLK_OE, TX0RX1);
	adc = ADC;
	interrupts();
	return adc;
}

volatile bool change = true;
volatile bool changedMode = 0;
volatile bool changedModeCAT = 0;
volatile int32_t freq = 14000000;
static int32_t vfo[] = { 7074000, 14074000 };
static uint8_t vfomode[] = { LSB, USB };  // G8RDI mod was USB, USB
enum vfo_t { VFOA = 0, VFOB = 1, SPLIT = 2 };
volatile uint8_t vfosel = VFOA;
volatile int32_t rit = 0;	// GW8RDI mod - changed to int32_t from int16_t
#ifdef CAT_XO_CMD
volatile int32_t tit = 0;	// GW8RDI mod - added Transit offset, used with Quantum Spectrum module
#endif
volatile uint8_t semi_qsk = false;

// We measure the average amplitude of the signal (see slow_dsp()) but the S-meter should be based on RMS value.
// So we multiply by 0.707/0.639 in an attempt to roughly compensate, although that only really works if the input
// is a sine wave
uint8_t smode = 1;
uint32_t max_absavg256 = 0;
int16_t dbm;

static int16_t smeter_cnt = 0;

int16_t smeter(int16_t ref = 0)
{
	max_absavg256 = max(_absavg256, max_absavg256); // peak

	if ((smode) && ((++smeter_cnt % 2048) == 0)) {   // slowed down display slightly
		float rms = (float)max_absavg256 * (float)(1 << att2);
		if (dsp_cap == SDR) rms /= (256.0 * 1024.0 * (float)R * 8.0 * 500.0 * 1.414 / (0.707 * 1.1));   // = -98.8dB  1 rx gain stage: rmsV = ADC value * AREF / [ADC DR * processing gain * receiver gain * "RMS compensation"]
		else               rms /= (256.0 * 1024.0 * (float)R * 2.0 * 100.0 * 120.0 / (1.750 * 5.0));   // = -94.6dB
		dbm = 10 * log10((rms * rms) / 50) + 30 - ref; //from rmsV to dBm at 50R

		lcd.noCursor();
		if (smode == 1) { // dBm meter
			lcd.setCursor(9, 0); lcd.print((int16_t)dbm); lcd.print(F("dBm "));
		}
		if (smode == 2) { // S-meter
			uint8_t s = (dbm < -63) ? ((dbm - -127) / 6) : (((uint8_t)(dbm - -73)) / 10) * 10;  // dBm to S (modified to work correctly above S9)
			lcd.setCursor(14, 0); if (s < 10) { lcd.print('S'); } lcd.print(s);
		}
		if (smode == 3) { // S-bar
			int8_t s = (dbm < -63) ? ((dbm - -127) / 6) : (((uint8_t)(dbm - -73)) / 10) * 10;  // dBm to S (modified to work correctly above S9)
			char tmp[5];
			for (uint8_t i = 0; i != 4; i++) { tmp[i] = max(2, min(5, s + 1)); s = s - 3; } tmp[4] = 0;
			lcd.setCursor(12, 0); lcd.print(tmp);
		}
#ifdef CW_DECODER
		if (smode == 4) { // wpm-indicator
			lcd.setCursor(14, 0); if (mode == CW) lcd.print(wpm); lcd.print("  ");
		}
#endif  //CW_DECODER
#ifdef VSS_METER
		if (smode == 5) { // Supply-voltage indicator; add resistor of value R_VSS (see below) between 12V supply input and pin 26 (PC3)   Contribution by Jeff WB4LCG: https://groups.io/g/ucx/message/4470
#define R_VSS   1000 // for 1000kOhm from VSS to PC3 (and 10kOhm to GND). Correct this value until VSS is matching
			uint8_t vss10 = (uint32_t)analogSafeRead(BUTTONS, true) * (R_VSS + 10) * 11 / (10 * 1024);   // use for a 1.1V ADC range VSS measurement
			//uint8_t vss10 = (uint32_t)analogSafeRead(BUTTONS, false) * (R_VSS + 10) * 50 / (10 * 1024);  // use for a 5V ADC range VSS measurement (use for 100k value of R_VSS)
			lcd.setCursor(10, 0); lcd.print(vss10 / 10); lcd.print('.'); lcd.print(vss10 % 10); lcd.print("V ");
		}
#endif //VSS_METER
#ifdef CLOCK
		if (smode == 6) { // clock-indicator
			uint32_t _s = (millis() * 16000000ULL / F_MCU) / 1000;
			uint8_t h = (_s / 3600) % 24;
			uint8_t m = (_s / 60) % 60;
			uint8_t s = (_s) % 60;
			lcd.setCursor(8, 0); lcd.print(h / 10); lcd.print(h % 10); lcd.print(':'); lcd.print(m / 10); lcd.print(m % 10); lcd.print(':'); lcd.print(s / 10); lcd.print(s % 10); lcd.print("  ");
		}
#endif //CLOCK
		stepsize_showcursor();
		max_absavg256 /= 2;  // Implement peak hold/decay for all meter types    
	}
	return dbm;
}

void start_rx() // Start radio receiver
{
	_init = 1;
	rx_state = 0;
	func_ptr = sdr_rx_00;  //enable RX DSP/SDR
	adc_start(2, true, F_ADC_CONV * 4); admux[2] = ADMUX;  // Note that conversion-rate for TX is factors more
	if (dsp_cap == SDR) {
		//#define SWAP_RX_IQ 1    // Swap I/Q ADC inputs, flips RX sideband
#ifdef SWAP_RX_IQ
		adc_start(1, !(att == 1)/*true*/, F_ADC_CONV); admux[0] = ADMUX;
		adc_start(0, !(att == 1)/*true*/, F_ADC_CONV); admux[1] = ADMUX;
#else
		adc_start(0, !(att == 1)/*true*/, F_ADC_CONV); admux[0] = ADMUX;
		adc_start(1, !(att == 1)/*true*/, F_ADC_CONV); admux[1] = ADMUX;
#endif //SWAP_RX_IQ
	}
	else { // ANALOG, DSP
		adc_start(0, false, F_ADC_CONV); admux[0] = ADMUX; admux[1] = ADMUX;
	}
	timer1_start(F_SAMP_PWM);
	timer2_start(F_SAMP_RX);
	TCCR1A &= ~(1 << COM1B1); digitalWrite(KEY_OUT, LOW); // disable KEY_OUT PWM
}

int16_t _centiGain = 0;

uint8_t txdelay = 0;
uint32_t semi_qsk_timeout = 0;

// Set RX or TX mode, with RIT support and CW offset
void switch_rxtx(uint8_t tx_enable)
{
	//GW8RDI - NOTE: Best use the PA line to trigger the Spectrum mode instead of CAT as gives near instant audio control. TX RFI can interfere with RS232 as TX starts to xmit.
#ifdef CAT_TX_CMD
  if (cat_enabled && mode != CW)  // To mute Spectrum DSP audio in CW mode connect the PA line to the Spectrum module PA input.
  {
    if (tx_enable)
    {
      Serial.print("TX0;");   	// GW8RDI mod - advise going to Xmit
	  Serial.print("TX0;");		// Send again as TX can cause RFI ?????? too check needed
    }
    else
    {
        Serial.print("RX0;");   // GW8RDI mod - advise going to Rx
		Serial.print("RX0;");
    }
  }
#endif

  TIMSK2 &= ~(1 << OCIE2A);  // disable timer compare interrupt
	//delay(1);
	delayMicroseconds(20); // wait until potential RX interrupt is finalized
	noInterrupts();
#ifdef TX_DELAY
#ifdef SEMI_QSK
	if (!(semi_qsk_timeout))
#endif
		if ((txdelay) && (tx_enable) && (!(tx)) && (!(practice))) {  // key-up TX relay in advance before actual transmission
			digitalWrite(RX, LOW); // TX (disable RX)
#ifdef NTX
			digitalWrite(NTX, LOW);  // TX (enable TX)
#endif //NTX
#ifdef PTX
			digitalWrite(PTX, HIGH);  // TX (enable TX)
#endif //PTX
			lcd.setCursor(15, 1); lcd.print('D');  // note that this enables interrupts again.
			interrupts();    //hack.. to allow delay()
			delay(F_MCU / 16000000 * txdelay);
			noInterrupts();  //end of hack
		}
#endif //TX_DELAY
	tx = tx_enable;

#ifdef CAT_XO_CMD
	if (rit || tit)
#else
	if (rit)
#endif
		display_vfo(freq);  // GW8RDI mod - update TX freq if RIT or TIT active on transmit, and restore in RX mode

	if (tx_enable)
	{
		// TX
		_centiGain = centiGain;  // backup AGC setting
#ifdef SEMI_QSK
		semi_qsk_timeout = 0;
#endif
		switch (mode)
		{
      case USB:
      case LSB: func_ptr = dsp_tx; break;
		  case CW:  func_ptr = dsp_tx_cw; break;
		  case AM:  func_ptr = dsp_tx_am; break;
		  case FM:  func_ptr = dsp_tx_fm; break;
		}
	}
	else
	{
		// RX
		if ((mode == CW) && (!(semi_qsk_timeout))) {
#ifdef SEMI_QSK
#ifdef KEYER
			semi_qsk_timeout = millis() + ditTime * 8;
#else
			semi_qsk_timeout = millis() + 8 * 8;  // no keyer? assume dit-time of 20 WPM
#endif //KEYER
#endif //SEMI_QSK
			if (semi_qsk) func_ptr = dummy; else func_ptr = sdr_rx_00;
		}
		else {
			centiGain = _centiGain;  // restore AGC setting
#ifdef SEMI_QSK
			semi_qsk_timeout = 0;
#endif
			func_ptr = sdr_rx_00;
		}
	}

	if ((!dsp_cap) && (!tx_enable) && vox)
		func_ptr = dummy; //hack: for SSB mode, disable dsp_rx during vox mode enabled as it slows down the vox loop too much!
	interrupts();
	if (tx_enable) ADMUX = admux[2];
	else _init = 1;
	rx_state = 0;
#ifdef CW_DECODER
	if ((cwdec) && (mode == CW)) { filteredstate = tx_enable; dec2(); }
#endif  //CW_DECODER

	if (tx_enable)
	{
		// TX

		if (practice) {
			digitalWrite(RX, LOW); // TX (disable RX)
			lcd.setCursor(15, 1); lcd.print('P');
			si5351.SendRegister(SI_CLK_OE, TX0RX0);
			// Do not enable PWM (KEY_OUT), do not enble CLK2
		}
		else
		{
			digitalWrite(RX, LOW); // TX (disable RX)

			// GW8RDI Note: Enable TX before setting PLL frequency?????? todo revise
#ifdef NTX
			digitalWrite(NTX, LOW);  // TX (enable TX)
#endif //NTX
#ifdef PTX
			digitalWrite(PTX, HIGH);  // TX (enable TX)
#endif //PTX

			lcd.setCursor(15, 1); lcd.print('T');   // Show Transmitting on LCD

			if (mode == CW)
			{
				si5351.freq_calc_fast(-cw_offset); si5351.SendPLLRegisterBulk();
			} // for CW, TX at freq
#ifdef RIT_ENABLE   // GW8RDI mod - restore freq
			else
			{
#ifdef CAT_XO_CMD
				if (rit || tit)
					si5351.freq_calc_fast(tit); si5351.SendPLLRegisterBulk();	// Restore PLL from RIT offset and add TIT offset
#else
				if (rit)
					si5351.freq_calc_fast(0); si5351.SendPLLRegisterBulk();	// Restore PLL from RIT offset
#endif
			}
#endif //RIT_ENABLE
			si5351.SendRegister(SI_CLK_OE, TX1RX0);
			OCR1AL = 0x80; // make sure SIDETONE is set at 2.5V
			if ((!mox) && (mode != CW)) TCCR1A &= ~(1 << COM1A1); // disable SIDETONE, prevent interference during SSB TX
			TCCR1A |= (1 << COM1B1);  // enable KEY_OUT PWM
#ifdef _SERIAL
			if (cat_active) { DDRC &= ~(1 << 2); } // disable PC2, so that ADC2 can be used as mic input
#endif
		}
	}
	else
	{
		// RX

#ifdef KEY_CLICK
		if (OCR1BL != 0) {
			for (uint16_t i = 0; i != 31; i++) {   // ramp down of amplitude: soft falling edge to prevent key clicks
				OCR1BL = lut[pgm_read_byte_near(ramp[i])];
				delayMicroseconds(60);
			}
		}
#endif //KEY_CLICK
		TCCR1A |= (1 << COM1A1);  // enable SIDETONE (was disabled to prevent interference during ssb tx)
		TCCR1A &= ~(1 << COM1B1); digitalWrite(KEY_OUT, LOW); // disable KEY_OUT PWM, prevents interference during RX
		OCR1BL = 0; // make sure PWM (KEY_OUT) is set to 0%
#ifdef QUAD
		if (quad_enabled)  // G8RDI mod - added - keep disabled else TX voice quality is distorted
		{
#ifdef TX_CLK0_CLK1
			si5351.SendRegister(16, 0x0f);  // disable invert on CLK0
			si5351.SendRegister(17, 0x0f);  // disable invert on CLK1
#else
			si5351.SendRegister(18, 0x0f);  // disable invert on CLK2
#endif  //TX_CLK0_CLK1
		}
#endif //QUAD
		si5351.SendRegister(SI_CLK_OE, TX0RX1);
#ifdef SEMI_QSK
		if ((!semi_qsk_timeout) || (!semi_qsk))   // enable RX when no longer in semi-qsk phase; so RX and NTX/PTX outputs are switching only when in RX mode
#endif //SEMI_QSK
		{
			digitalWrite(RX, !(att == 2)); // RX (enable RX when attenuator not on)
#ifdef NTX
			digitalWrite(NTX, HIGH);  // RX (disable TX)
#endif //NTX
#ifdef PTX
			digitalWrite(PTX, LOW);   // TX (disable TX)
#endif //PTX
		}

#ifdef RIT_ENABLE
		si5351.freq_calc_fast(rit); si5351.SendPLLRegisterBulk();  // restore original PLL RX frequency
#else
		si5351.freq_calc_fast(0); si5351.SendPLLRegisterBulk();  // restore original PLL RX frequency
#endif //RIT_ENABLE
#ifdef SWR_METER
		if (swrmeter > 0) { show_banner(); lcd.print("                "); }
#endif
		lcd.setCursor(15, 1); lcd.print((vox) ? 'V' : 'R');
#ifdef _SERIAL
		if (!vox) if (cat_active) { DDRC |= (1 << 2); } // enable PC2, so that ADC2 is pulled-down so that CAT TX is not disrupted via mic input
#endif
	}
	OCR2A = ((F_CPU / 64) / ((tx_enable) ? F_SAMP_TX : F_SAMP_RX)) - 1;
	TIMSK2 |= (1 << OCIE2A);  // enable timer compare interrupt TIMER2_COMPA_vect
}

uint8_t rx_ph_q = 90;

#ifdef QCX
#define CAL_IQ 1
#ifdef CAL_IQ
int16_t cal_iq_dummy = 0;
// RX I/Q calibration procedure: terminate with 50 ohm, enable CW filter, adjust R27, R24, R17 subsequently to its minimum side-band rejection value in dB
void calibrate_iq()
{
	smode = 1;
	lcd.setCursor(0, 0); lcd_blanks(); lcd_blanks();
	digitalWrite(SIG_OUT, true); // loopback on
	si5351.freq(freq, 0, 90);  // RX in USB  
	si5351.SendRegister(SI_CLK_OE, TX1RX1);
	float dbc;
	si5351.freqb(freq + 700); delay(100);
	dbc = smeter();
	si5351.freqb(freq - 700); delay(100);
	lcd.setCursor(0, 1); lcd.print("I-Q bal. 700Hz"); lcd_blanks();
	for (; !_digitalRead(BUTTONS);) { wdt_reset(); smeter(dbc); } for (; _digitalRead(BUTTONS);) wdt_reset();
	si5351.freqb(freq + 600); delay(100);
	dbc = smeter();
	si5351.freqb(freq - 600); delay(100);
	lcd.setCursor(0, 1); lcd.print("Phase Lo 600Hz"); lcd_blanks();
	for (; !_digitalRead(BUTTONS);) { wdt_reset(); smeter(dbc); } for (; _digitalRead(BUTTONS);) wdt_reset();
	si5351.freqb(freq + 800); delay(100);
	dbc = smeter();
	si5351.freqb(freq - 800); delay(100);
	lcd.setCursor(0, 1); lcd.print("Phase Hi 800Hz"); lcd_blanks();
	for (; !_digitalRead(BUTTONS);) { wdt_reset(); smeter(dbc); } for (; _digitalRead(BUTTONS);) wdt_reset();

	lcd.setCursor(9, 0); lcd_blanks();  // cleanup dbmeter
	digitalWrite(SIG_OUT, false); // loopback off
	si5351.SendRegister(SI_CLK_OE, TX0RX1);
	change = true;  //restore original frequency setting
}
#endif
#endif //QCX

uint8_t prev_bandval = 3;
uint8_t bandval = 3;

#define N_BANDS 11  // See KEEP_BAND_DATA if more than 9 bands required.

#ifdef CW_FREQS_QRP
uint32_t band[N_BANDS] = { /*472000,*/ 1810000, 3560000, 5351500, 7030000, 10106000, 14060000, 18096000, 21060000, 24906000, 28060000, 50096000/*, 70160000, 144060000*/ };  // CW QRP freqs
#else
#ifdef CW_FREQS_FISTS
uint32_t band[N_BANDS] = { /*472000,*/ 1818000, 3558000, 5351500, 7028000, 10118000, 14058000, 18085000, 21058000, 24908000, 28058000, 50058000/*, 70158000, 144058000*/ };  // CW FISTS freqs
#else
uint32_t band[N_BANDS] = { /*472000,*/ 1840000, 3573000, 5357000, 7074000, 10136000, 14074000, 18100000, 21074000, 24915000, 28074000, 50313000/*, 70101000, 144125000*/ };  // FT8 freqs
#endif
#endif

enum step_t { STEP_10M, STEP_1M, STEP_500k, STEP_100k, STEP_10k, STEP_1k, STEP_500, STEP_100, STEP_10, STEP_1 };
uint32_t stepsizes[10] = { 10000000, 1000000, 500000, 100000, 10000, 1000, 500, 100, 10, 1 };
volatile uint8_t stepsize = STEP_1k;
uint8_t prev_stepsize[] = { STEP_1k, STEP_500 }; //default stepsize for resp. SSB, CW


#ifdef KEEP_BAND_DATA  // G8RDI mod - Up to 9 bands are supported of 11. To increase change code.

#define BANDCOUNT N_BANDS-2

static int32_t freq_last[BANDCOUNT];  // 0-8 Last freq used on each band
//static uint8_t mode_last[] = {LSB, LSB, LSB, LSB, USB, USB, USB, USB, USB};  // Last mode used
static uint8_t mode_last[BANDCOUNT];  // Last mode used
#endif

void process_encoder_tuning_step(int8_t steps)
{
	int32_t stepval = stepsizes[stepsize];
	//if(stepsize < STEP_100) freq %= 1000; // when tuned and stepsize > 100Hz then forget fine-tuning details
	if (rit) {
		rit += steps * stepval;
		//rit = max(-9999, min(9999, rit));
		rit = max(-99999, min(99999, rit));	// GW8RDI - mod changed to support +/- 99 KHz RIT receiver offset
	}
	else {
		freq += steps * stepval;
		freq = max(1, min(999999999, freq));
	}
	change = true;
}

void stepsize_showcursor()
{
	lcd.setCursor(stepsize + 1, 1);  // display stepsize with cursor
	lcd.cursor();
}

void stepsize_change(int8_t val)
{
	stepsize += val;
	if (stepsize < STEP_1M) stepsize = STEP_10;
	if (stepsize > STEP_10) stepsize = STEP_1M;
	if (stepsize == STEP_10k || stepsize == STEP_500k) stepsize += val;
	stepsize_showcursor();
}

void powerDown()
{ // Reduces power from 110mA to 70mA (back-light on) or 30mA (back-light off), remaining current is probably opamp quiescent current
	lcd.setCursor(0, 1); lcd.print(F("Power-off 73 :-)")); lcd_blanks();

	MCUSR = ~(1 << WDRF);  // MSY be done before wdt_disable()
	wdt_disable();   // WDTON Fuse High bit need to be 1 (0xD1), if NOT it will override and set WDE=1; WDIE=0, meaning MCU will reset when watchdog timer is zero, and this seems to happen when wdt_disable() is called

	timer2_stop();
	timer1_stop();
	adc_stop();

	si5351.powerDown();

	delay(1500);

	// Disable external interrupts INT0, INT1, Pin Change
	PCICR = 0;
	PCMSK0 = 0;
	PCMSK1 = 0;
	PCMSK2 = 0;
	// Disable internal interrupts
	TIMSK0 = 0;
	TIMSK1 = 0;
	TIMSK2 = 0;
	WDTCSR = 0;
	// Enable BUTTON Pin Change interrupt
	*digitalPinToPCMSK(BUTTONS) |= (1 << digitalPinToPCMSKbit(BUTTONS));
	*digitalPinToPCICR(BUTTONS) |= (1 << digitalPinToPCICRbit(BUTTONS));

	// Power-down sub-systems
	PRR = 0xff;

	lcd.noDisplay();
	///PORTD &= ~0x08; // disable backlight
	PORTD &= ~BACKLIGHT_PIN; // disable backlight

	set_sleep_mode(SLEEP_MODE_PWR_DOWN);
	sleep_enable();
	interrupts();
	sleep_bod_disable();
	//MCUCR |= (1<<BODS) | (1<<BODSE);  // turn bod off by settings BODS, BODSE; note BODS is reset after three clock-cycles, so quickly go to sleep before it is too late
	//MCUCR &= ~(1<<BODSE);  // must be done right before sleep
	sleep_cpu();  // go to sleep mode, wake-up by either INT0, INT1, Pin Change, TWI Addr Match, WDT, BOD
	sleep_disable();

	//void(* reset)(void) = 0; reset();   // soft reset by calling reset vector (does not reset registers to defaults)
	do { wdt_enable(WDTO_15MS); for (;;); } while (0);  // soft reset by trigger watchdog timeout
}

char* szStation = (char*)MY_CALLSIGN_PADDED;  // If callsign is different length, change [5] and [6] below to match 2 spaces at end.

void show_banner() {
	lcd.setCursor(0, 0);
#ifdef QCX
	lcd.print(F("QCX"));
	const char* cap_label[] = { "SSB", "DSP", "SDR" };
	if (ssb_cap || dsp_cap) { lcd.print('-'); lcd.print(cap_label[dsp_cap]); }
#else
	if (quad_enabled)
		szStation[CALLSIGN_LENGTH] = 'q';
	else
		szStation[CALLSIGN_LENGTH] = ' ';
	if (cat_enabled)
		szStation[CALLSIGN_LENGTH + 1] = 'c';
	else
		szStation[CALLSIGN_LENGTH + 1] = ' ';

#ifdef DEBUG_G8RDI  // DEBUG DISPLAY HEX VALUE - disable on release code
	//sprintf(&szStation[CALLSIGN_LENGTH], "%02X", last_state);   // Last rotary encoder state
	sprintf(&szStation[0], "%04X", fir_value);
	//sprintf(&szStation[CALLSIGN_LENGTH], "%02X", filt);
#else
	if (error_code > 0)
		sprintf(&szStation[CALLSIGN_LENGTH], "%02X", error_code);     // G8RDI mod - added
#endif

	lcd.print(szStation);   // "uSDX"
#endif //QCX
	lcd.print('\x01'); lcd_blanks(); lcd_blanks();
}

const char* vfosel_label[] = { "A", "B"/*, "Split"*/ };
///const char* vfosel_label[] = { "A", "B", "Split" };   // GW8RDI note - to add Split to the menu, will need a control adding to show mode, and change receive offset (int16_t rit)
const char* mode_label[5] = { "LSB", "USB", "CW ", "FM ", "AM " };

// Display frequency on LCD.  If RIT is enabled, displays just the receiver offset unless in TX
inline void display_vfo(int32_t f)
{
	lcd.setCursor(0, 1);

	lcd.print((rit) ? ' ' : ((vfosel % 2) | ((vfosel == SPLIT) & tx)) ? '\x07' : '\x06');  // RIT, VFO A/B

#ifdef CAT_XO_CMD
	if (tx && tit != 0)	// GW8RDI mod - TX offset
		f += tit;
#endif

	int32_t scale = 10e6;

	if (rit && !tx)   // GW8RDI mod - In TX mode always show TX freq.
	{
		f = rit;      // Show RIT RX offset only
		scale = 1e3;  // RIT frequency
		//lcd.print(F("RIT ")); lcd.print(rit < 0 ? '-' : '+');
		lcd.print(F("RIT")); lcd.print(rit < 0 ? '-' : '+');		// GW8RDI mod - changinged from RIT +/- 10 KHz, to +/- 50 KHz so need that digit!
	}
	else
	{
		if (f / scale == 0) { lcd.print(' '); scale /= 10; }  // Initial space instead of zero
	}
	for (; scale != 1; f %= scale, scale /= 10) {
		lcd.print(abs(f / scale));
		if (scale == (int32_t)1e3 || scale == (int32_t)1e6) lcd.print(',');  // Thousands separator
	}

	lcd.print(' '); lcd.print(mode_label[mode]); lcd.print(' ');
	lcd.setCursor(15, 1); lcd.print((vox) ? 'V' : 'R');
}

volatile uint8_t event;
//volatile uint8_t menumode = 0;  // 0=not in menu, 1=selects menu item, 2=selects parameter value
volatile uint8_t prev_menumode = 0;
volatile int8_t menu = 0;  // current parameter id selected in menu

#define pgm_cache_item(addr, sz) byte _item[sz]; memcpy_P(_item, addr, sz);  // copy array item from PROGMEM to SRAM
#define get_version_id() ((VERSION[0]-'1') * 2048 + ((VERSION[2]-'0')*10 + (VERSION[3]-'0')) * 32 +  ((VERSION[4]) ? (VERSION[4] - 'a' + 1) : 0) * 1)  // converts VERSION string with (fixed) format "9.99z" into uint16_t (max. values shown here, z may be removed) 

//uint8_t eeprom_version; // G8RDI mod 2022/08/27 was set to unit8_t and should have been 16-bit!  Caused constant resetting when VERSION changed
uint16_t eeprom_version;
#define EEPROM_OFFSET 0x150  // avoid collision with QCX settings, overwrites text settings though
int eeprom_addr;

// Support functions for parameter and menu handling
enum action_t { UPDATE, UPDATE_MENU, NEXT_MENU, LOAD, SAVE, SKIP, NEXT_CH };

// output menuid in x.y format
void printmenuid(uint8_t menuid) {
	static const char seperator[] = { '.', ' ' };
	uint8_t ids[] = { (uint8_t)(menuid >> 4), (uint8_t)(menuid & 0xF) };
	for (int i = 0; i < 2; i++) {
		uint8_t id = ids[i];
		if (id >= 10) {
			id -= 10;
			lcd.print('1');
		}
		lcd.print(char('0' + id));
		lcd.print(seperator[i]);
	}
}

void printlabel(uint8_t action, uint8_t menuid, const __FlashStringHelper * label) {
	if (action == UPDATE_MENU) {
		lcd.setCursor(0, 0);
		printmenuid(menuid);
		lcd.print(label); lcd_blanks(); lcd_blanks();
		lcd.setCursor(0, 1); // value on next line
		if (menumode >= 2) lcd.print('>');
	}
	else { // UPDATE (not in menu)
		lcd.setCursor(0, 1); lcd.print(label); lcd.print(F(": "));
	}
}

void actionCommon(uint8_t action, uint8_t * ptr, uint8_t size) {
	//uint8_t n;
	switch (action) {
	case LOAD:
		//for(n = size; n; --n) *ptr++ = eeprom_read_byte((uint8_t *)eeprom_addr++);
		eeprom_read_block((void*)ptr, (const void*)eeprom_addr, size);
		break;
	case SAVE:
		//noInterrupts();
		//for(n = size; n; --n){ wdt_reset(); eeprom_write_byte((uint8_t *)eeprom_addr++, *ptr++); }
		eeprom_write_block((const void*)ptr, (void*)eeprom_addr, size);
		//interrupts();
		break;
	case SKIP:
		//eeprom_addr += size;
		break;
	}
	eeprom_addr += size;
}

template<typename T> void paramAction(uint8_t action, volatile T & value, uint8_t menuid, const __FlashStringHelper * label, const char* enumArray[], int32_t _min, int32_t _max, bool continuous) {
	switch (action) {
	case UPDATE:
	case UPDATE_MENU:
		if (((int32_t)value + encoder_val) < _min) value = (continuous) ? _max : _min;
		else if (((int32_t)value + encoder_val) > _max) value = (continuous) ? _min : _max;
		else value = (int32_t)value + encoder_val;
		encoder_val = 0;

		lcd.noCursor();
		printlabel(action, menuid, label);  // print normal/menu label
		if (enumArray == NULL) {  // print value
			if ((_min < 0) && (value >= 0)) lcd.print('+');  // add + sign for positive values, in case negative values are supported
			lcd.print(value);
		}
		else {
			lcd.print(enumArray[value]);
		}
		lcd_blanks(); lcd_blanks(); //lcd.setCursor(0, 1);
		//if(action == UPDATE) paramAction(SAVE, value, menuid, label, enumArray, _min, _max, continuous, init_val);
		break;
	default:
		actionCommon(action, (uint8_t*)&value, sizeof(value));
		break;
	}
}

#ifdef MENU_STR
static uint8_t pos = 0;
void paramAction(uint8_t action, char* value, uint8_t menuid, const __FlashStringHelper * label, uint8_t size) {
	const uint8_t _min = ' '; const uint8_t _max = 'Z';
	switch (action) {
	case NEXT_CH:
		if (pos < size) pos++;  // allow to go to next character when string size allows and when current character is not string end
		action = UPDATE_MENU; //fall-through next case
	case UPDATE:
	case UPDATE_MENU:
		if (menumode != 3) pos = 0;
		if (menumode == 2) menumode = 3; // hack: for strings enter in edit mode
		if (((value[pos] + encoder_val) < _min) || ((value[pos] + encoder_val) == 0)) value[pos] = _min;
		else if ((value[pos] + encoder_val) > _max) value[pos] = _max;
		else value[pos] = value[pos] + encoder_val;
		encoder_val = 0;

		printlabel(action, menuid, label);  // print normal/menu label
		for (int i = 0; i != 13; i++) { char ch = value[(pos / 8) * 8 + i]; if (ch) lcd.print(ch); else break; } // print value
		//lcd.print(&value[(pos / 8) * 8]); // print value
		lcd.print('\x01');  // print terminator
		lcd_blanks();
		lcd.setCursor((pos % 8) + (menumode >= 2), 1); lcd.cursor();
		break;
	case SAVE:
		for (uint8_t i = size; i > 0; i--) {
			if ((value[i - 1] == ' ') || (value[i - 1] == 0)) value[i - 1] = 0;  // remove trailing spaces
			else break; // stop once content found
		}
		//fall-through next case
	default:
		actionCommon(action, (uint8_t*)value, size);
		break;
	}
}
#endif //MENU_STR

static uint32_t save_event_time = 0;
static uint8_t vox_tx = 0;
static uint8_t vox_sample = 0;
static uint16_t vox_adc = 0;

static uint8_t pwm_min = 0;    // PWM value for which PA reaches its minimum: 29 when C31 installed;   0 when C31 removed;   0 for biasing BS170 directly
#ifdef QCX
static uint8_t pwm_max = 255;  // PWM value for which PA reaches its maximum: 96 when C31 installed; 255 when C31 removed;
#else
static uint8_t pwm_max = 160;  // PWM value for which PA reaches its maximum:  128 for biasing BS170 directly, 160 for IRFI510G
#endif

const char* offon_label[2] = { "OFF", "ON" };
#if(F_MCU > 16000000)
const char* filt_label[N_FILT + 1] = { "Full", "3000", "2400", "1800", "500", "200", "100", "50" };
#else
const char* filt_label[N_FILT + 1] = { "Full", "2400", "2000", "1500", "500", "200", "100", "50" };
#endif
#ifdef NR_FIR
//const int filt_val[N_FILT + 1] = { ((F_SAMP_RX / 8) / 2) - 1, 3000, 2400, 1800, 500, 200, 100, 50 };  // GW8RDI mod
//const int filt_val[N_FILT + 1] = { 2200, 1800, 1400, 1200, 400, 150, 80, 30 };  // GW8RDI mod
const int filt_val[N_FILT + 1] = { 3000, 2700, 2200, 1800, 400, 150, 80, 30 };  // GW8RDI mod
#endif

const char* band_label[N_BANDS] = { "x", "80m", "60m", "40m", "30m", "20m", "17m", "15m", "12m", "10m", "x" };  // G8RDI mod - squeezing out every free byte!
//const char* band_label[N_BANDS] = { "160m", "80m", "60m", "40m", "30m", "20m", "17m", "15m", "12m", "10m", "6m" };
const char* stepsize_label[] = { "10M", "1M", ".5M", "100k", "10k", "1k", ".5k", "100", "10", "1" };  // GW8RDI 0 b4 0. removed to save memory
const char* att_label[] = { "0dB", "-13dB", "-20dB", "-33dB", "-40dB", "-53dB", "-60dB", "-73dB" };
#ifdef CLOCK
const char* smode_label[] = { "OFF", "dBm", "S", "Sbar", "wpm", "Vss", "time" };
#else
#ifdef VSS_METER
const char* smode_label[] = { "OFF", "dBm", "S", "Sbar", "wpm", "Vss" };
#else
const char* smode_label[] = { "OFF", "dBm", "S", "Sbar", "wpm" };
#endif
#endif
#ifdef SWR_METER
///const char* swr_label[] = { "OFF", "FWD-SWR", "FWD-REF", "VFWD-VREF" };
const char* swr_label[] = { "OFF", "FwdSWR", "FwdRef", "VFwdVREF" };  // GW8RDI mod - byte saving
#endif
const char* cw_tone_label[] = { "700", "600" };
#ifdef KEYER
const char* keyer_mode_label[] = { "IambicA", "IambicB","Straight" };  // GW8RDI mod - byte saving was "Iambic A"
#endif
const char* agc_label[] = { "OFF", "Fast", "Slow" };

#define _N(a) sizeof(a)/sizeof(a[0])

#define N_PARAMS 44+3  // number of (visible) parameters  // G8RDI mod +3 for added visible menu items
#ifdef KEEP_BAND_DATA
#define I_PARAMS 5+9
enum params_t { _NULL, VOLUME, MODE, FILTER, BAND, STEP, VFOSEL, RIT, AGC, NR, ATT, ATT2, SMETER, SWRMETER, CWDEC, CWTONE, CWOFF, SEMIQSK, KEY_WPM, KEY_MODE, KEY_PIN, KEY_TX, TONE_VOL, VOX, VOXGAIN, DRIVE, TXDELAY, MOX, CWINTERVAL, CWMSG1, CWMSG2, CWMSG3, CWMSG4, CWMSG5, CWMSG6, PWM_MIN, PWM_MAX, SIFXTAL, IQ_ADJ, CAT_ACTIVE, QUAD_ACTIVE, CALIB, SR, CPULOAD, PARAM_A, PARAM_B, PARAM_C, BACKL, FREQA, FREQB, MODEA, MODEB, VERS, BAND_DATA0, BAND_DATA1, BAND_DATA2, BAND_DATA3, BAND_DATA4, BAND_DATA5, BAND_DATA6, BAND_DATA7, BAND_DATA8, ALL = 0xff };
#else
#define I_PARAMS 5
enum params_t { _NULL, VOLUME, MODE, FILTER, BAND, STEP, VFOSEL, RIT, AGC, NR, ATT, ATT2, SMETER, SWRMETER, CWDEC, CWTONE, CWOFF, SEMIQSK, KEY_WPM, KEY_MODE, KEY_PIN, KEY_TX, TONE_VOL, VOX, VOXGAIN, DRIVE, TXDELAY, MOX, CWINTERVAL, CWMSG1, CWMSG2, CWMSG3, CWMSG4, CWMSG5, CWMSG6, PWM_MIN, PWM_MAX, SIFXTAL, IQ_ADJ, CAT_ACTIVE, QUAD_ACTIVE, CALIB, SR, CPULOAD, PARAM_A, PARAM_B, PARAM_C, BACKL, FREQA, FREQB, MODEA, MODEB, VERS, ALL = 0xff };
#endif
#define N_ALL_PARAMS (N_PARAMS+I_PARAMS)  // number of parameters

int8_t paramAction(uint8_t action, uint8_t id = ALL)  // list of parameters
{
	if ((action == SAVE) || (action == LOAD)) {
		eeprom_addr = EEPROM_OFFSET;
		for (uint8_t _id = 1; _id < id; _id++) paramAction(SKIP, _id);
	}
	if (id == ALL) for (id = 1; id != N_ALL_PARAMS + 1; id++) paramAction(action, id);  // for all parameters

	switch (id) {    // Visible parameters
	case VOLUME:  paramAction(action, volume, 0x11, F("Vol"), NULL, -1, 16, false); break;  // GW8RDI mod - "Volume"
	case MODE:    paramAction(action, mode, 0x12, F("Mode"), mode_label, 0, _N(mode_label) - 1, false); break;
	case FILTER:  paramAction(action, filt, 0x13, F("FilterBW"), filt_label, 0, _N(filt_label) - 1, false); break;
	case BAND:    paramAction(action, bandval, 0x14, F("Band"), band_label, 1, _N(band_label) - 2, false); break;  // G8RDI mod - changed min to 1 as 160M not in use, and _N(band_label) - 1 to -2 as 6m also
	case STEP:    paramAction(action, stepsize, 0x15, F("Tune Rate"), stepsize_label, 0, _N(stepsize_label) - 1, false); break;
	case VFOSEL:  paramAction(action, vfosel, 0x16, F("VFO Mode"), vfosel_label, 0, _N(vfosel_label) - 1, false); break;
#ifdef RIT_ENABLE
	case RIT:     paramAction(action, rit, 0x17, F("RIT"), offon_label, 0, 1, false); break;
#endif
#ifdef FAST_AGC
	case AGC:     paramAction(action, agc, 0x18, F("AGC"), agc_label, 0, _N(agc_label) - 1, false); break;
#else
	case AGC:     paramAction(action, agc, 0x18, F("AGC"), offon_label, 0, 1, false); break;
#endif // FAST_AGC
#ifdef NR_FIR
	case NR:
		if ((int32_t)nr + encoder_val < 3)
			paramAction(action, nr, 0x19, F("NR"), NULL, 0, 8, false);
		else
			paramAction(action, nr, 0x19, F("DSP NR"), NULL, 0, 8, false);
#else
	case NR:      paramAction(action, nr, 0x19, F("NR"), NULL, 0, 8, false);
#endif
#ifdef NR_FIR
		if (nr > 2)
			FirFilterSetup(7 + (((nr - 2) - 1) * 2), filt_val[filt], F_SAMP_RX / 8);  // GW8RDI mod
#endif
		break;
	case ATT:     paramAction(action, att, 0x1A, F("ATT"), att_label, 0, 7, false); break;
	case ATT2:    paramAction(action, att2, 0x1B, F("ATT2"), NULL, 0, 16, false); break;
	case SMETER:  paramAction(action, smode, 0x1C, F("S-Meter"), smode_label, 0, _N(smode_label) - 1, false); break;
#ifdef SWR_METER
	case SWRMETER:  paramAction(action, swrmeter, 0x1D, F("SWR Meter"), swr_label, 0, _N(swr_label) - 1, false); break;
#endif
#ifdef CW_DECODER
	case CWDEC:   paramAction(action, cwdec, 0x21, F("CW Decoder"), offon_label, 0, 1, false); break;
#endif
#ifdef FILTER_700HZ
	case CWTONE:  if (dsp_cap) paramAction(action, cw_tone, 0x22, F("CW Tone"), cw_tone_label, 0, 1, false); break;
#endif
#ifdef QCX
	case CWOFF:   paramAction(action, cw_offset, 0x23, F("CW Off"), NULL, 300, 2000, false); break; // GW8RDI mod save buyes "CW Offset"
#endif
#ifdef SEMI_QSK
	case SEMIQSK: paramAction(action, semi_qsk, 0x24, F("Semi QSK"), offon_label, 0, 1, false); break;
#endif
#if defined(KEYER) || defined(CW_MESSAGE)
	case KEY_WPM:  paramAction(action, keyer_speed, 0x25, F("Keyer Speed"), NULL, 1, 60, false); break;
#endif
#ifdef KEYER
	case KEY_MODE: paramAction(action, keyer_mode, 0x26, F("Keyer Mode"), keyer_mode_label, 0, 2, false); break;
	case KEY_PIN:  paramAction(action, keyer_swap, 0x27, F("Keyer Swap"), offon_label, 0, 1, false); break;
#endif
	case KEY_TX:   paramAction(action, practice, 0x28, F("Practice"), offon_label, 0, 1, false); break;
#ifdef CW_VOLUME
	case TONE_VOL: paramAction(action, tone_vol, 0x29, F("Tone Vol"), NULL, 0, 16, false); break;
#endif
#ifdef VOX_ENABLE
	case VOX:     paramAction(action, vox, 0x31, F("VOX"), offon_label, 0, 1, false); break;
	case VOXGAIN: paramAction(action, vox_thresh, 0x32, F("Noise Gate"), NULL, 0, 255, false); break; // GW8RDI mod - "Noise Gate" save bytes
#endif
	case DRIVE:   paramAction(action, drive, 0x33, F("TX Drive"), NULL, 0, 8, false); break;
#ifdef TX_DELAY
	case TXDELAY: paramAction(action, txdelay, 0x34, F("TX Delay"), NULL, 0, 255, false); break;
#endif
#ifdef MOX_ENABLE
	case MOX:     paramAction(action, mox, 0x35, F("MOX"), NULL, 0, 2, false); break;
#endif
#ifdef CW_MESSAGE
	case CWINTERVAL: paramAction(action, cw_msg_interval, 0x41, F("CQ Interval"), NULL, 0, 60, false); break;
#ifdef CW_MESSAGE_EXT
	case CWMSG1:    paramAction(action, cw_msg[0], 0x42, F("CQ Msg 1"), sizeof(cw_msg)); break;
	case CWMSG2:    paramAction(action, cw_msg[1], 0x43, F("CW Msg 2"), sizeof(cw_msg)); break;
	case CWMSG3:    paramAction(action, cw_msg[2], 0x44, F("CW Msg 3"), sizeof(cw_msg)); break;
	case CWMSG4:    paramAction(action, cw_msg[3], 0x45, F("CW Msg 4"), sizeof(cw_msg)); break;
	case CWMSG5:    paramAction(action, cw_msg[4], 0x46, F("CW Msg 5"), sizeof(cw_msg)); break;
	case CWMSG6:    paramAction(action, cw_msg[5], 0x47, F("CW Msg 6"), sizeof(cw_msg)); break;
#else
	case CWMSG1:    paramAction(action, cw_msg[0], 0x42, F("CQ Msg"), sizeof(cw_msg)); break;
#endif
#endif
	case PWM_MIN: paramAction(action, pwm_min, 0x81, F("PA bias min"), NULL, 0, pwm_max - 1, false); break;
	case PWM_MAX: paramAction(action, pwm_max, 0x82, F("PA max"), NULL, pwm_min, 255, false); break;
  ///case PWM_MAX: paramAction(action, pwm_max, 0x82, F("PA Bias max"), NULL, pwm_min, 255, false); break;
	case SIFXTAL: paramAction(action, si5351.fxtal, 0x83, F("Ref frq"), NULL, 14000000, 28000000, false); break;
	case IQ_ADJ:  paramAction(action, rx_ph_q, 0x84, F("IQ phase"), NULL, 0, 180, false); break;
#ifdef CAL_IQ
	case CALIB:   if (dsp_cap != SDR) paramAction(action, cal_iq_dummy, 0x85, F("IQ Test/Cal."), NULL, 0, 0, false); break;
#endif
#ifdef CAT
#if defined(CAT_FAST) || defined(CAT_STREAMING)
	case CAT_ACTIVE: paramAction(action, cat_enabled, 0x86, F("CAT115K"), offon_label, 0, 1, false);       // CAT115K2-81N
  if (cat_enabled)  // G8RDI mod 230401
	{
		Serial.begin(16000000ULL * 115200 / F_MCU); // corrected for F_CPU=20M
		Command_IF();
#if !defined(OLED) && defined(TESTBENCH)
		smode = 0;  // In case of LCD, turn off smeter
#endif
	}
  break;
#else
	case CAT_ACTIVE: paramAction(action, cat_enabled, 0x86, F("CAT38K"), offon_label, 0, 1, false);   // CAT38K4-81N
  if (cat_enabled)  // G8RDI mod 230401
	{
		Serial.begin(16000000ULL * 38400 / F_MCU); // corrected for F_CPU=20M
		Command_IF();
#if !defined(OLED) && defined(TESTBENCH)
		smode = 0;  // In case of LCD, turn off smeter
#endif
	}
  break;
#endif
#endif
#ifdef QUAD
	case QUAD_ACTIVE: paramAction(action, quad_enabled, 0x87, F("QUAD"), offon_label, 0, 1, false); break;
#endif
#ifdef DEBUG
	case SR:      paramAction(action, sr, 0x91, F("Sample rate"), NULL, INT32_MIN, INT32_MAX, false); break;
	case CPULOAD: paramAction(action, cpu_load, 0x92, F("CPU load %"), NULL, INT32_MIN, INT32_MAX, false); break;
	case PARAM_A: paramAction(action, param_a, 0x93, F("ParamA"), NULL, 0, UINT16_MAX, false); break;
	case PARAM_B: paramAction(action, param_b, 0x94, F("ParamB"), NULL, INT16_MIN, INT16_MAX, false); break;
	case PARAM_C: paramAction(action, param_c, 0x95, F("ParamC"), NULL, INT16_MIN, INT16_MAX, false); break;
#endif
	case BACKL:   paramAction(action, backlight, 0xA1, F("Light"), offon_label, 0, 1, false); break;   // GW8RDI "Backlight" workaround for varying N_PARAM and not being able to overflowing default cases properly
		// Invisible parameters
	case FREQA:   paramAction(action, vfo[VFOA], 0, NULL, NULL, 0, 0, false); break;
	case FREQB:   paramAction(action, vfo[VFOB], 0, NULL, NULL, 0, 0, false); break;
	case MODEA:   paramAction(action, vfomode[VFOA], 0, NULL, NULL, 0, 0, false); break;
	case MODEB:   paramAction(action, vfomode[VFOB], 0, NULL, NULL, 0, 0, false); break;
	case VERS:    paramAction(action, eeprom_version, 0, NULL, NULL, 0, 0, false); break;
/* #ifdef KEEP_BAND_DATA
	case BAND_DATA0:      // G8RDI mod - added
		paramAction(action, freq_last[0], 0, NULL, NULL, 0, 0, false);
		paramAction(action, mode_last[0], 0, NULL, NULL, 0, 0, false); break;
	case BAND_DATA1:      // G8RDI mod - added
		paramAction(action, freq_last[1], 0, NULL, NULL, 0, 0, false);
		paramAction(action, mode_last[1], 0, NULL, NULL, 0, 0, false); break;
	case BAND_DATA2:      // G8RDI mod - added
		paramAction(action, freq_last[2], 0, NULL, NULL, 0, 0, false);
		paramAction(action, mode_last[2], 0, NULL, NULL, 0, 0, false); break;
	case BAND_DATA3:      // G8RDI mod - added
		paramAction(action, freq_last[3], 0, NULL, NULL, 0, 0, false);
		paramAction(action, mode_last[3], 0, NULL, NULL, 0, 0, false); break;
	case BAND_DATA4:      // G8RDI mod - added
		paramAction(action, freq_last[4], 0, NULL, NULL, 0, 0, false);
		paramAction(action, mode_last[4], 0, NULL, NULL, 0, 0, false); break;
	case BAND_DATA5:      // G8RDI mod - added
		paramAction(action, freq_last[5], 0, NULL, NULL, 0, 0, false);
		paramAction(action, mode_last[5], 0, NULL, NULL, 0, 0, false); break;
	case BAND_DATA6:      // G8RDI mod - added
		paramAction(action, freq_last[6], 0, NULL, NULL, 0, 0, false);
		paramAction(action, mode_last[6], 0, NULL, NULL, 0, 0, false); break;
	case BAND_DATA7:      // G8RDI mod - added
		paramAction(action, freq_last[7], 0, NULL, NULL, 0, 0, false);
		paramAction(action, mode_last[7], 0, NULL, NULL, 0, 0, false); break;
	case BAND_DATA8:      // G8RDI mod - added
		paramAction(action, freq_last[8], 0, NULL, NULL, 0, 0, false);
		paramAction(action, mode_last[8], 0, NULL, NULL, 0, 0, false); break;
#endif */
		// Non-parameters
	case _NULL:   menumode = 0; show_banner(); change = true; break;

	default:
  #ifdef KEEP_BAND_DATA // 230401 freed 258 bytes!!
  if (id >= BAND_DATA0 && id <= BAND_DATA8)
  {
    uint8_t is = id - BAND_DATA0;
    paramAction(action, freq_last[is], 0, NULL, NULL, 0, 0, false);
		paramAction(action, mode_last[is], 0, NULL, NULL, 0, 0, false); break;
  }
  #endif

  if ((action == NEXT_MENU) && (id != N_PARAMS))
    id = paramAction(action, max(1 /*0*/, min(N_PARAMS, id + ((encoder_val > 0) ? 1 : -1))));
  break;  // keep iterating util menu item found
	}
	return id;
}

void initPins() {
	// initialize
	digitalWrite(SIG_OUT, LOW);
	digitalWrite(RX, HIGH);
	digitalWrite(KEY_OUT, LOW);
	digitalWrite(SIDETONE, LOW);

	// pins
	pinMode(SIDETONE, OUTPUT);
	pinMode(SIG_OUT, OUTPUT);
	pinMode(RX, OUTPUT);
	pinMode(KEY_OUT, OUTPUT);
#ifdef ONEBUTTON
	pinMode(BUTTONS, INPUT_PULLUP);  // rotary button
#else
	pinMode(BUTTONS, INPUT);  // L/R/rotary button
#endif
	pinMode(DIT, INPUT_PULLUP);
	pinMode(DAH, INPUT);  // pull-up DAH 10k via AVCC
	//pinMode(DAH, INPUT_PULLUP); // Could this replace D4? But leaks noisy VCC into mic input!

	digitalWrite(AUDIO1, LOW);  // when used as output, help can mute RX leakage into AREF
	digitalWrite(AUDIO2, LOW);
	pinMode(AUDIO1, INPUT);
	pinMode(AUDIO2, INPUT);

#ifdef NTX
	digitalWrite(NTX, HIGH);
	pinMode(NTX, OUTPUT);
#endif //NTX
#ifdef PTX
	digitalWrite(PTX, LOW);
	pinMode(PTX, OUTPUT);
#endif //PTX
#ifdef SWR_METER
	pinMode(PIN_FWD, INPUT);
	pinMode(PIN_REF, INPUT);
#endif
#ifdef OLED  // assign unused LCD pins
	pinMode(PD4, OUTPUT);
	pinMode(PD5, OUTPUT);
#else
#if defined(RED_CORNERS) || defined(BLACK_BRICK)
	pinMode(PD5, OUTPUT);    // G8RDI mod as drives LCD 1602 backlight
#else
	pinMode(PD3, OUTPUT);    // G8RDI mod - uSDX+ have backlight control
#endif
#endif
}

#ifdef CAT
// CAT support inspired by Charlie Morris, ZL2CTM, contribution by Alex, PE1EVX, source: http://zl2ctm.blogspot.com/2020/06/digital-modes-transceiver.html?m=1
// https://www.kenwood.com/i/products/info/amateur/ts_480/pdf/ts_480_pc.pdf
#define CATCMD_SIZE   32
char CATcmd[CATCMD_SIZE];

#ifdef CAT_STREAMING  // G8RDI mod - moved before so in scope
void Command_UA(char en)
{
	char Catbuffer[16];
	sprintf(Catbuffer, "UA%01u;", (_cat_streaming = (en == '1')));
	Serial.print(Catbuffer);
	if (_cat_streaming) { Serial.print("US"); cat_streaming = true; }
}
#endif

void analyseCATcmd()    // Supported Kenwood TS-480 protocol CAT commands
{
	if ((CATcmd[0] == 'F') && (CATcmd[1] == 'A') && (CATcmd[2] == ';'))
		Command_GETFreqA();

	else if ((CATcmd[0] == 'F') && (CATcmd[1] == 'A') && (CATcmd[13] == ';'))
		Command_SETFreqA();

	else if ((CATcmd[0] == 'I') && (CATcmd[1] == 'F') && (CATcmd[2] == ';'))
		Command_IF();

	else if ((CATcmd[0] == 'I') && (CATcmd[1] == 'D') && (CATcmd[2] == ';'))
		Command_ID();

	else if ((CATcmd[0] == 'P') && (CATcmd[1] == 'S') && (CATcmd[2] == ';'))
		Command_PS();

	else if ((CATcmd[0] == 'P') && (CATcmd[1] == 'S') && (CATcmd[2] == '1'))
		Command_PS1();

	else if ((CATcmd[0] == 'A') && (CATcmd[1] == 'I') && (CATcmd[2] == ';'))
		Command_AI();

	else if ((CATcmd[0] == 'A') && (CATcmd[1] == 'I') && (CATcmd[2] == '0'))
		Command_AI0();

	else if ((CATcmd[0] == 'M') && (CATcmd[1] == 'D') && (CATcmd[2] == ';'))
		Command_GetMD();

	else if ((CATcmd[0] == 'M') && (CATcmd[1] == 'D') && (CATcmd[3] == ';'))
		Command_SetMD();

	else if ((CATcmd[0] == 'R') && (CATcmd[1] == 'X') && (CATcmd[2] == ';'))
		Command_RX();

	else if ((CATcmd[0] == 'T') && (CATcmd[1] == 'X') && (CATcmd[2] == ';'))
		Command_TX0();

	else if ((CATcmd[0] == 'T') && (CATcmd[1] == 'X') && (CATcmd[2] == '0'))
		Command_TX0();

	else if ((CATcmd[0] == 'T') && (CATcmd[1] == 'X') && (CATcmd[2] == '1'))
		Command_TX1();

	else if ((CATcmd[0] == 'T') && (CATcmd[1] == 'X') && (CATcmd[2] == '2'))
		Command_TX2();

	else if ((CATcmd[0] == 'A') && (CATcmd[1] == 'G') && (CATcmd[2] == '0'))  // add
		Command_AG0();

	else if ((CATcmd[0] == 'X') && (CATcmd[1] == 'T') && (CATcmd[2] == '1'))  // add
		Command_XT1();

	else if ((CATcmd[0] == 'R') && (CATcmd[1] == 'T') && (CATcmd[2] == '1'))  // add
		Command_RT1();

#ifdef RIT_ENABLE
	else if ((CATcmd[0] == 'R') && (CATcmd[1] == 'T') && (CATcmd[2] == 'S') && (CATcmd[8] == ','))  // GW8RDI mod - set RIT receiver offset
		Command_RTS();	// “RTSxxxxx; ” Sets RIT frequency
#endif

#ifdef CAT_XO_CMD
	else if ((CATcmd[0] == 'X') && (CATcmd[1] == 'O') && (CATcmd[14] == ';'))  // GW8RDI mod - Added/invented : set TIT transmitter offset "XO000000012000;"
		Command_XO();	// -12KHz == "XO100000012000;" Sets TX offset frequency
#endif

	else if ((CATcmd[0] == 'R') && (CATcmd[1] == 'C') && (CATcmd[2] == ';'))  // add
		Command_RC();

	else if ((CATcmd[0] == 'F') && (CATcmd[1] == 'L') && (CATcmd[2] == '0'))  // need?
		Command_FL0();

	else if ((CATcmd[0] == 'R') && (CATcmd[1] == 'S') && (CATcmd[2] == ';'))
		Command_RS();

	else if ((CATcmd[0] == 'V') && (CATcmd[1] == 'X') && (CATcmd[2] != ';'))
		Command_VX(CATcmd[2]);

	else if ((CATcmd[0] == 'F') && (CATcmd[1] == 'W') && (CATcmd[2] == ';'))
		Command_FW();

	/*
	The following CAT extensions are available to support remote operatons. Use a baudrate of 115200 when enabling CAT_STREAMING config switch:

	UA1;  enable audio streaming; an 8-bit audio stream nnn at 4812 samples/s is then sent within CAT response USnnn; here nnn is of undefined length, it will end as soon other CAT requests are processed and resume with a new USnnn; response
	UA0;  disable audio streaming
	UD;   requests display contents
	UKnn; control keys, where nn is a sum of the following hexadecimal values: 0x80 encoder left, 0x40 encoder right, 0x20 DIT/PTT, 0x10 DAH, 0x04 left-button, 0x02 encoder button, 0x01 right button


	The following Linux script may be useful to stream audio over CAT, play the audio and redirect CAT to /tmp/ttyS0:

	1. Pre-requisite installation: sudo apt-get install socat

	2. Insert USB serial converter (/dev/ttyUSB0) and start:

	stty -F /dev/ttyUSB0 raw -echo -echoe -echoctl -echoke 115200;

	socat -d -d pty,link=/tmp/ttyS0,echo=0,ignoreeof,b115200,raw,perm=0777 pty,link=/tmp/ttyS1,echo=0,ignoreeof,b115200,raw,perm=0777 &

	sleep 5; cat /tmp/ttyS1 > /dev/ttyUSB0 &

	cat /dev/ttyUSB0 | while IFS= read -n1 c; do
	  case $state in
	  0) if [ "$c" = ";" ]; then state=1; fi; printf "%s" "$c">>/tmp/ttyS1;
	   ;;
	  1) if [ "$c" = "U" ]; then state=2; else printf "%s" "$c">>/tmp/ttyS1; state=0; fi;
	   ;;
	  2) if [ "$c" = "S" ]; then state=3; else printf "U%s" "$c">>/tmp/ttyS1; state=0; fi;
	   ;;
	  3) if [ "$c" = ";" ]; then state=1; else printf "%s" "$c"; fi;
	   ;;
	  *) state=3;
	   ;;
	  esac
	done | pacat --rate=7812 --channels=1 --format=u8 --latency-msec=30 --process-time-msec=30 &

	echo ";UA1;" > /tmp/ttyS0;

	3. You should now hear audio, use pavucontrol. Now start your favorite digimode/logbook application, and use /tmp/ttyS0 as CAT port.

	4. Instead of step 3, you could visualize uSDX console:

	clear; echo ";UA1;UD;" > /tmp/ttyS0; cat /tmp/ttyS0 | while IFS= read -d \; c; do echo "${c}" |sed -E  's/^UD..(.+)$|^[^U][^D](.*)$/\1/g'| sed -E 's/^(.{16})(.+)$/\x1B[;1H\x1B[1m\x1B[44m\x1B[97m\1\x1B[0m\x1B[K\n\x1B[1m\x1B[44m\x1B[97m\2\x1B[0m\x1B[K\n\x1B[K\n\x1B[K/g'; echo ";UD;UD;" >> /tmp/ttyS0; sleep 1; done

	Use pavumeter to set the correct mixer settings

	For an Arduino board at 16 MHz, the following simple script will start streaming audio:

	cat /dev/ttyACM0 | aplay -c 1 -r 6249 -f U8
	stty -F /dev/ttyACM0 raw -echo -echoe -echoctl -echoke 115200;
	echo ";UA1;" > /dev/ttyACM0

	*/
#ifdef CAT_EXT
	else if ((CATcmd[0] == 'U') && (CATcmd[1] == 'K') && (CATcmd[4] == ';'))  // remote key press
		Command_UK(CATcmd[2], CATcmd[3]);

	else if ((CATcmd[0] == 'U') && (CATcmd[1] == 'D') && (CATcmd[2] == ';'))  // display contents
		Command_UD();
#endif //CAT_EXT

#ifdef CAT_STREAMING
	else if ((CATcmd[0] == 'U') && (CATcmd[1] == 'A') && (CATcmd[3] == ';'))  // audio streaming enable/disable
		Command_UA(CATcmd[2]);
#endif //CAT_STREAMING

	else {
		Serial.print("?;");
#ifdef DEBUG
		{ lcd.setCursor(0, 0); lcd.print(CATcmd); lcd_blanks(); }  // Print error cmd
#else
		//{ lcd.setCursor(15, 1); lcd.print('E'); }
#endif
	}
}

#ifdef CAT
volatile uint8_t cat_ptr = 0;
void serialEvent() {
	if (Serial.available()) {
		rxend_event = millis() + 10;  // block display until this moment, to prevent CAT cmds that initiate display changes to interfere with the next CAT cmd e.g. Hamlib: FA00007071000;ID;
		char data = Serial.read();
		CATcmd[cat_ptr++] = data;
		if (data == ';') {
			CATcmd[cat_ptr] = '\0'; // terminate the array
			cat_ptr = 0;            // reset for next CAT command
#ifdef _SERIAL
			if (!cat_active) { cat_active = 1; smode = 0; } // disable smeter to reduce display activity
#endif
#ifdef CAT_STREAMING
			if (cat_streaming) { noInterrupts(); cat_streaming = false; Serial.print(';'); }   // terminate CAT stream
			analyseCATcmd();   // process CAT cmd
			if (_cat_streaming) { Serial.print("US"); cat_streaming = true; }  // resume CAT stream
			interrupts();
#else
			analyseCATcmd();
#endif // CAT_STREAMING
			delay(10);
		}
		else if (cat_ptr > (CATCMD_SIZE - 1)) { Serial.print("E;"); cat_ptr = 0; } // overrun
	}
}
#endif //CAT

#ifdef CAT_EXT
void Command_UK(char k1, char k2)
{
	cat_key = ((k1 - '0') << 4) | (k2 - '0');
	if (cat_key & 0x40) { encoder_val--; cat_key &= 0x3f; }
	if (cat_key & 0x80) { encoder_val++; cat_key &= 0x3f; }
	char Catbuffer[16];
	sprintf(Catbuffer, "UK%c%c;", k1, k2);
	Serial.print(Catbuffer);
}

void Command_UD()
{
	char Catbuffer[40];
	sprintf(Catbuffer, "UD%02u%s;", (lcd.curs) ? lcd.y * 16 + lcd.x : 16 * 2 + 1, lcd.text);
	Serial.print(Catbuffer);
}

#endif // CAT_EXT

void Command_GETFreqA()
{
#ifdef _SERIAL
	if (!cat_active) return;
#endif
	char Catbuffer[32];
	unsigned int g, m, k, h;
	uint32_t tf;

	tf = freq;
	g = (unsigned int)(tf / 1000000000lu);
	tf -= g * 1000000000lu;
	m = (unsigned int)(tf / 1000000lu);
	tf -= m * 1000000lu;
	k = (unsigned int)(tf / 1000lu);
	tf -= k * 1000lu;
	h = (unsigned int)tf;

	sprintf(Catbuffer, "FA%02u%03u", g, m);
	Serial.print(Catbuffer);
	sprintf(Catbuffer, "%03u%03u;", k, h);
	Serial.print(Catbuffer);
}

void Command_SETFreqA()
{
	/*	char Catbuffer[16];   // GW8RDI mod - unnecessary to double buffer
		strncpy(Catbuffer, CATcmd + 2, 11);
		Catbuffer[11] = '\0';
	*/
	//uint32_t fq = (uint32_t)atol(Catbuffer);
	uint32_t fq = (uint32_t)atol(CATcmd + 2);  // "FA00024000000;" GW8RDI mod - CAT freq error check
	if (fq >= 1500000 && fq <= 60000000)   // Ignore corrupted freq data
	{
		freq = fq;
		change = true;
	}
}

void Command_IF()
{
#ifdef _SERIAL
	if (!cat_active) return;
#endif
	char Catbuffer[32];
	unsigned int g, m, k, h;
	uint32_t tf;

	tf = freq;
	g = (unsigned int)(tf / 1000000000lu);
	tf -= g * 1000000000lu;
	m = (unsigned int)(tf / 1000000lu);
	tf -= m * 1000000lu;
	k = (unsigned int)(tf / 1000lu);
	tf -= k * 1000lu;
	h = (unsigned int)tf;

	sprintf(Catbuffer, "IF%02u%03u%03u%03u", g, m, k, h);
	Serial.print(Catbuffer);
	sprintf(Catbuffer, "00000+000000");
	Serial.print(Catbuffer);
	sprintf(Catbuffer, "0000");
	Serial.print(Catbuffer);
	Serial.print(mode + 1);
	sprintf(Catbuffer, "0000000;");
	Serial.print(Catbuffer);
}

void Command_AI()
{
	Serial.print("AI0;");
}

void Command_AG0()
{
	Serial.print("AG0;");
}

void Command_XT1()
{
	Serial.print("XT1;");
}

void Command_RT1()
{
	Serial.print("RT1;");
}

#ifdef CAT_XO_CMD
void Command_XO()		// GW8RDI mod - added set TX offset, i.e. "XO000000012000;"
{
	int32_t fq = atol(CATcmd + 3);  // GW8RDI mod - CAT freq error check
	if (CATcmd[2] == 1)
		fq = -fq;
	//no space...//if (fq >= -1000000 && fq <= 1000000)   // Ignore corrupted freq data. Limit to +/-1 MHz to better screen for errors
	{
		tit = fq;
		//change = true;
	}
}
#endif

#ifdef RIT_ENABLE
void Command_RTS()		// GW8RDI mod - added set RIT offset, i.e. "RTS30000;"
{
	int32_t fq = atol(CATcmd + 3);  // GW8RDI mod - CAT freq error check
	if (fq >= -99999 && fq <= 99999)   // Ignore corrupted freq data
	{
		rit = fq;
		change = true;
	}
}
#endif

void Command_RC()
{
	rit = 0;
	Serial.print("RC;");
}

void Command_FL0()
{
	Serial.print("FL0;");
}

void Command_GetMD()
{
	Serial.print("MD");
	Serial.print(mode + 1);
	Serial.print(';');
}

void Command_SetMD()
{
  prev_mode = mode;
	mode = CATcmd[2] - '1';
  changedModeCAT = true;
	/*vfomode[vfosel % 2] = mode;
  si5351.iqmsa = 0;  // enforce PLL reset
	change = true; */
}

void Command_AI0()
{
	Serial.print("AI0;");
}

void Command_RX()
{
#ifdef TX_ENABLE
	switch_rxtx(0);
	semi_qsk_timeout = 0;  // hack: fix for multiple RX cmds
#endif
	Serial.print("RX0;");
}

void Command_TX0()
{
#ifdef TX_ENABLE
	switch_rxtx(1);
#endif
}

void Command_TX1()
{
#ifdef TX_ENABLE
	switch_rxtx(1);
#endif
}

void Command_TX2()
{
#ifdef TX_ENABLE
	switch_rxtx(1);
#endif
}

void Command_RS()
{
	Serial.print("RS0;");
}

void Command_VX(char mode)
{
	char Catbuffer[16];
	sprintf(Catbuffer, "VX%c;", mode);
	Serial.print(Catbuffer);
}

void Command_ID()
{
	Serial.print("ID020;");
}

void Command_PS()
{
	Serial.print("PS1;");
}

void Command_PS1()
{
}

void Command_FW()
{
#ifdef _SERIAL
	if (!cat_active) return;
#endif
	char Catbuffer[32];

	const int filt_val[N_FILT + 1] = { 3600, 3000, 2400, 1800, 500, 200, 100, 50 };

	sprintf(Catbuffer, "FW%04u;", filt_val[filt]);
	Serial.print(Catbuffer);
}
#endif //CAT

void fatal(const __FlashStringHelper * msg, int value = 0, char unit = '\0') {
	lcd.setCursor(0, 1);
	lcd.print('!'); lcd.print('!');
	lcd.print(msg);
	if (unit != '\0') {
		lcd.print('=');
		lcd.print(value);
		lcd.print(unit);
	}
	lcd_blanks();
	delay(1500);
	wdt_reset();
}

//refresh LUT based on pwm_min, pwm_max
void build_lut()
{
	for (uint16_t i = 0; i != 256; i++)    // refresh LUT based on pwm_min, pwm_max
		lut[i] = (i * (pwm_max - pwm_min)) / 255 + pwm_min;
	//lut[i] = min(pwm_max, (float)106*log(i) + pwm_min);  // compressed microphone output: drive=0, pwm_min=115, pwm_max=220
}

#ifdef SWR_METER
void readSWR()
// reads FWD / REF values from A6 and A7 and computes SWR
// credit Duwayne, KV4QB
/* This should similar as PE1DDA's (more direct) approach:
  busvoltage = ina219.getBusVoltage_V();
  current_mA = ina219.getCurrent_mA();
  power_mW = ina219.getPower_mW();
  Vinc = analogRead(3);
  Vref = analogRead(2);
  SWR = (Vinc + Vref) / (Vinc - Vref);
  Vinc = ((Vinc * 5.0) / 1024.0) + 0.5;
  pwr = ((((Vinc) * (Vinc)) - 0.25 ) * k);
  Eff = (pwr) / ((power_mW) / 1000) * 100; */
{
	float v_FWD = 0;
	float v_REF = 0;
	for (int i = 0; i <= 7; i++) {
		v_FWD = v_FWD + (ref_V / 1023) * (int)analogRead(PIN_FWD);
		v_REF = v_REF + (ref_V / 1023) * (int)analogRead(PIN_REF);
		delay(5);
	}
	v_FWD = v_FWD / 8;
	v_REF = v_REF / 8;

	float p_FWD = sq(v_FWD);
	float p_REV = sq(v_REF);

	float vRatio = v_REF / v_FWD;
	float VSWR = (1 + vRatio) / (1 - vRatio);

	if ((VSWR > 9.99) || (VSWR < 1))VSWR = 9.99;

	if (p_FWD != FWD || VSWR != SWR) {
		lcd.noCursor();
		lcd.setCursor(0, 0);
		switch (swrmeter) {
		case 1:
			lcd.print(" "); lcd.print(floor(100 * p_FWD) / 100); lcd.print("W  SWR:"); lcd.print(floor(100 * VSWR) / 100);
			break;
		case 2:
			lcd.print(" F:"); lcd.print(floor(100 * p_FWD) / 100); lcd.print("W R:"); lcd.print(floor(100 * p_REV) / 100); lcd.print("W");
			break;
		case 3:
			lcd.print(" F:"); lcd.print(floor(100 * v_FWD) / 100); lcd.print("V R:"); lcd.print(floor(100 * v_REF) / 100); lcd.print("V");
			break;
		}
		FWD = p_FWD;
		SWR = VSWR;
	}
}
#endif
void setup()
{
	digitalWrite(KEY_OUT, LOW);  // for safety: to prevent exploding PA MOSFETs, in case there was something still biasing them.
	si5351.powerDown();  // disable all CLK outputs (especially needed for si5351 variants that has CLK2 enabled by default, such as Si5351A-B04486-GT)

	//uint8_t mcusr = MCUSR;
	MCUSR = 0;
	//wdt_disable();
	wdt_enable(WDTO_4S);  // Enable watchdog
	uint32_t t0, t1;
#ifdef DEBUG
	// Benchmark dsp_tx() ISR (this needs to be done in beginning of setup() otherwise when VERSION containts 5 chars, mis-alignment impact performance by a few percent)
	func_ptr = dsp_tx;
	t0 = micros();
	TIMER2_COMPA_vect();
	//func_ptr();
	t1 = micros();
	uint16_t load_tx = (float)(t1 - t0) * (float)F_SAMP_TX * 100.0 / 1000000.0 * 16000000.0 / (float)F_CPU;
	// benchmark sdr_rx_00() ISR
	func_ptr = sdr_rx_00;
	rx_state = 0;
	uint16_t load_rx[8];
	uint16_t load_rx_avg = 0;
	uint16_t i;
	for (i = 0; i != 8; i++) {
		rx_state = i;
		t0 = micros();
		TIMER2_COMPA_vect();
		//func_ptr();
		t1 = micros();
		load_rx[i] = (float)(t1 - t0) * (float)F_SAMP_RX * 100.0 / 1000000.0 * 16000000.0 / (float)F_CPU;
		load_rx_avg += load_rx[i];
	}
	load_rx_avg /= 8;

	//adc_stop();  // recover general ADC settings so that analogRead is working again
#endif //DEBUG
	ADMUX = (1 << REFS0);  // restore reference voltage AREF (5V)

	// disable external interrupts
	PCICR = 0;
	PCMSK0 = 0;
	PCMSK1 = 0;
	PCMSK2 = 0;

	encoder_setup();

	initPins();

	delay(100);           // at least 40ms after power rises above 2.7V before sending commands
	lcd.begin(16, 4);     // Init LCD
#ifndef OLED
	for (i = 0; i != N_FONTS; i++) {  // Init fonts
		pgm_cache_item(fonts[i], 8);
		lcd.createChar(0x01 + i, /*fonts[i]*/_item);
	}
#endif

	show_banner();
	lcd.setCursor(7, 0); lcd.print(F(" R")); lcd.print(F(VERSION)); lcd_blanks();
	delay(500);	// GW8RDI MOD - SHOW LONGER

#ifdef QCX
	// Test if QCX has DSP/SDR capability: SIDETONE output disconnected from AUDIO2
	si5351.SendRegister(SI_CLK_OE, TX0RX0); // Mute QSD
	digitalWrite(RX, HIGH);  // generate pulse on SIDETONE and test if it can be seen on AUDIO2
	delay(1); // settle
	digitalWrite(SIDETONE, LOW);
	int16_t v1 = analogRead(AUDIO2);
	digitalWrite(SIDETONE, HIGH);
	int16_t v2 = analogRead(AUDIO2);
	digitalWrite(SIDETONE, LOW);
	dsp_cap = !(abs(v2 - v1) > (0.05 * 1024.0 / 5.0));  // DSP capability?
	if (dsp_cap) {  // Test if QCX has SDR capability: AUDIO2 is disconnected from AUDIO1  (only in case of DSP capability)
		delay(400); wdt_reset(); // settle:  the following test only works well 400ms after startup
		v1 = analogRead(AUDIO1);
		digitalWrite(AUDIO2, HIGH);   // generate pulse on AUDIO2 and test if it can be seen on AUDIO1
		pinMode(AUDIO2, OUTPUT);
		delay(1);
		digitalWrite(AUDIO2, LOW);
		delay(1);
		digitalWrite(AUDIO2, HIGH);
		v2 = analogRead(AUDIO1);
		pinMode(AUDIO2, INPUT);
		if (!(abs(v2 - v1) > (0.125 * 1024.0 / 5.0))) dsp_cap = SDR;  // SDR capacility?
	}
	// Test if QCX has SSB capability: DAH is connected to DVM
	delay(1); // settle
	pinMode(DAH, OUTPUT);
	digitalWrite(DAH, LOW);
	v1 = analogRead(DVM);
	digitalWrite(DAH, HIGH);
	v2 = analogRead(DVM);
	digitalWrite(DAH, LOW);
	//pinMode(DAH, INPUT_PULLUP);
	pinMode(DAH, INPUT);
	ssb_cap = (abs(v2 - v1) > (0.05 * 1024.0 / 5.0));  // SSB capability?

	//ssb_cap = 0; dsp_cap = ANALOG;  // force standard QCX capability
	//ssb_cap = 1; dsp_cap = ANALOG;  // force SSB and standard QCX-RX capability
	//ssb_cap = 1; dsp_cap = DSP;     // force SSB and DSP capability
	//ssb_cap = 1; dsp_cap = SDR;     // force SSB and SDR capability
#endif // QCX

#ifdef DEBUG
/*if((mcusr & WDRF) && (!(mcusr & EXTRF)) && (!(mcusr & BORF))){
  lcd.setCursor(0, 1); lcd.print(F("!!Watchdog RESET")); lcd_blanks();
  delay(1500); wdt_reset();
  }
  if((mcusr & BORF) && (!(mcusr & WDRF))){
  lcd.setCursor(0, 1); lcd.print(F("!!Brownout RESET")); lcd_blanks();  // Brow-out reset happened, CPU voltage not stable or make sure Brown-Out threshold is set OK (make sure E fuse is set to FD)
  delay(1500); wdt_reset();
  }
  if(mcusr & PORF){
  lcd.setCursor(0, 1); lcd.print(F("!!Power-On RESET")); lcd_blanks();
  delay(1500); wdt_reset();
  }*/
  /*if(mcusr & EXTRF){
  lcd.setCursor(0, 1); lcd.print(F("Power-On")); lcd_blanks();
  delay(1); wdt_reset();
  }*/

  // Measure CPU loads
	if (!(load_tx <= 100)) {
		fatal(F("CPU_tx"), load_tx, '%');
	}

	if (!(load_rx_avg <= 100)) {
		fatal(F("CPU_rx"), load_rx_avg, '%');
	}
	/*for(i = 0; i != 8; i++){
	  if(!(load_rx[i] <= 100)){   // and specify individual timings for each of the eight alternating processing functions
	  //fatal(F("CPU_rx"), load_rx[i], '%');
	  lcd.setCursor(0, 1); lcd.print(F("!!CPU_rx")); lcd.print(i); lcd.print('='); lcd.print(load_rx[i]); lcd.print('%'); lcd_blanks();
	  }
	}*/
#endif

#ifdef DIAG
	// Measure VDD (+5V); should be ~5V
	si5351.SendRegister(SI_CLK_OE, TX0RX0); // Mute QSD
	digitalWrite(KEY_OUT, LOW);
	digitalWrite(RX, LOW);  // mute RX
	delay(100); // settle
	float vdd = 2.0 * (float)analogRead(AUDIO2) * 5.0 / 1024.0;
	digitalWrite(RX, HIGH);
	if (!(vdd > 4.8 && vdd < 5.2)) {
		fatal(F("V5.0"), vdd, 'V');
	}

	// Measure VEE (+3.3V); should be ~3.3V
	float vee = (float)analogRead(SCL) * 5.0 / 1024.0;
	if (!(vee > 3.2 && vee < 3.8)) {
		fatal(F("V3.3"), vee, 'V');
	}

	// Measure AVCC via AREF and using internal 1.1V reference fed to ADC; should be ~5V
	analogRead(6); // setup almost proper ADC readout
	bitSet(ADMUX, 3); // Switch to channel 14 (Vbg=1.1V)
	delay(1); // delay improves accuracy
	bitSet(ADCSRA, ADSC);
	for (; bit_is_set(ADCSRA, ADSC););
	float avcc = 1.1 * 1023.0 / ADC;
	if (!(avcc > 4.6 && avcc < 5.2)) {
		fatal(F("Vavcc"), avcc, 'V');
	}

	// Report no SSB capability
	if (!ssb_cap) {
		fatal(F("No MIC input..."));
	}

	// Test microphone polarity
	/*if((ssb_cap) && (!_digitalRead(DAH))){
	  fatal(F("MIC in rev.pol"));
	}*/

	// Measure DVM bias; should be ~VAREF/2
#ifdef _SERIAL
	DDRC &= ~(1 << 2);  // disable PC2, so that ADC2 can be used as mic input
#else
	PORTD |= 1 << 1; DDRD |= 1 << 1;  // keep PD1 HIGH so that in case diode is installed to PC2 it is kept blocked (otherwise ADC2 input is pulled down!)
#endif
	delay(10);
#ifdef TX_ENABLE
	float dvm = (float)analogRead(DVM) * 5.0 / 1024.0;
	if ((ssb_cap) && !(dvm > 1.8 && dvm < 3.2)) {
		fatal(F("Vadc2"), dvm, 'V');
	}
#endif

	// Measure AUDIO1, AUDIO2 bias; should be ~VAREF/2
	if (dsp_cap == SDR) {
		float audio1 = (float)analogRead(AUDIO1) * 5.0 / 1024.0;
		if (!(audio1 > 1.8 && audio1 < 3.2)) {
			fatal(F("Vadc0"), audio1, 'V');
		}
		float audio2 = (float)analogRead(AUDIO2) * 5.0 / 1024.0;
		if (!(audio2 > 1.8 && audio2 < 3.2)) {
			fatal(F("Vadc1"), audio2, 'V');
		}
	}

#ifdef TX_ENABLE
	// Measure I2C Bus speed for Bulk Transfers
	//si5351.freq(freq, 0, 90);
	wdt_reset();
	t0 = micros();
	for (i = 0; i != 1000; i++) si5351.SendPLLRegisterBulk();
	t1 = micros();
	uint32_t speed = (1000000 * 8 * 7) / (t1 - t0); // speed in kbit/s
	if (false) {
		fatal(F("i2cspeed"), speed, 'k');
	}

	// Measure I2C Bit-Error Rate (BER); should be error free for a thousand random bulk PLLB writes
	si5351.freq(freq, 0, 90);  // freq needs to be set in order to use freq_calc_fast()
	wdt_reset();
	uint16_t i2c_error = 0;  // number of I2C byte transfer errors
	for (i = 0; i != 1000; i++) {
		si5351.freq_calc_fast(i);
		//for(int j = 0; j != 8; j++) si5351.pll_regs[j] = rand();
		si5351.SendPLLRegisterBulk();
#define SI_SYNTH_PLL_A 26
#ifdef NEW_TX
		for (int j = 4; j != 8; j++) if (si5351.RecvRegister(SI_SYNTH_PLL_A + j) != si5351.pll_regs[j]) i2c_error++;
#else
		for (int j = 3; j != 8; j++) if (si5351.RecvRegister(SI_SYNTH_PLL_A + j) != si5351.pll_regs[j]) i2c_error++;
#endif //NEW_TX
	}
	wdt_reset();
	if (i2c_error) {
		fatal(F("BER_i2c"), i2c_error, ' ');
	}
#endif //TX_ENABLE
#endif  // DIAG

	drive = 4;  // Init settings
#ifdef QCX
	if (!ssb_cap) { vfomode[0] = CW; vfomode[1] = CW; filt = 4; stepsize = STEP_500; }
	if (dsp_cap != SDR) pwm_max = 255; // implies that key-shaping circuit is probably present, so use full-scale
	if (dsp_cap == DSP) volume = 10;
	if (!dsp_cap) cw_tone = 2;   // use internal 700Hz QCX filter, so use same offset and keyer tone
#endif //QCX
	cw_offset = tones[cw_tone];
	//freq = bands[band];

#ifdef KEEP_BAND_DATA  // G8RDI mod
	memset(freq_last, 0, sizeof(freq_last));   // G8RDI mod - set to default
	//memset(mode_last, LSB, sizeof(mode_last));
	mode_last[0] = mode_last[1] = mode_last[2] = mode_last[3] = LSB;
	mode_last[4] = mode_last[5] = mode_last[6] = mode_last[7] = mode_last[8] = USB;   // Set for up to 9 bands only xyzzy
#endif

	// Load parameters from EEPROM, reset to factory defaults when stored values are from a different version
	paramAction(LOAD, VERS);

	if ((eeprom_version != get_version_id()) || _digitalRead(BUTTONS)) {  // EEPROM clean: if rotary-key pressed or version signature in EEPROM does NOT corresponds with this firmware
		eeprom_version = get_version_id();

		// G8RDI mod - reduce EEPROM writes
		//for(int n = 0; n != 1024; n++){ eeprom_write_byte((uint8_t *) n, 0); wdt_reset(); } //clean EEPROM
		//eeprom_write_dword((uint32_t *)EEPROM_OFFSET/3, 0x000000);

		/*memset(freq_last, 0, sizeof(freq_last));   // G8RDI mod - set to default
		mode_last[0] = mode_last[1] = mode_last[2] = mode_last[3] = LSB;
		mode_last[4] = mode_last[5] = mode_last[6] = mode_last[7] = mode_last[8] = USB;   // Set for up to 9 bands only xyzzy
		*/
		paramAction(SAVE);  // save default parameter values
		lcd.setCursor(0, 1); lcd.print(F("Reset settings.."));
		delay(500); wdt_reset();
	}
	else {
		paramAction(LOAD);  // load all parameters
	}
	//if(abs((int32_t)F_XTAL - (int32_t)si5351.fxtal) > 50000){ si5351.fxtal = F_XTAL; }  // if F_XTAL frequency deviates too much with actual setting -> use default
	si5351.iqmsa = 0;  // enforce PLL reset
	change = true;
	prev_bandval = bandval;
	vox = false;  // disable VOX
	//nr = 2; // set 2 default / 0 disable NR
	rit = false;  // disable RIT
	freq = vfo[vfosel % 2];
	mode = vfomode[vfosel % 2];

#ifdef NR_FIR
	if (nr > 2)
		FirFilterSetup(7 + (((nr - 2) - 1) * 2), filt_val[filt], F_SAMP_RX / 8);  // GW8RDI mod
#endif

#ifdef TX_ENABLE
	build_lut();
#endif

	delay(800); // G8RDI mod added so visible

	show_banner();  // remove release number

	start_rx();   // Start radio receiver

#if defined(CAT) || defined(TESTBENCH)
#ifdef CAT_STREAMING
#define BAUD   115200           //  Baudrate used for serial communications
#else
#ifdef CAT_FAST
#define BAUD   115200           // 57600/115200 Fast comms, see if reduces audio glitching on CAT polling G8RDI mod
#else
#define BAUD   38400            // 38400 57600//115200//4800 //Baudrate used for serial communications (CAT, TESTBENCH)
#endif
#endif

	if (cat_enabled)  // G8RDI mod
	{
		Serial.begin(16000000ULL * BAUD / F_MCU); // corrected for F_CPU=20M
		Command_IF();
#if !defined(OLED) && defined(TESTBENCH)
		smode = 0;  // In case of LCD, turn off smeter
#endif
	}
#endif //CAT TESTBENCH

#ifdef KEYER
	keyerState = IDLE;
	keyerControl = IAMBICB;      // Or 0 for IAMBICA
	loadWPM(keyer_speed);        // Fix speed at 15 WPM
#endif //KEYER

	for (; !_digitalRead(DIT) || ((mode == CW && keyer_mode != SINGLE) && (!_digitalRead(DAH)));) { fatal(F("Check PTT/key")); }// wait until DIH/DAH/PTT is released to prevent TX on startup
}

static int32_t _step = 0;
//static int8_t prev_mode;

void loop()
{
#ifdef VOX_ENABLE
	if ((vox) && ((mode == LSB) || (mode == USB))) {  // If VOX enabled (and in LSB/USB mode), then take mic samples and feed ssb processing function, to derive amplitude, and potentially detect cross vox_threshold to detect a TX or RX event: this is expressed in tx variable
		if (!vox_tx) { // VOX not active
#ifdef MULTI_ADC
			if (vox_sample++ == 16) {  // take N sample, then process
				ssb(((int16_t)(vox_adc / 16) - (512 - AF_BIAS)) >> MIC_ATTEN);   // sampling mic
				vox_sample = 0;
				vox_adc = 0;
			}
			else {
				vox_adc += analogSampleMic();
			}
#else
			ssb(((int16_t)(analogSampleMic()) - 512) >> MIC_ATTEN);   // sampling mic
#endif
			if (tx) {  // TX triggered by audio -> TX
				vox_tx = 1;
				switch_rxtx(255);
				//for(;(tx);) wdt_reset();  // while in tx (workaround for RFI feedback related issue)
				//delay(100); tx = 255;
			}
		}
		else if (!tx) {  // VOX activated, no audio detected -> RX
			switch_rxtx(0);
			vox_tx = 0;
			delay(32); //delay(10);
			//vox_adc = 0; for(i = 0; i != 32; i++) ssb(0); //clean buffers
			//for(int i = 0; i != 32; i++) ssb((analogSampleMic() - 512) >> MIC_ATTEN); // clear internal buffer
			//tx = 0; // make sure tx is off (could have been triggered by rubbish in above statement)
		}
	}
#endif //VOX_ENABLE

#ifdef CW_DECODER
	//if((mode == CW) && cwdec) cw_decode();  // if(!(semi_qsk_timeout)) cw_decode(); else dec2();
	if ((mode == CW) && cwdec && ((!tx) && (!semi_qsk_timeout))) cw_decode();  // CW decoder only active during RX
#endif  //CW_DECODER

	if (menumode == 0) { // in main
#ifdef CW_DECODER
		if (cw_event) {
			const char offv[] = { 0, 7, 3, 5, 3, 7, 8 };    // G8RDI mod - adjusted to meet standard
			uint8_t offset = offv[smode]; // depending on smeter more/less cw-text
			//illegal format in Viz : G8RDI uint8_t offset = (uint8_t[7]){ 0, 7, 3, 5, 3, 7, 8 }[smode]; // depending on smeter more/less cw-text
			lcd.noCursor();
#ifdef OLED
			//cw_event = false; for(int i = 0; out[offset + i] != '\0'; i++){ lcd.setCursor(i, 0); lcd.print(out[offset + i]); if((!tx) && (!semi_qsk_timeout)) cw_decode(); }   // like 'lcd.print(out + offset);' but then in parallel calling cw_decoding() to handle long OLED writes
			//uint8_t i = cw_event - 1; if(out[offset + i]){ lcd.setCursor(i, 0); lcd.print(out[offset + i]); cw_event++; } else cw_event = false;  // since an oled string write would hold-up reliable decoding/keying, write only a single char each time and continue
			uint8_t i = cw_event - 1; if (15 - offset - i + 1) { lcd.setCursor(15 - offset - i, 0); lcd.print(out[15 - i]); cw_event++; }
			else cw_event = false;  // since an oled string write would hold-up reliable decoding/keying, write only a single char each time and continue
#else
			cw_event = false;
			lcd.setCursor(0, 0); lcd.print(out + offset);
#endif
			stepsize_showcursor();
		}
		else
#endif  //CW_DECODER
			if ((!semi_qsk_timeout) && (!vox_tx))
				smeter();
	}

#ifdef KEYER  //Keyer
	if (mode == CW && keyer_mode != SINGLE)  // check DIT/DAH keys for CW
  {
		switch (keyerState) { // Basic Iambic Keyer, keyerControl contains processing flags and keyer mode bits, Supports Iambic A and B, State machine based, uses calls to millis() for timing.
		case IDLE: // Wait for direct or latched paddle press
			if ((_digitalRead(DAH) == LOW) ||
				(_digitalRead(DIT) == LOW) ||
				(keyerControl & 0x03))
			{
#ifdef CW_MESSAGE
				cw_msg_event = 0;  // clear cw message event
#endif //CW_MESSAGE
				update_PaddleLatch();
				keyerState = CHK_DIT;
			}
			break;
		case CHK_DIT: // See if the dit paddle was pressed
			if (keyerControl & DIT_L) {
				keyerControl |= DIT_PROC;
				ktimer = ditTime;
				keyerState = KEYED_PREP;
			}
			else {
				keyerState = CHK_DAH;
			}
			break;
		case CHK_DAH: // See if dah paddle was pressed
			if (keyerControl & DAH_L) {
				ktimer = ditTime * 3;
				keyerState = KEYED_PREP;
			}
			else {
				keyerState = IDLE;
			}
			break;
		case KEYED_PREP: // Assert key down, start timing, state shared for dit or dah
			Key_state = HIGH;
			switch_rxtx(Key_state);
			ktimer += millis();                 // set ktimer to interval end time
			keyerControl &= ~(DIT_L + DAH_L);   // clear both paddle latch bits
			keyerState = KEYED;                 // next state
			break;
		case KEYED: // Wait for timer to expire
			if (millis() > ktimer) {            // are we at end of key down ?
				Key_state = LOW;
				switch_rxtx(Key_state);
				ktimer = millis() + ditTime;    // inter-element time
				keyerState = INTER_ELEMENT;     // next state
			}
			else if (keyerControl & IAMBICB) {
				update_PaddleLatch();           // early paddle latch in Iambic B mode
			}
			break;
		case INTER_ELEMENT:
			// Insert time between dits/dahs
			update_PaddleLatch();               // latch paddle state
			if (millis() > ktimer) {            // are we at end of inter-space ?
				if (keyerControl & DIT_PROC) {             // was it a dit or dah ?
					keyerControl &= ~(DIT_L + DIT_PROC);   // clear two bits
					keyerState = CHK_DAH;                  // dit done, check for dah
				}
				else {
					keyerControl &= ~(DAH_L);              // clear dah latch
					keyerState = IDLE;                     // go idle
				}
			}
			break;
		}

	}
	else {
#endif //KEYER

#ifdef TX_ENABLE
		uint8_t pin = ((mode == CW) && (keyer_swap)) ? DAH : DIT;
		if (!vox_tx)  //  ONLY if VOX not active, then check DIT/DAH (fix for VOX to prevent RFI feedback through EMI on DIT or DAH line)
			if (!_digitalRead(pin)) {  // PTT/DIT keys transmitter
#ifdef CW_MESSAGE
				cw_msg_event = 0;  // clear cw message event
#endif //CW_MESSAGE
				switch_rxtx(1);   // Switch to TX mode
				do {
					wdt_reset();
					delay((mode == CW) ? 10 : 100);  // keep the tx keyed for a while before sensing (helps against RFI issues on DAH/DAH line)
#ifdef SWR_METER
					if (smeter > 0 && mode == CW && millis() >= stimer) { readSWR(); stimer = millis() + 500; }
#endif
					if (inv ^ _digitalRead(BUTTONS)) break;  // break if button is pressed (to prevent potential lock-up)
				} while (!_digitalRead(pin)); // until released
				switch_rxtx(0);
			}
#endif //TX_ENABLE
#ifdef KEYER
	}
#endif //KEYER

#ifdef SEMI_QSK
	if ((semi_qsk_timeout) && (millis() > semi_qsk_timeout)) { switch_rxtx(0); }  // delayed QSK RX
#endif

	enum event_t { BL = 0x10, BR = 0x20, BE = 0x30, SC = 0x01, DC = 0x02, PL = 0x04, PLC = 0x05, PT = 0x0C }; // button-left, button-right and button-encoder; single-click, double-click, push-long, push-and-turn

	if (inv ^ _digitalRead(BUTTONS))   // Left-/Right-/Rotary-button (while not already pressed)
  {
		if (!((event & PL) || (event & PLC))) {  // hack: if there was long-push before, then fast forward
			uint16_t v = analogSafeRead(BUTTONS);
#ifdef CAT_EXT
			if (cat_key) { v = (cat_key & 0x04) ? 512 : (cat_key & 0x01) ? 870 : (cat_key & 0x02) ? 1024 : 0; }  // override analog value exercised by BUTTONS press
#endif //CAT_EXT
			event = SC;
			int32_t t0 = millis();
			for (; inv ^ _digitalRead(BUTTONS);) { // until released or long-press
				if ((millis() - t0) > 300) { event = PL; break; }
				wdt_reset();
			}
			delay(10); //debounce
			for (; (event != PL) && ((millis() - t0) < 500);) { // until 2nd press or timeout
				if (inv ^ _digitalRead(BUTTONS)) { event = DC; break; }
				wdt_reset();
			}
			for (; inv ^ _digitalRead(BUTTONS);) { // until released, or encoder is turned while longpress
				if (encoder_val && event == PL) { event = PT; break; }
#ifdef ONEBUTTON
				if (event == PL) break;  // do not lock on longpress, so that L and R buttons can be used for tuning
#endif
				wdt_reset();
			}  // Max. voltages at ADC3 for buttons L,R,E: 3.76V;4.55V;5V, thresholds are in center
			event |= (v < (uint16_t)(4.2 * 1024.0 / 5.0)) ? BL : (v < (uint16_t)(4.8 * 1024.0 / 5.0)) ? BR : BE; // determine which button pressed based on threshold levels
		}
		else {  // hack: fast forward handling
			event = (event & 0xf0) | ((encoder_val) ? PT : PLC/*PL*/);  // only alternate between push-long/turn when applicable
		}

		switch (event) {
#ifndef ONEBUTTON
		case BL | PL:  // Called when menu button pressed
		case BL | PLC: // or kept pressed
			menumode = 2;
			break;
		case BL | PT:
			menumode = 1;
			//if(menu == 0) menu = 1;
			break;
		case BL | SC:
#ifdef CW_MESSAGE
			if ((menumode == 1) && (menu >= CWMSG1) && (menu <= CWMSG6)) {
				cw_msg_event = millis();
				cw_msg_id = menu - CWMSG1;
				menumode = 0;
				break;
			}
#endif //CW_MESSAGE
			int8_t _menumode;
			if (menumode == 0) { _menumode = 1; if (menu == 0) menu = 1; }  // short left-click while in default screen: enter menu mode
			if (menumode == 1) { _menumode = 2; }                          // short left-click while in menu: enter value selection screen
			if (menumode >= 2) { _menumode = 0; paramAction(SAVE, menu); } // short left-click while in value selection screen: save, and return to default screen
			menumode = _menumode;
			break;
		case BL | DC:
			break;
		case BR | SC:   // Mode change - Button Right, Single Click
      changedMode = true; // GW8RDI 230401
      /* Mode processing handled below now
			if (!menumode) {
				prev_mode = mode;
				if (rit) { rit = 0; stepsize = prev_stepsize[mode == CW]; change = true;  break; }
				mode++;  // Change to next mode += 1
				//encoder_val = 1;
				//paramAction(UPDATE, MODE); // Mode param //paramAction(UPDATE, mode, NULL, F("Mode"), mode_label, 0, _N(mode_label), true);

//#define MODE_CHANGE_RESETS  1

#ifdef SHOW_USB_LSB_CW_ONLY
				if (mode > CW)  // Menu only shows USB, LSB, CW
#else
				if (mode > AM)  // G8RDI mod - *changed from > CW so that all modes can be accessed
#endif
					mode = LSB;  // *now shows all / skip all other modes (only LSB (0), USB, CW(2))
#ifdef MODE_CHANGE_RESETS
				if (mode != CW)
        {
            stepsize = STEP_1k;
          else
            stepsize = STEP_500; // sets suitable stepsize

          filt = 0;  // resets filter (to most BW) and NR on mode change
        }
				else { filt = 4; nr = 0; }
#else
				if (mode == CW) { nr = 0; }
				prev_stepsize[prev_mode == CW] = stepsize; stepsize = prev_stepsize[mode == CW]; // backup stepsize setting for previous mode, restore previous stepsize setting for current selected mode; filter settings captured for either CQ or other modes.
				prev_filt[prev_mode == CW] = filt;
				filt = prev_filt[mode == CW];  // backup filter setting for previous mode, restore previous filter setting for current selected mode; filter settings captured for either CQ or other modes.
#endif
				//paramAction(UPDATE, MODE);
				vfomode[vfosel % 2] = mode;
				paramAction(SAVE, (vfosel % 2) ? MODEB : MODEA);  // save vfoa/b changes
				paramAction(SAVE, MODE);
				paramAction(SAVE, FILTER);
				si5351.iqmsa = 0;  // enforce PLL reset
#ifdef CW_DECODER
				if (prev_mode == CW && cwdec)
					show_banner();
#endif
				change = true;
			}
			else {
				if (menumode == 1) { menumode = 0; }  // short right-click while in menu: enter value selection screen
				if (menumode >= 2) { menumode = 1; change = true; paramAction(SAVE, menu); } // short right-click while in value selection screen: save, and return to menu screen
			} */
			break;
		case BR | DC:
			filt++;
			_init = true;
			if (mode == CW && filt > N_FILT) filt = 4;
			if (mode == CW && filt == 4) stepsize = STEP_500; // reset stepsize for 500Hz filter
			if (mode == CW && (filt == 5 || filt == 6) && stepsize < STEP_100) stepsize = STEP_100; // for CW BW 200, 100      -> step = 100 Hz
			if (mode == CW && filt == 7 && stepsize < STEP_10) stepsize = STEP_10;                  // for CW BW 50 -> step = 10 Hz
			if (mode != CW && filt > 3) filt = 0;
			encoder_val = 0;
			paramAction(UPDATE, FILTER);
			paramAction(SAVE, FILTER);
			wdt_reset(); delay(1500); wdt_reset();
			change = true; // refresh display
#ifdef NR_FIR
			if (nr > 2)
				FirFilterSetup(7 + (((nr - 2) - 1) * 2), filt_val[filt], F_SAMP_RX / 8);  // GW8RDI mod
#endif
			break;
		case BR | PL:
#ifdef SIMPLE_RX
			// Experiment: ISR-less sdr_rx():
			smode = 0;
			TIMSK2 &= ~(1 << OCIE2A);  // disable timer compare interrupt
			delay(100);
			lcd.setCursor(15, 1); lcd.print('X');
			static uint8_t x = 0;
			uint32_t next = 0;
			for (;;) {
				func_ptr();
#ifdef DEBUG
				numSamples++;
#endif
				if (!rx_state) {
					x++;
					if (x > 16) {
						loop();
						//lcd.setCursor(9, 0); lcd.print((int16_t)100); lcd.print(F("dBm   "));  // delays are taking too long!
						x = 0;
					}
				}
				//for(;micros() < next;);  next = micros() + 16;   // sync every 1000000/62500=16ms (or later if missed)
			} //
#endif //SIMPLE_RX
#ifdef RIT_ENABLE
			rit = !rit;
			stepsize = (rit) ? STEP_10 : prev_stepsize[mode == CW];
			if (!rit) {  // after RIT comes VFO A/B swap
#else
			{
#endif //RIT_ENABLE
        vfosel = !vfosel;
        freq = vfo[vfosel % 2];  // todo: share code with menumode
        mode = vfomode[vfosel % 2];
        // make more generic: 
        if (mode != CW) stepsize = STEP_1k; else stepsize = STEP_500;
        if (mode == CW) { filt = 4; nr = 0; }
        else filt = 0;
			}
				change = true;
				break;
				//#define TUNING_DIAL  1
#ifdef TUNING_DIAL
		case BR | PLC:  // while pressed long continues
		case BE | PLC:
			freq = freq + ((_step > 0) ? 1 : -1) * pow(2, abs(_step)); change = true;
			break;
		case BR | PT:
			_step += encoder_val; encoder_val = 0;
			lcd.setCursor(0, 0); lcd.print(_step); lcd_blanks();
			break;
#endif //TUNING_DIAL
		case BE | SC:
			if (!menumode) {
				stepsize_change(+1);
			}
			else {
				int8_t _menumode;
				if (menumode == 1) { _menumode = 2; }  // short encoder-click while in menu: enter value selection screen
				if (menumode == 2) { _menumode = 1; change = true; paramAction(SAVE, menu); } // short encoder-click while in value selection screen: save, and return to menu screen
#ifdef MENU_STR
				if (menumode == 3) { _menumode = 3; paramAction(NEXT_CH, menu); } // short encoder-click while in string edit mode: change position to next character
#endif
				menumode = _menumode;
			}
			break;

		case BE | DC:     // Button Encoder and DC double-click for Band change (( G8RDI mod - now bi-directional and restores freq & mode)

#ifdef KEEP_BAND_DATA  // G8RDI mod
			prev_bandval = bandval;
			prev_mode = mode;
			if (bandval > 0 && bandval <= BANDCOUNT)   // bandval 1-8 (0 is 6m, 9 is 160m)
			{
				freq_last[bandval - 1] = freq;  //vfo[vfosel % 2]    // G8RDI mod - Save freq and mode last used on this band
				mode_last[bandval - 1] = vfomode[vfosel % 2];
			}
#ifdef DEBUG_G8RDI
			else
			{
				error_code = 2;
				show_banner();  // Debug only to show error
				delay(600);
			}
#endif
#endif
			//delay(100);
			///if(bandval >= (N_BANDS-1)) bandval = 1;  // excludes 6m, 160m

			//if (last_state == 0x31 || last_state == 0x10 || last_state == 0x02 || last_state == 0x23) // ++Dir last freq step - G8RDI mod
#if defined(RED_BUTTONS) || defined(WHITE_BUTTONS) || defined(BLACK_BRICK)  // For some reason, even without SWAP_ROTARY, Red Buttons is reversed
#if defined(REVERSE_BAND_CHANGE)
			if (last_state == 0x13 || last_state == 0x32 || last_state == 0x20 || last_state == 0x01) // --Dir last freq step - G8RDI mod
				bandval++;
			else
				bandval--;
#else
			if (last_state == 0x13 || last_state == 0x32 || last_state == 0x20 || last_state == 0x01) // --Dir last freq step - G8RDI mod
				bandval--;
			else
				bandval++;    //  G8RDI mod to make last freq change control and change dir
#endif  
#else
#if defined(SWAP_ROTARY) || defined(REVERSE_BAND_CHANGE)  // G8RDI mod A7. If your freq. change is correct, but band jump goes backwards, define REVERSE_BAND_CHANGE
			if (last_state == 0x13 || last_state == 0x32 || last_state == 0x20 || last_state == 0x01) // --Dir last freq step - G8RDI mod
				bandval--;
			else
				bandval++;    //  G8RDI mod to make last freq change control and change dir
#else
			if (last_state == 0x13 || last_state == 0x32 || last_state == 0x20 || last_state == 0x01) // --Dir last freq step - G8RDI mod
				bandval++;
			else
				bandval--;    //  G8RDI mod to make last freq change control and change dir
#endif
#endif          

#ifdef TRUSDX
			if (bandval >= (N_BANDS - 5))
#else
			if (bandval >= (N_BANDS - 1))
#endif
        bandval = 1;  // excludes 6m
			else
				if (bandval < 1) bandval = N_BANDS - 2;  // excludes 160m  // G8RDI mod - added

			// G8RDI mod 2022/07/19 end.

			stepsize = STEP_500;  // G8RDI mod //STEP_1k;
			change = true;
			break;

		case BE | PL: stepsize_change(-1); break;
		case BE | PT:
			for (; _digitalRead(BUTTONS);) { // process encoder changes until released
				wdt_reset();
				if (encoder_val) {
					paramAction(UPDATE, VOLUME);
					if (volume < 0) { volume = 10; paramAction(SAVE, VOLUME); powerDown(); }  // powerDown when volume < 0
					paramAction(SAVE, VOLUME);
				}
			}
			change = true; // refresh display
			break;
#else //ONEBUTTON
		case BE | SC:
			int8_t _menumode;
			if (menumode == 0) { _menumode = 1; if (menu == 0) menu = 1; }  // short enc-click while in default screen: enter menu mode
			if (menumode == 1) { _menumode = 2; }                          // short enc-click while in menu: enter value selection screen
			if (menumode == 2) { _menumode = 0; paramAction(SAVE, menu); } // short enc-click while in value selection screen: save, and return to default screen
#ifdef MENU_STR
			if (menumode == 3) { _menumode = 3; paramAction(NEXT_CH, menu); } // short encoder-click while in string edit mode: change position to next character
#endif
			menumode = _menumode;
			break;
/* GW8RDI now used for band changing      
		case BE | DC: // Mode change
      if (!menumode)
      {
				prev_mode = mode;
				if (rit) { rit = 0; stepsize = prev_stepsize[mode == CW]; change = true;  break; }
				mode += 1;    // Step thro modes
				//encoder_val = 1;
				//paramAction(UPDATE, MODE); // Mode param //paramAction(UPDATE, mode, NULL, F("Mode"), mode_label, 0, _N(mode_label), true);
			//#define MODE_CHANGE_RESETS  1
#ifdef MODE_CHANGE_RESETS
				if (mode != CW) stepsize = STEP_1k; else stepsize = STEP_500; // sets suitable stepsize
#endif
				if (mode > CW)
          mode = LSB;  // skip all other modes (only LSB, USB, CW)
#ifdef MODE_CHANGE_RESETS
				if (mode == CW) { filt = 4; nr = 0; }
				else filt = 0;  // resets filter (to most BW) and NR on mode change
#else
				if (mode == CW) { nr = 0; }
				prev_stepsize[prev_mode == CW] = stepsize; stepsize = prev_stepsize[mode == CW]; // backup stepsize setting for previous mode, restore previous stepsize setting for current selected mode; filter settings captured for either CQ or other modes.
				prev_filt[prev_mode == CW] = filt; filt = prev_filt[mode == CW];  // backup filter setting for previous mode, restore previous filter setting for current selected mode; filter settings captured for either CQ or other modes.
#endif
				//paramAction(UPDATE, MODE);
				vfomode[vfosel % 2] = mode;
				paramAction(SAVE, (vfosel % 2) ? MODEB : MODEA);  // save vfoa/b changes
				paramAction(SAVE, MODE);
				paramAction(SAVE, FILTER);
				si5351.iqmsa = 0;  // enforce PLL reset
				if ((prev_mode == CW) && (cwdec)) show_banner();
				change = true;
			}
			else {
				if (menumode == 1) { menumode = 0; }  // short right-click while in menu: enter value selection screen
				if (menumode >= 2) { menumode = 1; change = true; paramAction(SAVE, menu); } // short right-click while in value selection screen: save, and return to menu screen
			}
			break;
*/
		case BE | PL:
			stepsize += 1;
			if (stepsize < STEP_1k) stepsize = STEP_10;
			if (stepsize > STEP_10) stepsize = STEP_1k;
			stepsize_showcursor();
			break;
		case BE | PLC: // or kept pressed
			menumode = 2;
			break;
		case BE | PT:
			menumode = 1;
			//if(menu == 0) menu = 1;
			break;
		case BL | SC:
		case BL | DC:
		case BL | PL:
		case BL | PLC:
			encoder_val++;
			break;
		case BR | SC:
		case BR | DC:
		case BR | PL:
		case BR | PLC:
			encoder_val--;
			break;
#endif //ONEBUTTON
			}
		}
	else event = 0;  // no button pressed: reset event

  if (changedMode || changedModeCAT)  // 230401 GW8RDI
  {
    changedMode = false;
  
    if (!menumode)
    {
      /*if (rit)  // If in RIT mode ignore mode change - original uSDX behaviour
      {
        rit = 0; stepsize = prev_stepsize[mode == CW];
        change = true;
      }
      else  */ // GW8RDI mod - modulation mode changes now supported in RIT mode
      {
        if (!changedModeCAT)  // Note: CAT cmd reception sets mode
        {
          prev_mode = mode;
          mode++;  // Change to next mode += 1
          //encoder_val = 1;
          //paramAction(UPDATE, MODE); // Mode param //paramAction(UPDATE, mode, NULL, F("Mode"), mode_label, 0, _N(mode_label), true);

#ifdef SHOW_USB_LSB_CW_ONLY
          if (mode > CW)  // Mode button only cycles USB, LSB, CW only
            mode = LSB;   // Skip all other modes (only LSB (0), USB, CW(2))
#else
          if (mode > AM)  // G8RDI mod - *changed from > CW so that all modes can be accessed
            mode = LSB;   // *now shows all / skip all other modes (only LSB (0), USB, CW(2))
#endif
        }
        else
        {
          if (mode > AM)  // G8RDI mod - allow changedModeCAT to change to any mode including AM
            mode = LSB;
        }

//#define MODE_CHANGE_RESETS  1
#ifdef MODE_CHANGE_RESETS
        if (mode != CW)
        {
          stepsize = STEP_500; // sets suitable stepsize //STEP_1k
          filt = 0;  // resets filter (to most BW) and NR on mode change
        }
        else
        { filt = 4; nr = 0; }
#else
        if (mode == CW) { nr = 0; }
        prev_stepsize[prev_mode == CW] = stepsize; stepsize = prev_stepsize[mode == CW]; // backup stepsize setting for previous mode, restore previous stepsize setting for current selected mode; filter settings captured for either CQ or other modes.
        prev_filt[prev_mode == CW] = filt;
        filt = prev_filt[mode == CW];  // backup filter setting for previous mode, restore previous filter setting for current selected mode; filter settings captured for either CQ or other modes.
#endif
        //paramAction(UPDATE, MODE);
        vfomode[vfosel % 2] = mode;
        paramAction(SAVE, (vfosel % 2) ? MODEB : MODEA);  // save vfoa/b changes
        paramAction(SAVE, MODE);
        paramAction(SAVE, FILTER);
        si5351.iqmsa = 0;  // enforce PLL reset
#ifdef CW_DECODER
        if (prev_mode == CW && cwdec)
          show_banner();
#endif
        change = true;
      }
    }
    else
    {
      if (menumode == 1) { menumode = 0; }  // short right-click while in menu: enter value selection screen
      if (menumode >= 2) { menumode = 1; change = true; paramAction(SAVE, menu); } // short right-click while in value selection screen: save, and return to menu screen
    }
    changedModeCAT = false;
  }

	if ((menumode) || (prev_menumode != menumode)) {  // Show parameter and value
		int8_t encoder_change = encoder_val;
		if ((menumode == 1) && encoder_change) {
			menu += encoder_val;   // Navigate through menu
#ifdef ONEBUTTON
			menu = max(0, min(menu, N_PARAMS));
#else
			// G8RDI mod to cycle menu //menu = max(1 /* 0 */, min(menu, N_PARAMS));
			if (menu > N_PARAMS)
				menu = 1;
			else
				if (menu < 1)
					menu = N_PARAMS;
#endif
			menu = paramAction(NEXT_MENU, menu);  // auto probe next menu item (gaps may exist)
			encoder_val = 0;
		}
		if (encoder_change || (prev_menumode != menumode)) paramAction(UPDATE_MENU, (menumode) ? menu : 0);  // update param with encoder change and display
		prev_menumode = menumode;
		if (menumode == 2) {
			if (encoder_change) {
				lcd.setCursor(0, 1); lcd.cursor();  // edits menu item value; make cursor visible
				if (menu == MODE) { // post-handling Mode parameter
					vfomode[vfosel % 2] = mode;
					paramAction(SAVE, (vfosel % 2) ? MODEB : MODEA);  // save vfoa/b changes
					change = true;
					si5351.iqmsa = 0;  // enforce PLL reset
					// make more generic: 
					if (mode != CW) stepsize = STEP_1k; else stepsize = STEP_500;
					if (mode == CW) { filt = 4; nr = 0; }
					else filt = 0;
				}
				if (menu == BAND) {
					change = true;
				}
				//if(menu == NR){ if(mode == CW) nr = false; }
				if (menu == VFOSEL) {
					freq = vfo[vfosel % 2];
					mode = vfomode[vfosel % 2];
					// make more generic: 
					if (mode != CW) stepsize = STEP_1k; else stepsize = STEP_500;
					if (mode == CW) { filt = 4; nr = 0; }
					else filt = 0;
					change = true;
				}
#ifdef RIT_ENABLE
				if (menu == RIT) {
					stepsize = (rit) ? STEP_10 : STEP_500;
					change = true;
				}
#endif
				//if(menu == VOX){ if(vox){ vox_thresh-=1; } else { vox_thresh+=1; }; }
				if (menu == ATT) { // post-handling ATT parameter
					if (dsp_cap == SDR) {
						noInterrupts();
#ifdef SWAP_RX_IQ
						adc_start(1, !(att & 0x01)/*true*/, F_ADC_CONV); admux[0] = ADMUX;
						adc_start(0, !(att & 0x01)/*true*/, F_ADC_CONV); admux[1] = ADMUX;
#else
						adc_start(0, !(att & 0x01)/*true*/, F_ADC_CONV); admux[0] = ADMUX;
						adc_start(1, !(att & 0x01)/*true*/, F_ADC_CONV); admux[1] = ADMUX;
#endif //SWAP_RX_IQ
						interrupts();
					}
					digitalWrite(RX, !(att & 0x02)); // att bit 1 ON: attenuate -20dB by disabling RX line, switching Q5 (antenna input switch) into 100k resistence
					pinMode(AUDIO1, (att & 0x04) ? OUTPUT : INPUT); // att bit 2 ON: attenuate -40dB by terminating ADC inputs with 10R
					pinMode(AUDIO2, (att & 0x04) ? OUTPUT : INPUT);
				}
				if (menu == SIFXTAL) {
					change = true;
				}
#ifdef TX_ENABLE
				if ((menu == PWM_MIN) || (menu == PWM_MAX)) {
					build_lut();
				}
#endif
				if (menu == CWTONE) {
					if (dsp_cap) { cw_offset = (cw_tone == 0) ? tones[0] : tones[1]; paramAction(SAVE, CWOFF); }
				}
				if (menu == IQ_ADJ) {
					change = true;
				}
#ifdef CAL_IQ
				if (menu == CALIB) {
					if (dsp_cap != SDR) calibrate_iq(); menu = 0;
				}
#endif
#ifdef KEYER
				if (menu == KEY_WPM) {
					loadWPM(keyer_speed);
				}
				if (menu == KEY_MODE) {
					if (keyer_mode == 0) { keyerControl = IAMBICA; }
					if (keyer_mode == 1) { keyerControl = IAMBICB; }
					if (keyer_mode == 2) { keyerControl = SINGLE; }
				}
#endif //KEYER
#ifdef TX_DELAY
				if (menu == TXDELAY) {
					semi_qsk = (txdelay > 0);
				}
#endif //TX_DELAY
			}
#ifdef DEBUG
			if (menu == SR) {          // measure sample-rate
				numSamples = 0;
				delay(F_MCU * 500UL / 16000000);   // delay 0.5s (in reality because F_CPU=20M instead of 16M, delay() is running 1.25x faster therefore we need to multiply with 1.25)
				sr = numSamples * 2;   // samples per second
				paramAction(UPDATE_MENU, menu); // refresh
			}
			if (menu == CPULOAD) {     // measure CPU-load
				uint32_t i = 0;
				uint32_t prev_time = millis();
				for (i = 0; i != 300000; i++) wdt_reset(); // fixed CPU-load 132052*1.25us delay under 0% load condition; is 132052*1.25 * 20M = 3301300 CPU cycles fixed load
				cpu_load = 100 - 132 * 100 / (millis() - prev_time);
				paramAction(UPDATE_MENU, menu); // refresh
			}
			if ((menu == PARAM_A) || (menu == PARAM_B) || (menu == PARAM_C)) {
				delay(300);
				paramAction(UPDATE_MENU, menu); // refresh
			}
#endif
		}
	}

	if (menumode == 0) {
		if (encoder_val) {  // process encoder tuning steps
			process_encoder_tuning_step(encoder_val);
			encoder_val = 0;
		}
	}

	if ((change) && (!tx) && (!vox_tx))  // Only change if TX is OFF, prevent simultaneous I2C bus access
  {
		change = false;
		if (prev_bandval != bandval) {  // If band changed
			prev_bandval = bandval;

#ifdef KEEP_BAND_DATA  // G8RDI mod
			if (freq_last[bandval - 1] != 0)    // G8RDI mod
			{
				freq = freq_last[bandval - 1];  // Change to last freq used on this band
				if (freq > 60000000)  // Keep in range to avoid lots of twiddling
				{
					freq = band[bandval];
					freq_last[bandval - 1] = freq;
				}
			}
			else
				freq = band[bandval];   // Load default
			if (mode_last[bandval - 1] > AM)
			{
				mode_last[bandval - 1] = LSB;  // Should never happen, error! G8RDI mod
				error_code = 3;
			}

      mode = mode_last[bandval - 1];   // Change to last used mode on this band

			// G8RDI mod 2022/07/19 updated filter, step and NR as per mode change.  NOTE: TODO We could save previous filter and maybe noise settings, CW decode needs to be disable for non-CW modes. We could also save for both VFO A & B:-                       
			//if (rit) {
				//rit = 0; stepsize = prev_stepsize[mode == CW]; }

			if (mode > CW)
				mode = LSB;  // skip all other modes (only LSB (0), USB, CW(2))
#ifdef MODE_CHANGE_RESETS
			if (mode != CW) stepsize = STEP_1k; else stepsize = STEP_500; // sets suitable stepsize
			if (mode == CW) { filt = 4; nr = 0; }
			else filt = 0;  // resets filter (to most BW) and NR on mode change
#else
			if (mode == CW) { nr = 0; }
			prev_stepsize[prev_mode == CW] = stepsize; stepsize = prev_stepsize[mode == CW]; // backup stepsize setting for previous mode, restore previous stepsize setting for current selected mode; filter settings captured for either CQ or other modes.
			prev_filt[prev_mode == CW] = filt; filt = prev_filt[mode == CW];  // backup filter setting for previous mode, restore previous filter setting for current selected mode; filter settings captured for either CQ or other modes.
#endif
#ifdef CW_DECODER
			if (prev_mode == CW && cwdec)
				show_banner();
#endif

#else
			freq = band[bandval];   // Change to new band freq start
#endif
			//paramAction(UPDATE, MODE);
			vfomode[vfosel % 2] = mode;
			paramAction(SAVE, (vfosel % 2) ? MODEB : MODEA);  // save vfoa/b changes
			paramAction(SAVE, MODE);
			paramAction(SAVE, FILTER);
			si5351.iqmsa = 0;  // enforce PLL reset
		}

		vfo[vfosel % 2] = freq;
		//save_event_time = millis() + 1000;  // schedule time to save freq (no save while tuning, hence no EEPROM wear out - G8RDI "Datasheet: Write/erase cycles: 10,000 flash/100,000 EEPROM")
		save_event_time = millis() + 2000;  // G8RDI mod - increased to 2 seconds // schedule time to save freq (no save while tuning, hence no EEPROM wear out - G8RDI "Datasheet: Write/erase cycles: 10,000 flash/100,000 EEPROM")

		if (menumode == 0) {
			display_vfo(freq);
			stepsize_showcursor();
#ifdef CAT
			//Command_GETFreqA();
#endif

	// The following is a hack for SWR measurement:
	//si5351.alt_clk2(freq + 2400);
	//si5351.SendRegister(SI_CLK_OE, TX1RX1);
	//digitalWrite(SIG_OUT, HIGH);  // inject CLK2 on antenna input via 120K
		}

		//noInterrupts();
		uint8_t f = freq / 1000000UL;
		set_lpf(f);
		bandval = (f > 32) ? 10 : (f > 26) ? 9 : (f > 22) ? 8 : (f > 20) ? 7 : (f > 16) ? 6 : (f > 12) ? 5 : (f > 8) ? 4 : (f > 6) ? 3 : (f > 4) ? 2 : (f > 2) ? 1 : 0;  prev_bandval = bandval; // align bandval with freq

		if (mode == CW) {
			si5351.freq(freq + cw_offset, rx_ph_q, 0/*90, 0*/);  // RX in CW-R (=LSB), correct for CW-tone offset
		}
		else
			if (mode == LSB)
				si5351.freq(freq, rx_ph_q, 0/*90, 0*/);  // RX in LSB
			else
				si5351.freq(freq, 0, rx_ph_q/*0, 90*/);  // RX in USB, ...
#ifdef RIT_ENABLE
		if (rit) { si5351.freq_calc_fast(rit); si5351.SendPLLRegisterBulk(); }
#endif //RIT_ENABLE
		//interrupts();
	}

	if (save_event_time && (millis() > save_event_time)) {  // save freq when time has reached schedule - reduce EEPROM writes as 10k limit to burnout (G8RDI)!
		paramAction(SAVE, (vfosel % 2) ? FREQB : FREQA);  // save vfoa/b changes

#ifdef KEEP_BAND_DATA  // G8RDI mod
		freq_last[bandval - 1] = vfo[vfosel % 2];       // = freq;
		mode_last[bandval - 1] = vfomode[vfosel % 2];   // = mode;

/* 230401
		switch (bandval - 1)    // G8RDI mod - added Save only changed
		{
		case 0:
			paramAction(SAVE, BAND_DATA0); break;  // Save updated data only
		case 1:
			paramAction(SAVE, BAND_DATA1); break;
		case 2:
			paramAction(SAVE, BAND_DATA2); break;
		case 3:
			paramAction(SAVE, BAND_DATA3); break;
		case 4:
			paramAction(SAVE, BAND_DATA4); break;
		case 5:
			paramAction(SAVE, BAND_DATA5); break;
		case 6:
			paramAction(SAVE, BAND_DATA6); break;
		case 7:
			paramAction(SAVE, BAND_DATA7); break;
		case 8:
			paramAction(SAVE, BAND_DATA8); break;
		default: error_code = 1;      // Flag error
		}
*/
    if ((bandval - 1) <= 8)
      paramAction(SAVE, BAND_DATA0 + (bandval - 1));  // Save updated data only
    else
      error_code = 1;      // Flag error

#endif
		save_event_time = 0;
		//lcd.setCursor(15, 1); lcd.print('S'); delay(100); lcd.setCursor(15, 1); lcd.print('R');
	}

#ifdef CW_MESSAGE
	// Auto-send CW message
	if ((mode == CW) && (cw_msg_event) && (millis() > cw_msg_event)) {  // if it is time, send CW message
		if ((cw_tx(cw_msg[cw_msg_id]) == 0) && ((cw_msg[cw_msg_id][0] == 'C') && (cw_msg[cw_msg_id][1] == 'Q')) && cw_msg_interval)     // If msg starts CQ, schedule repeat send at set interval
			cw_msg_event = millis() + (1000 * cw_msg_interval);
		else
			cw_msg_event = 0;  // Done/aborted by key press (if CW aborts can also be caused by noise getting onto keys ADC line)
	}
#endif //CW_MESSAGE

	wdt_reset();

	//{ lcd.setCursor(0, 0); lcd.print(freeMemory()); lcd.print(F("    ")); }
	}

/* BACKLOG:
code definitions and re-use for comb, integrator, dc decoupling, arctan
refactor main()
agc based on rms256, agc/smeter after filter
noisefree integrator (rx audio out) in lower range
raised cosine tx amp for cw, 4ms tau seems enough: http://fermi.la.asu.edu/w9cf/articles/click/index.html
32 bin fft
dynamic range cw
att extended agc
Split
undersampling, IF-offset
K2/TS480 CAT control
faster RX-TX switch to support CW
usdx API demo code
scan
move last bit of arrays into flash? https://web.archive.org/web/20180324010832/https://www.microchip.com/webdoc/AVRLibcReferenceManual/FAQ_1faq_rom_array.html
u-law in RX path?: http://dystopiancode.blogspot.com/2012/02/pcm-law-and-u-law-companding-algorithms.html
Arduino library?
1. RX bias offset correction by measurement avg, 2. charge decoupling cap. by resetting to 0V and setting 5V for a certain amount of (charge) time
add 1K (500R?) to GND at TTL RF output to keep zero-level below BS170 threshold
additional PWM output for potential BOOST conversion
squelch gating
more buttons
s-meter offset vs DC bal.
keyer with interrupt-driven timers (to reduce jitter)

Analyse assembly:
/home/guido/Downloads/arduino-1.8.10/hardware/tools/avr/bin/avr-g++ -S -g -Os -w -std=gnu++11 -fpermissive -fno-exceptions -ffunction-sections -fdata-sections -fno-threadsafe-statics -Wno-error=narrowing -MMD -mmcu=atmega328p -DF_CPU=16000000L -DARDUINO=10810 -DARDUINO_AVR_UNO -DARDUINO_ARCH_AVR -I/home/guido/Downloads/arduino-1.8.10/hardware/arduino/avr/cores/arduino -I/home/guido/Downloads/arduino-1.8.10/hardware/arduino/avr/variants/standard /tmp/arduino_build_483134/sketch/QCX-SSB.ino.cpp -o /tmp/arduino_build_483134/sketch/QCX-SSB.ino.cpp.txt

Rewire/code I/Q clk pins so that a Div/1 and Div/2 scheme is used instead of 0 and 90 degrees phase shift
10,11,13,12   10,11,12,13  (pin)
Q- I+ Q+ I-   Q- I+ Q+ I-
90 deg.shift  div/2@S1(pin2)

50MHz LSB OK, USB NOK

atmega328p signature: https://forum.arduino.cc/index.php?topic=341799.15   https://www.eevblog.com/forum/microcontrollers/bootloader-on-smd-atmega328p-au/msg268938/#msg268938 https://www.avrfreaks.net/forum/undocumented-signature-row-contents

Alain k1fm AGC sens issue:  https://groups.io/g/ucx/message/3998   https://groups.io/g/ucx/message/3999
txdelay when vox is on (disregading the tx>0 state due to ssb() overrule, instead use RX-digitalinput)
Adrian: issue #41, set cursor just after writing 'R' when smeter is off, and (menumode == 0)
Konstantinos: backup/restore vfofilt settings when changing vfo.
Bob: 2mA for clk0/1 during RX
Uli: accuracate voltages during diag

agc behind filter
vcc adc extend. power/curr measurement
swr predistort eff calc
block ptt while in vox mode

adc bias error and potential error correction
noise burst on tx
https://groups.io/g/ucx/topic/81030243#6265

for (size_t i = 0; i < 9; i++) id[i] = boot_signature_byte_get(0x0E + i + (i > 5));

// https://www.ti.com/lit/ds/symlink/ina226.pdf
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27, 20, 4);
#include <Adafruit_INA219.h>
Adafruit_INA219 ina219;
float pwr;
float Eff;
float Vinc, Vref = 0, SWR;
float k = 0.85;
float busvoltage = 0;
float current_mA = 0;
float power_mW = 0;

void setup() {
  ina219.begin();
  lcd.init();
  lcd.backlight();
}
void loop() {
  busvoltage = ina219.getBusVoltage_V();
  current_mA = ina219.getCurrent_mA();
  power_mW = ina219.getPower_mW();
  Vinc = analogRead(3);
  Vref = analogRead(2);
  SWR = (Vinc + Vref) / (Vinc - Vref);
  Vinc = ((Vinc * 5.0) / 1024.0) + 0.5;
  pwr = ((((Vinc) * (Vinc)) - 0.25 ) * k);
  Eff = (pwr) / ((power_mW) / 1000) * 100;
  if (pwr > 0 ) (pwr = pwr + 0.25);
  lcd.setCursor(0, 0);
  lcd.print("SWR     :1 / P     W");
  lcd.setCursor(4, 0);
  lcd.print(SWR);
  lcd.setCursor(15, 0);
  lcd.print(pwr);

  lcd.setCursor (0, 2);
  //lcd.print ("Vss = ");
  lcd.print(busvoltage);
  lcd.print("V  ");

  lcd.setCursor(8, 2);
  lcd.print (-((current_mA) / 1000));
  lcd.print("A  ");

  lcd.setCursor(15, 2);
  lcd.print((power_mW) / 1000);
  lcd.print("W   ");
  lcd.setCursor(0, 1);
  lcd.print("Efficiency = ");
  lcd.print(Eff);
  lcd.setCursor(17, 1);
  lcd.print("%   ");

  delay(300);
}
*/
/*
int8_t updateMode() // GW8RDI mod - relocated to function
{
  if (!menumode)
  {
    prev_mode = mode;
    if (rit) { rit = 0; stepsize = prev_stepsize[mode == CW]; change = true;  return -1; }
    mode += 1;    // Step thro modes
    //encoder_val = 1;
    //paramAction(UPDATE, MODE); // Mode param //paramAction(UPDATE, mode, NULL, F("Mode"), mode_label, 0, _N(mode_label), true);
//#define MODE_CHANGE_RESETS  1
#ifdef MODE_CHANGE_RESETS
    if (mode != CW) stepsize = STEP_1k; else stepsize = STEP_500; // sets suitable stepsize
#endif
    if (mode > CW) mode = LSB;  // skip all other modes (only LSB, USB, CW)
#ifdef MODE_CHANGE_RESETS
    if (mode == CW) { filt = 4; nr = 0; }
    else filt = 0;  // resets filter (to most BW) and NR on mode change
#else
    if (mode == CW) { nr = 0; }
    prev_stepsize[prev_mode == CW] = stepsize; stepsize = prev_stepsize[mode == CW]; // backup stepsize setting for previous mode, restore previous stepsize setting for current selected mode; filter settings captured for either CQ or other modes.
    prev_filt[prev_mode == CW] = filt; filt = prev_filt[mode == CW];  // backup filter setting for previous mode, restore previous filter setting for current selected mode; filter settings captured for either CQ or other modes.
#endif
    //paramAction(UPDATE, MODE);
    vfomode[vfosel % 2] = mode;
    paramAction(SAVE, (vfosel % 2) ? MODEB : MODEA);  // save vfoa/b changes
    paramAction(SAVE, MODE);
    paramAction(SAVE, FILTER);
    si5351.iqmsa = 0;  // enforce PLL reset
    if ((prev_mode == CW) && (cwdec))
      show_banner();
    change = true;
  }
  else
  {
    if (menumode == 1) { menumode = 0; }  // short right-click while in menu: enter value selection screen
    if (menumode >= 2) { menumode = 1; change = true; paramAction(SAVE, menu); } // short right-click while in value selection screen: save, and return to menu screen
  }
  return 1;
}
*/

