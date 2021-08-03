#include "dac.h"
#include "iodefine.h" 
#include "sinewavedata.h"

unsigned short ampl=0; //amplituda na wyjściu
unsigned char index=0; //index próbki
unsigned char indexNext=0; //index pomocniczy dla sinusa dla funkcji nextSample()
unsigned char triangle=0; //wybór trójkąt/sinus


unsigned short nextSample(void) //funkcja odwracająca pierwszą ćwiartkę sinusa na pozostałe
{
	if(index<64)
		return sinewave_data[indexNext++];
	
	else if(index<128)
		return sinewave_data[indexNext--];
	
	else if(index<192)
		return 255-sinewave_data[indexNext++];

	else
		return 255-sinewave_data[indexNext--];
}


void sinOn(void) //ustawienie flagi dla sinusa
{
	triangle=0;
}

void triOn(void) //ustawienie flagi dla trójkąta
{
	triangle=1;
}

void amPl(unsigned short a) //przekazywanie wartości wzmocnienia
{
	ampl=a;
}

void dataOut(unsigned char d) //wysłanie jednego bajtu
{
	int n;
	for(n = 0; n < 8; n++){
		if(d & 0x80){ //maska na najbardziej znaczący bit i jego wystawienie
			DAC_DATA_ON();
		}else{
			DAC_DATA_OFF();
		}
		
		DAC_CLOCK_ON(); //
		DAC_CLOCK_OFF(); //takt zegarowy na SPI
		d <<= 1; //przesunięcie danych
	}
}	

void dacOut(void) //funkcja obsługi DACa
{
	//////WYPROWADZENIE WYJSCIA Z REJESTRU NA WYJ

	DAC_LDAC_OFF();
	DAC_LDAC_ON(); //takt na LDAC, występuje na początku funkcji dla uzyskania stałego odstępu czasu
	
	unsigned char Data11, Data12; //dwa kolejno wysyłane bajty SPI, zmienne tymczasowe

	unsigned short data; //wartość aktualnie wysyłanej próbki
	
	
	if(triangle)
		data=(ampl*nextSample()+(100-ampl)*127)/100; //wystawienie na wyjście wartości dla sinusa

	else 
		data=((ampl*index++) + (100-ampl)*127)/100; //index 0-255 przyrost tej zmiennej odpowiada przebiegowy o kształcie piła
	
	//////// w powyższych obliczanie wartości amplitudy w zależności od zadanych procentów oraz przesunięcie o składową stałą
	

	Data11 = 0xb0+((data>>4)&0x0f); //0xb0 konfiguracja DAC, pozostałe część danych
	Data12 = data<<4 & 0xf0; //rozłożenie danych ze zmiennej data, przetwornik akceptuje dane 12 bitowe

	DAC_CE_OFF();
	
	dataOut(Data11); //wystawienie 2 bajtów na SPI
	dataOut(Data12);
	
	DAC_CE_ON();

}