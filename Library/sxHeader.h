// בס"ד 
// בשם ה' נעשה ונצליח

//////////////////////////////////////////////////////////
////////////				header.h		//////////////
//////////////////////////////////////////////////////////
// <******************************************************>


////////////			INSTRUCTIONS		//////////////
// <--------------------	Start	------------------->
/*

1. QUICK MODE:	In Quick mode you can send a message with default settings:
				Bandwidth = 125 kHz; Coding Rate = 4/5; Spreading Factor = 8; Implicit Header Mode On;
				To enter this mode, change the value of QUICK_MODE to 1.

					____   ___     __    ___
					 |    |   | _ |  \  |   |
					 |    |___|   |__/  |___|



*/




#include <SPI.h>

// <--------------------	Start	------------------->
////////////////////	Bandwidth		////////////////
const uint8_t BW7_8		=	0x00;
const uint8_t BW10_4		=	0x10;
const uint8_t BW15_6		=	0x20;
const uint8_t BW20_8		=	0x30;
const uint8_t BW31_25		=	0x40;
const uint8_t BW41_7		=	0x50;
const uint8_t BW62_5		=	0x60;
const uint8_t BW125		=	0x70;
const uint8_t BW250		=	0x80;
const uint8_t BW500		=	0x90;
// <********************	END	***********************>


// <--------------------	Start	------------------->
///////////////////	Spreading Factor	////////////////
const uint8_t SF6		=	6;
const uint8_t SF7		=	7;
const uint8_t SF8		=	8;
const uint8_t SF9		=	9;
const uint8_t SF10		=	10;
const uint8_t SF11		=	11;
const uint8_t SF12		=	12;
// <********************	END	***********************>


// <--------------------	Start	------------------->
/////////////////////	Coding Rate	////////////////////
const uint8_t CR1		=	0x02;		// 4/5
const uint8_t CR2		=	0x04;		// 4/6
const uint8_t CR3		=	0x06;		// 4/7
const uint8_t CR4		=	0x08;		// 4/8
// <********************	END	***********************>


// <--------------------	Start	------------------->
///////////				Register's Names:		////////
///////////   		(There are 75 Registers,	////////
///////////			 and LoRa uses 54 of them)	////////
const uint8_t RegOpMode			=	0x01;
const uint8_t RegFrMsb				=	0x06;
const uint8_t RegFrMid				=	0x07;
const uint8_t RegFrLsb				=	0x08;
const uint8_t RegModemConfig1		=	0x1D;
const uint8_t RegModemConfig2		=	0x1E;
const uint8_t RegFifoTxBaseAddr		=	0x0E;
const uint8_t RegFifoAddrPtr		=	0x0D;
const uint8_t RegFifoRxBaseAddr		=	0x0F;
const uint8_t RegPayloadLength		=	0x22;
const uint8_t RegRxNbBytes			=	0x13;
const uint8_t RegIrqFlagsMask		=	0x11;		// To investigate
const uint8_t RegIrqFlags			=	0x12;		// To investigate
const uint8_t RegPaConfig			=	0x09;		// To investigate
const uint8_t RegPktRssiValue		=	0x1A;		// To investigate
const uint8_t RegPktSnrValue		=	0x19;		// To investigate
// <********************	END	***********************>


// <--------------------	Start	------------------->
////////////////////	Constants		////////////////
#define FSK_SLEEP				0x00
#define FSK_STANDBY				0x01
#define LORA_SLEEP				0x80
#define LORA_STANDBY			0x81
#define LORA_TRANSMIT			0x83
#define LORA_RXCONTINUOUS		0x85
#define CRC_ON					1
#define EXPLICIT				0
#define IMPLICIT				1
// <********************	END	***********************>


// *********************** LoRa ********************\\

struct package{
	uint8_t start;
	uint8_t end;
	uint8_t type;
	uint8_t destination;
	uint8_t source;
	uint8_t timeout;
	uint8_t power;
};

class lora {
	public:
	
					// Class Variables START
	uint8_t _loraNSS = 1;
	uint8_t _loraReset = 2;
	uint8_t _myAddress = 0;
	uint8_t _BW;
	uint8_t _CR;
	uint8_t _SF;
	uint8_t _headerMode;
	uint8_t _packFlag;
	uint8_t _CRC;
					// Class Variables END
	
	// Constructor.
	lora();
	lora(uint8_t loraNSS, uint8_t loraReset, uint8_t myAddress): _loraNSS(loraNSS), _loraReset(loraReset), _myAddress(myAddress){}

	// Start operation of the chip.
	void Begin();	
	
	// Set modem as LoRa
	void loraOn();
	
	// Resets the chip.
	void Reset();
	
	// Write to register.
	void writeToReg(uint8_t regAdd, uint8_t data);
	
	// Read from register
	// Return: The data received from the register.
	uint8_t readFromReg(uint8_t regAdd);
	
	// getBit
	// Get the value of a selected bit.
	// Parameters:	1. num:		bit index
	//				2. regAdd:	The selected register
	// Return: The value of the bit (0 or 1).
	uint8_t getBit(uint8_t regAdd, uint8_t num);
	
	// setBit
	// Set a value to a specific bit.
	// Only 0 or 1
	// Parameters:	1. num:		bit index
	//				2. value:	0 or 1
	//				3. regAdd:	The selected register
	void setBit(uint8_t num, uint8_t value, uint8_t regAdd);
	
	// Check if the device is responding.
	// Return: 1 for success and 0 for failure.
	// Attention: For proper operation, call Reset() before.
	//			  After a reset, you need to call loraOn() again to activate LoRa.
	bool Check();
	
	// Make Lora go to sleep.
	void Sleep();
	
	// Make lora enter Standby mode.
	void Standby();
	
	// modemConfig
	// Parameters:	1. BW:			Bandwidth
	//				2. CR:			Coding Rate.
	//				3. SF:			Spreading Factor.
	//				4. headerMode:	0 for Explicit, 1 for Implicit.
	void modemConfig(uint8_t BW, uint8_t CR, uint8_t SF, uint8_t headerMode);
	
	// setFreq
	// Set the modem frequency
	// Parameters:	1. float freq:	The frequency (in MHz).
	void setFreq(float freq);
	
	// setCRC()
	// Parameters: 1 to enable crc, 0 to disable
//	void setCRC(uint8_t status);
	
	// buildPacket
	// Building a struct with package details to resend
	void buildPackage(uint8_t start, uint8_t end, uint8_t type, uint8_t destination, uint8_t source, uint8_t timeout, uint8_t power);
	
	// Send Packet
	// Parametes:	1. pctStart:		Buffer location address for start.
	//				2. pctEnd:			Buffer location address for end.
	//				3. pctType:			Package type.
	//				4. pctDestination:	Destination node.
	//				5. pctSource:		Source node.
	//				6. pctTimeout:		Transmission timeout (ms).
	//				7. pctPower:		Transmitter power (dBm). Between 2 to 17.
	void Send(uint8_t pctStart, uint8_t pctEnd, uint8_t pctType, uint8_t pctDestination, long pctTimeout, uint8_t Power);
	
	// Send Packet - Implicit Mode
	// In this case, we don't have an header.
	// For that reason, the payload length, error coding and CRC should
	// be configured manually on both sides (transmitter and receiver).
	// Parameters:	1. pctLength:		Payload size.
	//				2. CR:				Coding rate.
	//				3. CRCStatus:		0 to disable, 1 to enable.
	void SendImplicit(uint8_t pctLength, uint8_t pctCR, uint8_t pctCRCStatus);
	
	// Send Packet - Explicit Mode
	// In this case, we have an header that includes:
	// 1. The payload length in bytes.
	// 2. The forward error correction code rate.
	// 3. The presence of an optional 16-bits CRC for the payload.
	void SendExplicit(uint8_t pctLength, uint8_t pctCR, uint8_t pctCRCStatus);
	
	
	// Receive Packet
	// Parameters:	1. timeout:	Timeout (in mS).
	void Receive(int timeout);
	
	// Send an ack
	void sendAck(uint8_t sourceAddress, uint8_t myAddress);
	
	// Receive an ack
	bool receiveAck(int timeout);
	
	// A function that let the user write and send a string.
	void sendStr();



							// Temporary START
			// Filling TX
			void Send2(uint8_t pctStart, uint8_t pctEnd, uint8_t pctType, uint8_t pctDestination, long pctTimeout, uint8_t Power, int ackTimeout);
			void Receive2(int timeout);
			void fillTX();
			void simpleSend(uint8_t mpctStart, uint8_t mpctEnd, uint8_t mpctType);
			void advanceSend(uint8_t pctStart, uint8_t pctEnd, uint8_t pctType, uint8_t pctDestination, uint8_t pctSource, long pctTimeout, uint8_t Power, bool flags);
			void myGet();
			void setTXBuffer(char message[]);
			void fillBuffer(char message2[], uint8_t msgLength);
							// Temporary END
							
};

/*
TO DO: an option to activate lora with default settings or
		  manually choose prefered settings. Setting your on settings
		  might be from constructor.

*/


// *********************** FSK ********************\\

/*
class FO {
	public:
	
	// Start operation of the chip.
	void begin();
	
	// Resets the chip.
	void Reset();
	
	// Write to register
	// Return: 1 for success and 0 for failure.
	void writeToReg(byte regAdd, byte data);
	
	// Read from register
	// Return: The data received from the register.
	byte readFromReg(byte regAdd);
	
	// Check if the lora modem responds.
	// Return: 1 for success and 0 for failure.
	bool Check();
};
*/