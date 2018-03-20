/*============================================================================
 * Autor:
 * Licencia:
 * Fecha:
 *===========================================================================*/

/*==================[inlcusiones]============================================*/

#include "sapi.h"        // <= Biblioteca sAPI

#define	DELAY_100ms	 	1
#define	DELAY_50ms 		0
typedef enum {BLINK_RED=0,BLINK_BLUE,BLINK_GREEN} blink_led_t;


/*==================[definiciones y macros]==================================*/

/*==================[definiciones de datos internos]=========================*/

DEBUG_PRINT_ENABLE
CONSOLE_PRINT_ENABLE

/*==================[definiciones de datos externos]=========================*/

/*==================[declaraciones de funciones internas]====================*/

/*==================[declaraciones de funciones externas]====================*/

/*==================[funcion principal]======================================*/

// FUNCION PRINCIPAL, PUNTO DE ENTRADA AL PROGRAMA LUEGO DE ENCENDIDO O RESET.
int main( void ){

   // ---------- CONFIGURACIONES ------------------------------

   // Inicializar y configurar la plataforma
   boardConfig();

   // Inicializar UART_USB como salida Serial de debug
   debugPrintConfigUart( UART_USB, 115200 );
   debugPrintlnString( "DEBUG: UART_USB configurada." );
   
   // Inicializar UART_232 como salida Serial de consola
   consolePrintConfigUart( UART_232, 115200 );
   consolePrintlnString( "UART_232 configurada." );

   // Crear varias variables del tipo booleano
   bool_t tec1Value = OFF;
   bool_t tec2Value = OFF;
   bool_t tec3Value = OFF;
   bool_t tec4Value = OFF;
   bool_t ledbValue = OFF;
   bool_t blink_status =  DELAY_100ms;
   blink_led_t blink_led = BLINK_RED;

   // ---------- REPETIR POR SIEMPRE --------------------------
   while( TRUE )
   {
      /* Si se presiona TEC1, enciende el LEDR */

      // Leer pin conectado a la tecla.
      tec1Value = gpioRead( TEC1 );
      // Invertir el valor leido, pues lee un 0 (OFF) con tecla
      // presionada y 1 (ON) al liberarla.
      tec1Value = !tec1Value;
      // Escribir el valor leido en el LED correspondiente.
      //gpioWrite( LEDR, tec1Value );


      /* Si se presiona TEC2, enciende el LED1 */

      // Leer pin conectado a la tecla.
      tec2Value = gpioRead( TEC2 );
      // Invertir el valor leido, pues lee un 0 (OFF) con tecla
      // presionada y 1 (ON) al liberarla.
      tec2Value = !tec2Value;
      // Escribir el valor leido en el LED correspondiente.
      gpioWrite( LED1, tec2Value );


      /* Si se presiona TEC3, enciende el LED2 */

      // Leer pin conectado a la tecla.
      tec3Value = gpioRead( TEC3 );
      // Invertir el valor leido, pues lee un 0 (OFF) con tecla
      // presionada y 1 (ON) al liberarla.
      tec3Value = !tec3Value;
      // Escribir el valor leido en el LED correspondiente.
      gpioWrite( LED2, tec3Value );


      /* Si se presiona TEC4, enciende el LED3 */

      // Leer pin conectado a la tecla.
      tec4Value = gpioRead( TEC4 );
      // Invertir el valor leido, pues lee un 0 (OFF) con tecla
      // presionada y 1 (ON) al liberarla.
      tec4Value = !tec4Value;
      // Escribir el valor leido en el LED correspondiente.
      gpioWrite( LED3, tec4Value );


      /* SI la tecla 2 esta selecciona voy eligiendo entre
       * entre los distintos valores posible del valor blink_led */
      if(tec2Value == ON){
    	  switch(blink_led) {
    	  	  case BLINK_RED:
    	  		  blink_led = BLINK_GREEN;
    	  		  gpioWrite(LEDR,OFF);
    	  		  break;
    	  	  case BLINK_GREEN:
    	  		  blink_led = BLINK_BLUE;
    	  		  gpioWrite(LEDG,OFF);
    	  		  break;
    	  	  case BLINK_BLUE:
    	  		  blink_led = BLINK_RED;
    	  		  gpioWrite(LEDB,OFF);
    	  		  break;
    	  	  default: blink_led = BLINK_RED;
    	  }
      }
      switch(blink_led){
      	  case BLINK_RED:
      		gpioToggle( LEDR );
      		break;
      	  case BLINK_GREEN:
      		gpioToggle( LEDG );
      		break;
      	  case BLINK_BLUE:
      		gpioToggle( LEDB );
      		break;
      	  default: gpioToggle( LEDR );;

      }

      /* Mostrar por UART_USB (8N1 115200) el estado del LEDB */

      // Leer el estado del pin conectado al led
      ledbValue = gpioRead( LEDR );
      // Chequear si el valor leido es encedido
      if( ledbValue == ON ){
         // Si esta encendido mostrar por UART_USB "LEDB encendido."
         debugPrintlnString( "DEBUG: LEDB encendido." );
         consolePrintlnString( "LEDB encendido." );
         consolePrintEnter();
      } else{
         // Si esta apagado mostrar por UART_USB "LEDB apagado."
         debugPrintlnString( "DEBUG: LEDB apagado." );
         consolePrintlnString( "LEDB apagado." );
         consolePrintEnter();
      }


      /* Dependiendo si la tecla esta encendida, se cambia el estado del blink de 100 ms a 50 ms, es un toggle
       * no se si el entorno presenta una funcion que haga un toogle de una boleana
       * Tal vez pueda reemplarse esta logica por blink_status = !blink_status, pero esto solo lo ataria a dos
       * valores */
      if(tec1Value == ON){
    	  if(blink_status == DELAY_100ms){
    		  blink_status = DELAY_50ms;
    	  }
    	  else {
    		  blink_status = DELAY_100ms;
    	  }
      }

      /*Se elije entre dos valores posible de toggle, en caso de necesitar mas valores habia que
       * hacer un enum con switch case*/
     blink_status == DELAY_100ms ? delay( 100 ) : delay(50);
   }

   // NO DEBE LLEGAR NUNCA AQUI, debido a que a este programa se ejecuta
   // directamenteno sobre un microcontroladore y no es llamado por ningun
   // Sistema Operativo, como en el caso de un programa para PC.
   return 0;
}

/*==================[definiciones de funciones internas]=====================*/

/*==================[definiciones de funciones externas]=====================*/

/*==================[fin del archivo]========================================*/
