/* ########################################################################

   PICsimLab - PIC laboratory simulator

   ########################################################################

   Copyright (c) : 2010-2020  Luis Claudio Gambôa Lopes

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

   For e-mail suggestions :  lcgamboa@yahoo.com
   ######################################################################## */


#include "board_picsim.h"

#include"../picsimlab1.h"

void
board_picsim::MSetSerial(const char * port)
{
 pic_set_serial (&pic, 0, port, 0, 0, 0);
 pic_set_serial (&pic, 1, "", 0, 0, 0);
}

int
board_picsim::MInit(const char * processor, const char * fname, float freq)
{
 lxString sproc = GetSupportedDevices ();
 int procn = 0;

 if (sproc.Contains (processor))
  {
   procn = getprocbyname (processor);
  }

 if (procn == 0)
  {
   printf ("PICSimLab: Unknown processor %s ! Loading Default\n", processor);
   int i = sproc.find (lxT ("PIC"));
   sproc = sproc.substr (i, sproc.length ());
   i = sproc.find (lxT (","));
   Proc = sproc.substr (0, i);
   procn = getprocbyname (Proc.c_str ());
  }

 int ret = pic_init (&pic, procn, fname, 1, freq);

 //disable DEBUG

 if ((pic.processor == getprocbyname ("PIC16F1619")) ||
     (pic.processor == getprocbyname ("PIC16F1788")) ||
     (pic.processor == getprocbyname ("PIC16F1789")) ||
     (pic.processor == getprocbyname ("PIC16F1939")))
  {

   pic.config[1] |= 0x0100;
  }
 else if ((pic.processor == getprocbyname ("PIC16F18324")) ||
          (pic.processor == getprocbyname ("PIC16F18855")))
  {
   pic.config[1] |= 0x0200;
  }
 else if ((pic.processor == getprocbyname ("PIC16F628A")) ||
          (pic.processor == getprocbyname ("PIC16F648A")) ||
          (pic.processor == getprocbyname ("PIC16F84A")) ||
          (pic.processor == getprocbyname ("PIC16F777")) ||
          (pic.processor == getprocbyname ("PIC16F877A")))
  {

   pic.config[0] |= 0x0800;
  }
 else if ((pic.processor == getprocbyname ("PIC18F452")) ||
          (pic.processor == getprocbyname ("PIC18F4520")) ||
          (pic.processor == getprocbyname ("PIC18F4550")) ||
          (pic.processor == getprocbyname ("PIC18F45K50")) ||
          (pic.processor == getprocbyname ("PIC18F4620")))
  {
   pic.config[3] |= 0x0080;
  }
 else if ((pic.processor == getprocbyname ("PIC18F27K40")) ||
          (pic.processor == getprocbyname ("PIC18F47K40")))
  {
   pic.config[1] |= 0x2000;
  }
  else if ((pic.processor == getprocbyname ("PIC18F46J50"))) 
  {
   pic.config[0] |= 0x0080;
  }
 else
  {
   printf ("PICSimLab: PIC 0x%04X not supported in picsimlab!!\n", pic.processor);
   exit (-1);
  }

 pic.pins = (picpin*) realloc (pic.pins, sizeof (picpin)*256);

 return ret;
}

void
board_picsim::MEnd(void)
{
 pic_end ();
 //prog_end();
 mplabxd_end ();
}

void
board_picsim::MEraseFlash(void)
{
 pic_erase_flash ();
}

void
board_picsim::MSetFreq(float freq)
{
 pic.freq = freq;
}

float
board_picsim::MGetFreq(void)
{
 return pic.freq;
}

float
board_picsim::MGetInstClock(void)
{
 return pic.freq / 4.0;
}

int
board_picsim::CpuInitialized(void)
{
 return 1;
}

void
board_picsim::MDumpMemory(const char * fname)
{
 switch (getfprocbyname ((const char*) Proc.c_str ()))
  {
  case P16:
   write_ihx (fname);
   break;
  case P16E:
   write_ihx16e (fname);
   break;
  case P18:
   write_ihx18 (fname);
   break;
  }


}

int
board_picsim::DebugInit(int dtyppe) //argument not used in picm only mplabx
{
 return !mplabxd_init (this, Window1.Get_debug_port ()) - 1;
}

void
board_picsim::DebugLoop(void)
{
 if (Window1.Get_mcupwr ())
  {
   //prog_loop(&pic);
   mplabxd_loop ();
  }
}

int
board_picsim::MGetPinCount(void)
{
 return pic.PINCOUNT;
}

lxString
board_picsim::MGetPinName(int pin)
{
 char cbuf[10];
 lxString pinname;

 pinname = getPinName (&pic, pin, cbuf);

 return pinname;
}

void
board_picsim::MSetPin(int pin, unsigned char value)
{
 pic_set_pin (pin, value);
}

void
board_picsim::MSetPinDOV(int pin, unsigned char ovalue)
{
 pic_set_pin_DOV (pin, ovalue);
}

void
board_picsim::MSetAPin(int pin, float value)
{
 pic_set_apin (pin, value);
}

unsigned char
board_picsim::MGetPin(int pin)
{
 return pic_get_pin (pin);
}

const picpin *
board_picsim::MGetPinsValues(void)
{
 return pic.pins;
}

void
board_picsim::MStep(void)
{
 pic_step ();
 if (pic.s2 == 1)pic_step ();
}

void
board_picsim::MStepResume(void)
{
 if (pic.s2 == 1)pic_step ();
}

int
board_picsim::DBGTestBP(unsigned int bp)
{
 return ((pic.pc == bp)&&(pic.s2 == 0));
}

void
board_picsim::MReset(int flags)
{
 pic_reset (flags);
}

unsigned short *
board_picsim::DBGGetProcID_p(void)
{
 return (unsigned short *) &pic.processor;
}

unsigned int
board_picsim::DBGGetPC(void)
{
 return pic.pc;
}

void
board_picsim::DBGSetPC(unsigned int pc)
{
 pic.pc = pc;
}

unsigned char *
board_picsim::DBGGetRAM_p(void)
{
 return pic.ram;
}

unsigned char *
board_picsim::DBGGetROM_p(void)
{
 return (unsigned char *) pic.prog;
}

unsigned char *
board_picsim::DBGGetCONFIG_p(void)
{
 return (unsigned char *) pic.config;
}

unsigned char *
board_picsim::DBGGetID_p(void)
{
 return (unsigned char *) pic.id;
}

unsigned char *
board_picsim::DBGGetEEPROM_p(void)
{
 return pic.eeprom;
}

unsigned int
board_picsim::DBGGetRAMSize(void)
{
 return pic.RAMSIZE;
}

unsigned int
board_picsim::DBGGetROMSize(void)
{
 return pic.ROMSIZE * 2;
}

unsigned int
board_picsim::DBGGetCONFIGSize(void)
{
 return pic.CONFIGSIZE * 2;

}

unsigned int
board_picsim::DBGGetIDSize(void)
{
 return pic.IDSIZE * 2;
}

unsigned int
board_picsim::DBGGetEEPROM_Size(void)
{
 return pic.EEPROMSIZE;
}
