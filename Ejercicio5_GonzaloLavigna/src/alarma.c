#include "sapi.h"
#include "PCLink.h"
#include "debounceTec.h"
#include "leds.h"
#include "alarma.h"
#include "string.h"
DEBUG_PRINT_ENABLE

#define INTRUDER_BLINK_PERIOD 500 //en ms


static uint8_t alarmaPasswordArray[] = {"1257"};
#define PASSWORD_LENGTH (sizeof(alarmaPasswordArray)/sizeof(uint8_t)) -1
static uint8_t line_reception_buffer[20];

#define NUMBER_OF_SENSORS 4
static bool_t sensoresCorruptos[NUMBER_OF_SENSORS];
typedef enum {
	PUERTA_INDICE = 0,
	VENTANA_1_INDICE,
	VENTANA_2_INDICE,
	VENTANA_3_INDICE
}sensoresCorruptosIndex_t;


typedef struct {
	fsmAlarmaState_t state;
	uint8_t * alarmaPasswordPointer;
	bool_t nuevoEstado;
	uint32_t fsmCallCountInState;
} fsmAlarmaData_t;

static fsmAlarmaData_t alarmaData;

static void alarmaCheckNewState(void);
static void alarmaSetIntruderActuatorConfiguration(bool_t actuator_configuration);
static void alarmaSetDisarmedActuatorConfiguration(bool_t actuator_configuration);
static void alarmaSetWaitForPasswordActuatorConfiguration (bool_t actuator_configuration);
static void alarmaSetArmedActuatorConfiguration(bool_t actuator_configuration);
static void alarmaSetArmingActuatorConfiguration(bool_t actuator_configuration);

static bool_t alarmaCheckDoorSensor(void);
static bool_t alarmaCheckWindowsSensors(void);

static bool_t alarmaGetAlarmPassword (void);

static void alarmaClearSensorsStatus (void);
static void alarmaSetSensorStatus(sensoresCorruptosIndex_t sensores_corruptos, bool_t sensor_status);
static void alarmaSetActuatorsSensorStatus (bool_t actuator_configuration);

void alarmaInit (void) {
	alarmaData.alarmaPasswordPointer = &alarmaPasswordArray;
	alarmaData.nuevoEstado = TRUE;
	alarmaData.state = ALARM_DISARMED;
	alarmaData.fsmCallCountInState = 0;
	alarmaClearSensorsStatus();
}

void alarmaUpdate (void){
	alarmaData.fsmCallCountInState ++;

	switch (alarmaData.state){
	case ALARM_DISARMED:
		alarmaCheckNewState();
		alarmaClearSensorsStatus();
		alarmaSetIntruderActuatorConfiguration(FALSE);
		alarmaSetDisarmedActuatorConfiguration(TRUE);
		if(alarmaGetAlarmPassword()==TRUE){
			alarmaData.state = ALARM_ARMING;
			alarmaData.nuevoEstado = TRUE;
			alarmaData.fsmCallCountInState = 0;
			alarmaSetDisarmedActuatorConfiguration(FALSE);
		}
		break;
	case ALARM_ARMING:
		if(alarmaData.nuevoEstado ==TRUE)
			alarmaSetArmingActuatorConfiguration(TRUE);
		alarmaCheckNewState();
		if(alarmaData.fsmCallCountInState > ALARMA_ARMING_TICK_LAPSE){
			alarmaData.state = ALARM_ARMED;
			alarmaData.nuevoEstado = TRUE;
			alarmaData.fsmCallCountInState = 0;
			alarmaSetArmingActuatorConfiguration(FALSE);
		}
		if(alarmaGetAlarmPassword()==TRUE){
			alarmaData.state = ALARM_DISARMED;
			alarmaData.nuevoEstado = TRUE;
			alarmaData.fsmCallCountInState = 0;
			alarmaSetArmingActuatorConfiguration(FALSE);
		}
		break;
	case ALARM_ARMED:
		alarmaCheckNewState();
		alarmaSetArmedActuatorConfiguration(TRUE);
		if(alarmaCheckDoorSensor() == FALSE){
			alarmaData.state = ALARM_DISARMING;
			alarmaData.nuevoEstado = TRUE;
			alarmaData.fsmCallCountInState = 0;
			alarmaSetArmedActuatorConfiguration(FALSE);
		}
		if(alarmaCheckWindowsSensors() == FALSE ){
			alarmaData.state = ALARM_INTRUDER;
			alarmaData.nuevoEstado = TRUE;
			alarmaData.fsmCallCountInState = 0;
			alarmaSetArmedActuatorConfiguration(FALSE);
		}
		if(alarmaGetAlarmPassword()==TRUE){
			alarmaData.state = ALARM_DISARMED;
			alarmaData.nuevoEstado = TRUE;
			alarmaData.fsmCallCountInState = 0;
			alarmaSetArmedActuatorConfiguration(FALSE);
		}
		break;
	case ALARM_DISARMING:
		alarmaCheckNewState();
		alarmaSetWaitForPasswordActuatorConfiguration(TRUE);
		if(alarmaData.fsmCallCountInState > ALARMA_INTRUDER_TICK_LAPSE){
			alarmaData.state = ALARM_INTRUDER;
			alarmaData.nuevoEstado = TRUE;
			alarmaData.fsmCallCountInState = 0;
			alarmaSetWaitForPasswordActuatorConfiguration(FALSE);
		}
		if(alarmaCheckWindowsSensors() == FALSE ){
			alarmaData.state = ALARM_INTRUDER;
			alarmaData.nuevoEstado = TRUE;
			alarmaData.fsmCallCountInState = 0;
			alarmaSetWaitForPasswordActuatorConfiguration(FALSE);
		}
		if(alarmaGetAlarmPassword()==TRUE){
			alarmaData.state = ALARM_DISARMED;
			alarmaData.nuevoEstado = TRUE;
			alarmaData.fsmCallCountInState = 0;
			alarmaSetWaitForPasswordActuatorConfiguration(FALSE);
		}
		break;
	case ALARM_INTRUDER:
		if(alarmaData.nuevoEstado == TRUE){
			alarmaSetIntruderActuatorConfiguration(TRUE);
		}
		alarmaSetActuatorsSensorStatus(TRUE);
		alarmaCheckNewState();
		if(alarmaGetAlarmPassword()==TRUE){
			alarmaData.state = ALARM_DISARMED;
			alarmaData.nuevoEstado = TRUE;
			alarmaData.fsmCallCountInState = 0;
			alarmaSetIntruderActuatorConfiguration(TRUE);
			alarmaSetActuatorsSensorStatus(FALSE);
		}
		break;
	default:
		alarmaCheckNewState();
		alarmaData.state = ALARM_DISARMED;
		alarmaData.nuevoEstado = TRUE;
		alarmaData.fsmCallCountInState = 0;
		debugPrintlnString("MODULO ALARMA ESTADO INCORRECTO");
		break;
	}
}

static void alarmaCheckNewState(void){
	if(alarmaData.nuevoEstado == TRUE){
		switch (alarmaData.state) {
			case ALARM_DISARMED:	PC_LINK_Write_String_To_Buffer_ln("ALARMA ESTADO DESARMADA");	break;
			case ALARM_ARMING: 		PC_LINK_Write_String_To_Buffer_ln("ALARMA ESTADO ARMANDO"); 	break;
			case ALARM_ARMED: 		PC_LINK_Write_String_To_Buffer_ln("ALARMA ESTADO ARMADA"); 		break;
			case ALARM_DISARMING: 	PC_LINK_Write_String_To_Buffer_ln("ALARMA ESTADO DISARMING"); 	break;
			case ALARM_INTRUDER: 	PC_LINK_Write_String_To_Buffer_ln("ALARMA ESTADO INTRUDER"); 	break;
			default: PC_LINK_Write_String_To_Buffer_ln("ESTADO ALARMA INCORRECTO"); break;
		}
		alarmaData.nuevoEstado = FALSE;
	}
}

static void alarmaSetIntruderActuatorConfiguration(uint8_t actuator_configuration){
	if(actuator_configuration == TRUE){
		setLedConfiguration(LED_RED_INDICE,LED_BLINK,INTRUDER_BLINK_PERIOD);
	}
	else{
		setLedConfiguration(LED_RED_INDICE,LED_BI_LEVEL_LOW,INTRUDER_BLINK_PERIOD);
	}
}

static void alarmaSetDisarmedActuatorConfiguration(bool_t actuator_configuration){
	if(actuator_configuration == TRUE){
		setLedConfiguration(LED_GREEN_INDICE,LED_BI_LEVEL_HIGH,INTRUDER_BLINK_PERIOD);
	}
	else{
		setLedConfiguration(LED_GREEN_INDICE,LED_BI_LEVEL_LOW,INTRUDER_BLINK_PERIOD);
	}
}

static void alarmaSetWaitForPasswordActuatorConfiguration (bool_t actuator_configuration){
	if(actuator_configuration == TRUE){
		setLedConfiguration(LED_BLUE_INDICE,LED_BI_LEVEL_HIGH,INTRUDER_BLINK_PERIOD);
	}
	else{
		setLedConfiguration(LED_BLUE_INDICE,LED_BI_LEVEL_LOW,INTRUDER_BLINK_PERIOD);
	}
}

static void alarmaSetArmedActuatorConfiguration(bool_t actuator_configuration){
	if(actuator_configuration == TRUE){
		setLedConfiguration(LED_RED_INDICE,LED_BI_LEVEL_HIGH,INTRUDER_BLINK_PERIOD);
	}
	else{
		setLedConfiguration(LED_RED_INDICE,LED_BI_LEVEL_LOW,INTRUDER_BLINK_PERIOD);
	}
}

static void alarmaSetArmingActuatorConfiguration(bool_t actuator_configuration){
	if(actuator_configuration == TRUE){
		setLedConfiguration(LED_GREEN_INDICE,LED_BLINK,INTRUDER_BLINK_PERIOD);
	}
	else{
		setLedConfiguration(LED_GREEN_INDICE,LED_BI_LEVEL_LOW,INTRUDER_BLINK_PERIOD);
	}
}


static bool_t alarmaGetAlarmPassword (void){
	uint32_t line_receive_length;
	line_receive_length = PC_LINK_Find_Line_From_Buffer();
	if(line_receive_length != LINE_NOT_FOUND){
		PC_LINK_Get_String_From_Buffer(line_reception_buffer,line_receive_length);
		PC_LINK_Write_String_To_Buffer_ln("LINEA RECIBIDA");
		if(strncmp(line_reception_buffer,alarmaData.alarmaPasswordPointer,PASSWORD_LENGTH)==0){
			PC_LINK_Write_String_To_Buffer_ln("PASSWORD CORRECTO");
			return TRUE;
		}
		else {
			PC_LINK_Write_String_To_Buffer_ln("PASSWORD INCORRECTO");
			return FALSE;
		}
	}
	return FALSE;
}

static bool_t alarmaCheckDoorSensor(void){
	if(getTeclaState(TEC1_INDICE)==BUTTON_DOWN){
		PC_LINK_Write_String_To_Buffer_ln("PUERTA ABIERTA");
		return FALSE;
	}
	return TRUE;
}
static bool_t alarmaCheckWindowsSensors(void){
	bool_t windowsEventFlag = TRUE;
	if(getTeclaState(TEC2_INDICE)==BUTTON_DOWN){
		PC_LINK_Write_String_To_Buffer_ln("VENTANA 1 ABIERTA");
		alarmaSetSensorStatus(VENTANA_1_INDICE,TRUE);
		windowsEventFlag = FALSE;
	}
	if(getTeclaState(TEC3_INDICE)==BUTTON_DOWN){
		PC_LINK_Write_String_To_Buffer_ln("VENTANA 2 ABIERTA");
		alarmaSetSensorStatus(VENTANA_2_INDICE,TRUE);
		windowsEventFlag = FALSE;
	}
	if(getTeclaState(TEC4_INDICE)==BUTTON_DOWN){
		PC_LINK_Write_String_To_Buffer_ln("VENTANA 3 ABIERTA");
		alarmaSetSensorStatus(VENTANA_3_INDICE,TRUE);
		windowsEventFlag = FALSE;
	}
	return windowsEventFlag;
}


static void alarmaClearSensorsStatus (void){
	uint32_t i = 0;
	for (i=0;i<NUMBER_OF_SENSORS;i++)
		sensoresCorruptos[i] = FALSE;
}
static void alarmaSetSensorStatus(sensoresCorruptosIndex_t indice_sensores_corruptos, bool_t sensor_status){
	sensoresCorruptos[indice_sensores_corruptos] = sensor_status;
}

static void alarmaSetActuatorsSensorStatus (bool_t actuator_configuration){
	if(actuator_configuration == TRUE){
		sensoresCorruptos[VENTANA_1_INDICE]==TRUE ? setLedConfiguration(LED_1_INDICE,LED_BI_LEVEL_HIGH,INTRUDER_BLINK_PERIOD):setLedConfiguration(LED_1_INDICE,LED_BI_LEVEL_LOW,INTRUDER_BLINK_PERIOD);
		sensoresCorruptos[VENTANA_2_INDICE]==TRUE ? setLedConfiguration(LED_2_INDICE,LED_BI_LEVEL_HIGH,INTRUDER_BLINK_PERIOD):setLedConfiguration(LED_2_INDICE,LED_BI_LEVEL_LOW,INTRUDER_BLINK_PERIOD);
		sensoresCorruptos[VENTANA_3_INDICE]==TRUE ? setLedConfiguration(LED_3_INDICE,LED_BI_LEVEL_HIGH,INTRUDER_BLINK_PERIOD):setLedConfiguration(LED_3_INDICE,LED_BI_LEVEL_LOW,INTRUDER_BLINK_PERIOD);
	}
	else{
		setLedConfiguration(LED_1_INDICE,LED_BI_LEVEL_LOW,INTRUDER_BLINK_PERIOD);
		setLedConfiguration(LED_2_INDICE,LED_BI_LEVEL_LOW,INTRUDER_BLINK_PERIOD);
		setLedConfiguration(LED_3_INDICE,LED_BI_LEVEL_LOW,INTRUDER_BLINK_PERIOD);
	}
}

