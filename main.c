/****************************************************************
KPIT Cummins Infosystems Ltd, Pune, India. - 10-Mar-2003.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
*****************************************************************/

/*****************************************************************


*****************************************************************/

#include "iodefine.h"
#include "lcd.h"
#include "inline.h"
#include "dac.h"
#define klawiatura_Obszar 0x20000
#define klawiatura_bufor (*(volatile unsigned char*)(klawiatura_Obszar+1))


short count; //licznik do odmierzania opóźnień w pętli głównej

short czestotliwosc_amplitudy = 400, fzad=5, dzad=10, fzadP, amplituda=0;

char i;
char stan_pracy=1;
char stan_pracy_sin_pila=1;
short przetwornik; 


void Ekran(void)
{
	GotoXY(1,1);
	if(stan_pracy_sin_pila) putstr("SIN ");
	else putstr("PILA ");
	
	GotoXY(1,7);
	putstr("A=");
	piszC(amplituda);
	
	GotoXY(2,1);
	if(stan_pracy) putstr("START");
	else putstr("STOP ");
	
	GotoXY(2,6);
	putstr(" F=");
	piszC(czestotliwosc_amplitudy);
	putstr("Hz");

}

void putstr(char * x) //funkcja mająca na celu wysłanie na LCD kolejnych znaków stringa
{	
	do
	{
		putchar(*x);
		x++;
	} while(*x);
	
}

char dz, jed, se,ty;
void piszC(unsigned short x) //funkcja wyświetlająca liczby całkowite do tysięcy
{	
	if(x/1000)
		putchar(x/1000+'0');
		else putchar(' ');
	putchar(x%1000/100+'0');
	putchar('.');
	putchar(x%1000%100/10+'0');
	putchar(x%1000%100%10+'0');
}


void init_AC(char kan) //Inicjalizacja ADC dla potencjometru, kanał jako argument
{
	AD.ADCSR.BIT.ADF=0;
	AD.ADCSR.BIT.ADIE=0;
	AD.ADCSR.BIT.SCAN=1;
	AD.ADCSR.BIT.CKS=0;
	AD.ADCSR.BIT.CH=kan;
	AD.ADCSR.BIT.ADST=1;
}


short odczyt_AC(char kan) //odczyt ADC dla wybranego kanału
{
	if(AD.ADCSR.BIT.ADF)
	{
		if(kan==0) return AD.ADDRA>>6;
		if(kan==1) return AD.ADDRB>>6;
		if(kan==2) return AD.ADDRC>>6;
		if(kan==3) return AD.ADDRD>>6;
	}
	return 0;
}

void odczytaj_klawiature(void) //funkcja obsługi klawiatury
{	
	if(klawiatura_bufor==0x7b) //przycisk 2
	{
		stan_pracy=0;

		amPl(0); //wyzerowanie amplitudy tj wartość 127 na DAC, gdy zarządano zatrzymania
	}
	if(klawiatura_bufor==0x77) //przycisk 1
	{
		stan_pracy=1;

		amPl(amplituda); //powrót do normalnej pracy
	}
	
		if(klawiatura_bufor==0x7e) //przycisk 4 
		{
			if (czestotliwosc_amplitudy == 400)
			{
				czestotliwosc_amplitudy = 600;
				ITU1.GRA=1302; //wartość dla preskalera /8 16MHz 256 próbek sinusa -> 6Hz
			}
			else
			{
				czestotliwosc_amplitudy = 400;
				ITU1.GRA=1953; //to samo 4Hz
			}
		}
		if(klawiatura_bufor==0x7d) //przycisk 3
		{
			if (stan_pracy_sin_pila==1)
			{
				stan_pracy_sin_pila=0;
				sinOn(); //wywołanie funkcji aktywującej sinus na wyjściu DAC
			}
			else
			{
				stan_pracy_sin_pila=1;
				triOn(); //trójkąt na wyjściu DAC
			}
		}
		
}


void initTimers(void)
{
	//odmierzanie czasu opóźnień w main
	ITU.TSTR.BIT.STR0=0;
	ITU0.TCR.BYTE=0xA3;
	ITU0.GRA=1999; //1kHz
	ITU0.TIOR.BYTE=0x88;
	ITU0.TIER.BYTE=0xF8;
	ITU.TSTR.BIT.STR0=1;	
	
	//przerwanie  SPI
	ITU.TSTR.BIT.STR1=0;
	ITU1.TCR.BYTE=0xA3;
	ITU1.GRA=1302; //początkowa wartość dla sinusa 6Hz
	ITU1.TIOR.BYTE=0x88;
	ITU1.TIER.BYTE=0xF9; //+ przerwanie INT_IMIA1
	ITU.TSTR.BIT.STR1=1;
}
//------------ wlasciwy program -----------------------------------

int main (void)
{
	
	IniLCD();
	Ekran();

	init_AC(2);
	initTimers();
		
	count=1000;
	
	while(1)						//petla gl
	{
		GotoXY(2,1);
			
		
		if(ITU0.TSR.BIT.IMFA) //1kHz 1ms
		{
			
			count++;	
			ITU0.TSR.BIT.IMFA=0;
		
		if(!(count%10)) //10ms miejsce zerowania ADC
		{

				for(i=0; i<20;i++)
					przetwornik+=odczyt_AC(2); //sumowanie

			amplituda=przetwornik/20; //obliczanie wartości średniej z 20 odczytów	

			amplituda=((long)amplituda*101)/1023; //przeliczenie wyniku ADC na wartość z zakresu 0-100(%)
			przetwornik=0;

			if(stan_pracy) amPl(amplituda); //sprawdzenie czy zażądano amplitudy na wyjściu i przekazanie jej wartość

			Ekran();		
		}	
		
		if(!(count%100)) //100ms odczyt klawiatury, odbywa się wolniej niż powyższy odczyt ADC
		{
			odczytaj_klawiature();
		}
		
		if(count>1000) //1000mszerowanie licznika szablonu czasowego 
		{
			count=0;
		}

	}
}
	

