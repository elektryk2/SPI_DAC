//definicje konkretnych wyjść na SPI mające poprawić czytelnosć kodu
#define DAC_DATA_ON()   PA.DR.BIT.B5=1;   
#define DAC_DATA_OFF()  PA.DR.BIT.B5=0;   

#define	DAC_CLOCK_ON()  PA.DR.BIT.B4=1;	  
#define DAC_CLOCK_OFF() PA.DR.BIT.B4=0; 

#define DAC_CE_ON()     PA.DR.BIT.B3=1;   
#define DAC_CE_OFF()    PA.DR.BIT.B3=0; 

#define	DAC_LDAC_ON()  PA.DR.BIT.B2=1;	  
#define DAC_LDAC_OFF() PA.DR.BIT.B2=0;   
    

//prototypy mające na celu komunikację z programem obsługi DACa tutaj wywoływane z poziomu main oraz jednego z przerwań
void amPl(unsigned short a) ;
void dacOut(void);
void DataOut(unsigned char d);
void triOn(void);
void sinOn(void);
void stopSign(unsigned char s);

