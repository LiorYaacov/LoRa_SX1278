// בס"ד 
// בשם ה' נעשה ונצליח

//////////////////////////////////////////////////////////
////////////			sx1278.cpp			//////////////
//////////////////////////////////////////////////////////
// <******************************************************>

#include <sxHeader.h>

// <------------------------------------------------->
// DEBUG MODE
#define DEBUG_MODE 1	// Set DEBUG_MODE to 1 to activate, zero to deactivate.
//#define QUICK_MODE 1	// Set QUICK_MODE to 1 to activate, zero to deactivate.
// <****************	**********************************>


////////////////// Temporary START
uint8_t TXBuffer[256];
uint8_t RXBuffer[256];
uint8_t  lora_TXStart;
uint8_t  lora_TXEnd;
////////////////// Temporary END

// *********************** LoRa ********************\\
	
lora::lora(){
	_BW = BW125;				// Default Bandwidth = 125 kHz
	_CR = CR1;					// Default Error coding rate = 4/5
	_SF = SF7;					// Defualt Spreading factor = 7
	_headerMode = EXPLICIT;		// Default Header mode = Explicit
	_packFlag = 0;
	_CRC = CRC_ON;
	//modemConfig(_BW, _CR, _SF, _headerMode);
}


void lora::Begin(){
	pinMode(_loraReset, OUTPUT);
		digitalWrite(_loraReset, LOW);
	pinMode(_loraNSS, OUTPUT);
		digitalWrite(_loraNSS, HIGH);
	Serial.begin(9600);
	SPI.begin();
	SPI.setClockDivider(SPI_CLOCK_DIV2);
	SPI.setDataMode(SPI_MODE0);
	SPI.setBitOrder(MSBFIRST);
	
	// DEBUG
		#if (DEBUG_MODE)
			Serial.println("lora::Begin()");
			Serial.print("My Address: "); Serial.println(_myAddress);
		#endif

	Reset();
	loraOn();	// Turns LoRa on.
	Check();	// Check that the device is working.
}

void lora::loraOn(){
	// DEBUG
		#if (DEBUG_MODE)
			Serial.println("loraON()");
		#endif
	
	writeToReg(RegOpMode, FSK_SLEEP);
	writeToReg(RegOpMode, LORA_SLEEP);
	writeToReg(RegOpMode, LORA_STANDBY);
	
//	if(QUICK_MODE)
//		// Set default settings.
//		writeToReg(RegModemConfig1, )
}
void lora::Reset(){
	digitalWrite(_loraReset, LOW);
	delay(5);
	digitalWrite(_loraReset, HIGH);
	
	// DEBUG
		#if (DEBUG_MODE)
			Serial.println("lora::Reset()");
		#endif
	
}

void lora::writeToReg(uint8_t regAdd, uint8_t data){
	digitalWrite(_loraNSS, LOW);
	SPI.transfer(regAdd | 0x80);	// Mask for writing
	SPI.transfer(data);
	digitalWrite(_loraNSS, HIGH);
}

uint8_t lora::readFromReg(uint8_t regAdd){
	uint8_t data;
	
	digitalWrite(_loraNSS, LOW);
	SPI.transfer(regAdd & 0x7F);	// Mask for reading
	data = SPI.transfer(0);
	digitalWrite(_loraNSS, HIGH);

	return data;
}

uint8_t lora::getBit(uint8_t regAdd, uint8_t num){
	uint8_t bits, mask=1;
	bits = readFromReg(regAdd);
	
	return ((bits&(mask<<=num))>>num);
}

void lora::setBit(uint8_t num, uint8_t value, uint8_t regAdd){
	uint8_t bits;
	
	if(value != 1 && value != 0){
		Serial.println("Invalid Value. Choose 1 or 0.");
		return;
	}
	
	bits = readFromReg(regAdd);
	
	// DEBUG
		#if (DEBUG_MODE)
			Serial.print("Before:\t"); Serial.println(bits, BIN);
		#endif

	if(value){
		bits |= (value<<=num);
	}
	else
		bits &= ((value<<=num)^0xFF);
	
	writeToReg(regAdd, bits);
	// DEBUG
		#if (DEBUG_MODE)
			Serial.print("After:\t"); Serial.println(bits, BIN);
		#endif
}

bool lora::Check(){
	uint8_t check1, check2, check3, check4;
	
	writeToReg(RegFrMsb, 0x01);			// 0x01	=	1
	writeToReg(RegFrMid, 0x0B);			// 0x0B	=	11
	writeToReg(RegFrLsb, 0x6F);			// 0x6F	=	111
	
	// Check writing to registers.
	check1 = readFromReg(RegFrMsb);
	check2 = readFromReg(RegFrMid);
	check3 = readFromReg(RegFrLsb);
	
	// Check if LoRa is on.
	check4 = readFromReg(0x0E);			// This register's default for FSK mode is 0x02; for LoRa mode - 0x80.
	
	if((check1 == 1) && (check2 == 11) && (check3 == 111))// && (check4 == 0x0E))
	{
		// DEBUG
			#if (DEBUG_MODE)
				Serial.println("lora::Check SUCCESS");
				if(check4 == 0x02)
					Serial.println("Working as FSK. (call loraOn() to activate LoRa)");
				else if(check4 == 0x80)
					Serial.println("Working as LoRa");
				else
					Serial.println("Working as unknown");
			#endif
		writeToReg(RegFrMsb, 0x6C);
		writeToReg(RegFrMid, 0x80);
		writeToReg(RegFrLsb, 0x00);
		return true;
	}
	else{
		// DEBUG
			#if (DEBUG_MODE)
				Serial.println("lora::Check FAILED");
			#endif
		return false;
	}
}

void lora::Sleep(){
	digitalWrite(_loraNSS, LOW);
	writeToReg(RegOpMode, LORA_SLEEP);
	digitalWrite(_loraNSS, HIGH);
	
	// DEBUG
		#if (DEBUG_MODE)
			Serial.println("lora::Sleep()");
		#endif
}

void lora::Standby(){
	digitalWrite(_loraNSS, LOW);
	writeToReg(RegOpMode, LORA_STANDBY);
	digitalWrite(_loraNSS, HIGH);
	
	// DEBUG
		#if (DEBUG_MODE)
			Serial.println("lora::Standby()");
		#endif
}

void lora::modemConfig(uint8_t BW, uint8_t CR, uint8_t SF, uint8_t headerMode){
	uint8_t config1, config2;
	
	config1 = BW+CR+headerMode;
	config2 = SF*16+7;
	
	writeToReg(RegOpMode, LORA_SLEEP);
	
	writeToReg(RegModemConfig1, config1);		// RegModemConfig1
	writeToReg(RegModemConfig2, config2);		// RegModemConfig2
	
	// DEBUG
		#if (DEBUG_MODE)
			Serial.println("lora::modemConfig()");
			Serial.print("config1 HEX:\t0x"); Serial.println(config1, HEX);
			Serial.print("config1 BIN:\t"); Serial.println(config1, BIN);
			Serial.print("config2 HEX:\t0x"); Serial.println(config2, HEX);
			Serial.print("config2 BIN:\t"); Serial.println(config2, BIN);
		#endif
}

void lora::setFreq(float freq){
	long newFreq;
	uint8_t freqMsb, freqMid, freqLsb;
	
	newFreq = ((freq*1000000)/61.03515625);
	Serial.print("newFreq:\t"); Serial.println(newFreq, HEX);
	freqMsb = newFreq >> 16;
	freqMid = ((newFreq >> 8)&0xFF);
	freqLsb = newFreq & 0x0000FF;
	
	// DEBUG
		#if(DEBUG_MODE)
			Serial.print("Frequency:\t"); Serial.println(newFreq, BIN);
			Serial.print("MSB:\t"); Serial.println(freqMsb, BIN);
			Serial.print("MID:\t"); Serial.println(freqMid, BIN);
			Serial.print("LSB:\t"); Serial.println(freqLsb, BIN);
		#endif
}

void lora::buildPackage(uint8_t start, uint8_t end, uint8_t type, uint8_t destination, uint8_t source, uint8_t timeout, uint8_t power){
	package pack;
	
	pack.start = start;
	pack.end = end;
	pack.type = type;
	pack.destination = destination;
	pack.source = source;
	pack.timeout = timeout;
	
	_packFlag = 1;
	
	// DEBUG
		#if (DEBUG_MODE)
			Serial.println("\t***** Pack: *****");
			Serial.print("Start\t\t=\t"); Serial.println(pack.start);
			Serial.print("End\t\t=\t"); Serial.println(pack.end);
			Serial.print("Type\t\t=\t"); Serial.println(pack.type);
			Serial.print("Destination\t=\t"); Serial.println(pack.destination);
			Serial.print("Source\t\t=\t"); Serial.println(pack.source);
			Serial.print("Timeout\t\t=\t"); Serial.println(pack.timeout);
		#endif
}

void lora::Send(uint8_t pctStart, uint8_t pctEnd, uint8_t pctType, uint8_t pctDestination, long pctTimeout, uint8_t Power){
	uint8_t start, end, pctLength, data;
	
	
	writeToReg(RegOpMode, LORA_STANDBY);
	writeToReg(RegIrqFlags, 0xFF);
	writeToReg(RegIrqFlagsMask, 0xF7);
	writeToReg(RegFifoTxBaseAddr, 0x00);
	writeToReg(RegFifoAddrPtr, 0x00);
	
	digitalWrite(_loraNSS, LOW);
	
	// Send 3 header's bytes START
	SPI.transfer(0x80);
	SPI.transfer(pctType);
	SPI.transfer(pctDestination);
	SPI.transfer(_myAddress);
	pctLength = 3;
	// Send 3 header's bytes END
	
	for(start = pctStart; start <= pctEnd; start++){
		pctLength++;
		data = TXBuffer[start];
		SPI.transfer(data);
	}
Serial.print("PacketLength:\t"); Serial.println(pctLength);
	
	digitalWrite(_loraNSS, HIGH);
	
	writeToReg(RegPayloadLength, pctLength);
	
	//TXONLoRa START
	writeToReg(RegPaConfig, 240);
	writeToReg(RegOpMode, LORA_TRANSMIT);
	//TXONLoRa END
	
	uint8_t lregData;
	do{
		delay(1);
		lregData = readFromReg(RegIrqFlags);
//		Serial.print(lregData); Serial.print(", ");  // TEMPORARY
	}
	while(lregData == 0);
	
	if(lregData == 8)
		Serial.println("Sent.");
	else
		Serial.println("Failed.");
	
	writeToReg(RegOpMode, LORA_SLEEP);

}

void lora::sendAck(uint8_t sourceAddress, uint8_t myAddress){
	uint8_t pctType = 7, pctLength;
	
	writeToReg(RegOpMode, LORA_STANDBY);
	writeToReg(RegIrqFlags, 0xFF);
	writeToReg(RegIrqFlagsMask, 0xF7);
	writeToReg(RegFifoTxBaseAddr, 0x00);
	writeToReg(RegFifoAddrPtr, 0x00);
	
	digitalWrite(_loraNSS, LOW);
	
	SPI.transfer(0x80);
	SPI.transfer(pctType);
	SPI.transfer(sourceAddress);	// Source to send the ack to
	SPI.transfer(_myAddress);		// My address
	pctLength = 3;
	
	digitalWrite(_loraNSS, HIGH);
	
	writeToReg(RegPayloadLength, pctLength);
	
	//TXONLoRa START
	writeToReg(RegPaConfig, 240);
	writeToReg(RegOpMode, LORA_TRANSMIT);
	//TXONLoRa END	

Serial.println();

	uint8_t lregData;
	do{
		delay(1);
		lregData = readFromReg(RegIrqFlags);
//		Serial.print(lregData); Serial.print(", ");  // TEMPORARY
	}
	while(lregData == 0);
	
	if(lregData == 8)
		Serial.println("Sent.");
	else
		Serial.println("Failed.");

	
	writeToReg(RegOpMode, LORA_SLEEP);
	
	Serial.println("Ack Sent.");
}

bool lora::receiveAck(int timeout){
	uint8_t pctLength, pctType, pctSource, pctDestination, rxDoneFlag;
	
	writeToReg(RegOpMode, LORA_STANDBY);
	writeToReg(RegFifoRxBaseAddr, 0x00);
	writeToReg(RegFifoAddrPtr, 0x00);
	writeToReg(RegIrqFlagsMask, 0x9F);
	writeToReg(RegIrqFlags, 0xFF);
	writeToReg(RegOpMode, LORA_RXCONTINUOUS);
	
	do{
		rxDoneFlag = readFromReg(RegIrqFlags);
		rxDoneFlag &= 0x40;				// checks if RxDone interrupt is set (P. 41)
		delay(1);
		timeout--;
	}
	while(rxDoneFlag==0 && timeout !=0);

	if(!timeout)
		return 0;
	
	writeToReg(RegOpMode, LORA_STANDBY);	// RXOff
	
	pctLength = readFromReg(RegRxNbBytes);
	
	writeToReg(RegFifoAddrPtr, 0x00);
	
	digitalWrite(_loraNSS, LOW);
	SPI.transfer(0x00);
	pctType = SPI.transfer(0);
	pctSource = SPI.transfer(0);
	pctDestination = SPI.transfer(0);
	
	digitalWrite(_loraNSS, HIGH);
	
	Serial.print("source:\t"); Serial.println(pctSource);
	Serial.print("destination:\t"); Serial.println(pctDestination);
	Serial.print("type:\t"); Serial.println(pctType);
	
	return 1;
}

///////////////////////////////////
void lora::Send2(uint8_t pctStart, uint8_t pctEnd, uint8_t pctType, uint8_t pctDestination, long pctTimeout, uint8_t Power, int ackTimeout){
	uint8_t start, end, pctLength, data;
	
	
	writeToReg(RegOpMode, LORA_STANDBY);
	writeToReg(RegIrqFlags, 0xFF);
	writeToReg(RegIrqFlagsMask, 0xF7);
	writeToReg(RegFifoTxBaseAddr, 0x00);
	writeToReg(RegFifoAddrPtr, 0x00);
	
	digitalWrite(_loraNSS, LOW);
	
	// Send 3 header's bytes START
	SPI.transfer(0x80);
	SPI.transfer(pctDestination);
	SPI.transfer(pctType);
	SPI.transfer(_myAddress);
	pctLength = 3;
	// Send 3 header's bytes END
	
	for(start = pctStart; start <= pctEnd; start++){
		pctLength++;
		data = TXBuffer[start];
		SPI.transfer(data);
	}
	//Serial.print("PacketLength:\t"); Serial.println(pctLength);
	
	digitalWrite(_loraNSS, HIGH);
	
	writeToReg(RegPayloadLength, pctLength);
	
	//TXONLoRa START
	writeToReg(RegPaConfig, 240);
	writeToReg(RegOpMode, LORA_TRANSMIT);
	//TXONLoRa END
	
	uint8_t lregData;
	do{
		delay(1);
		lregData = readFromReg(RegIrqFlags);
		//Serial.print(lregData); Serial.print(", ");  // TEMPORARY
	}
	while(lregData == 0);
	
	if(lregData == 8)
		Serial.println("Sent.");
	else
		Serial.println("Failed.");
	
	if(ackTimeout){
		bool ack;
		
		do{
		ack = receiveAck(5000);
		delay(1);
		ackTimeout--;
		if(ack)
			break;
		}
		while(!ackTimeout);
	}
	
	writeToReg(RegOpMode, LORA_SLEEP);

}

void lora::Receive2(int timeout){
	uint8_t pctType;
	uint8_t pctDestination;
	uint8_t pctSource;
	uint8_t pctLength;
	uint8_t start,end;
	uint8_t data;
	uint8_t rxDoneFlag;
	
	Serial.println("Receiving...");
	
	writeToReg(RegOpMode, LORA_STANDBY);
	writeToReg(RegFifoRxBaseAddr, 0x00);
	writeToReg(RegFifoAddrPtr, 0x00);
	writeToReg(RegIrqFlagsMask, 0x9F);
	writeToReg(RegIrqFlags, 0xFF);
	writeToReg(RegOpMode, LORA_RXCONTINUOUS);
	
	do{
		rxDoneFlag = readFromReg(RegIrqFlags);
		rxDoneFlag &= 0x40;				// checks if RxDone interrupt is set (P. 41)
		delay(1);
		timeout--;
	}
	while(rxDoneFlag==0 && timeout !=0);

	if(!timeout){
		Serial.println("Nothing received.");
		return;
	}
	
	writeToReg(RegOpMode, LORA_STANDBY);	// RXOff
	
	pctLength = readFromReg(RegRxNbBytes);
		Serial.print("pctLength:\t"); Serial.println(pctLength);		
	writeToReg(RegFifoAddrPtr, 0x00);
	
	digitalWrite(_loraNSS, LOW);
	SPI.transfer(0x00);
	pctDestination = SPI.transfer(0);
	
		// TEMPORARY START
	if(pctDestination != _myAddress){
		Serial.println("Not for me.");
		Sleep();
		return;
	}
		// TEMPORARY END
		
		
	
//DEBUG		Serial.print("pctDestination:\t"); Serial.println(pctDestination);
	pctType = SPI.transfer(0);
//DEBUG		Serial.print("pctType:\t"); Serial.println(pctType);
	pctSource = SPI.transfer(0);
//DEBUG		Serial.print("pctSource:\t"); Serial.println(pctSource);
	start = 0;
	end = pctLength - 4;
	
	for(start; start <= end; start++){
		data = SPI.transfer(0);
		RXBuffer[start] = data;
	}
	
	for(start=0; start<=end; start++){
		Serial.write(RXBuffer[start]);
	}
	
	digitalWrite(_loraNSS, HIGH);
	
	sendAck(1,2);
}
///////////////////////////////////

void lora::SendImplicit(uint8_t pctLength, uint8_t pctCR, uint8_t pctCRCStatus){
			// TODO
}

void lora::SendExplicit(uint8_t pctLength, uint8_t pctCR, uint8_t pctCRCStatus){
			// TODO	
	// Explicit or Implicit.
	
	// 1. Set ImplicitHeaderModeOn bit on RegModemConfig1 to 1.
	// 2. Only in explicit mode: Header contain payload length, CR and optional CRC
	// 3. the presence of the CRC at the end of the payload in selected only on the
	//	  transmitter side through the bit RxPayloadCrcOn in the register
	//	  RegModemConfig1.
	//    On the receiver side, that bit is not used.
	// 4.
	
	//writeToReg()
}

void testMessage(){
	
}

void lora::Receive(int timeout){
	uint8_t pctType;
	uint8_t pctDestination;
	uint8_t pctSource;
	uint8_t pctLength;
	uint8_t start,end;
	uint8_t data;
	
	Serial.println("Receive");
	
	writeToReg(RegOpMode, LORA_STANDBY);
	writeToReg(RegFifoRxBaseAddr, 0x00);
	writeToReg(RegFifoAddrPtr, 0x00);
	writeToReg(RegIrqFlagsMask, 0x9F);
	writeToReg(RegIrqFlags, 0xFF);
	writeToReg(RegOpMode, LORA_RXCONTINUOUS);

	
						//////// Temporary START
	uint8_t lregData, var1;
	do{
		delay(1);
		lregData = readFromReg(RegIrqFlags);
		var1 = (lregData & 0x40);				// checks if RxDone interrupt is set (P. 41)
	}
	while(var1 == 0);
	//////// Temporary END

	
	writeToReg(RegOpMode, LORA_STANDBY);	// RXOff
	
	pctLength = readFromReg(RegRxNbBytes);
	
	writeToReg(RegFifoAddrPtr, 0x00);
	
	digitalWrite(_loraNSS, LOW);
	SPI.transfer(0x00);
	pctType = SPI.transfer(0);
	pctDestination = SPI.transfer(0);
	pctSource = SPI.transfer(0);
	start = 0;
	end = pctLength - 4;
	
	for(start; start <= end; start++){
		data = SPI.transfer(0);
		RXBuffer[start] = data;
	}
	
	for(start=0; start<=end; start++){
		Serial.write(RXBuffer[start]);
	}
	
	digitalWrite(_loraNSS, HIGH);
}

void lora::sendStr(){
	char message[256] = "0";
	int i=0, len=0;
	
	Serial.println("Write your string (up to 256 characters):");
	
	while(1){
		if(Serial.available()){
			do{
				message[i] = Serial.read();
				i++;
				delay(50);
			}
			while(Serial.available());
			
			Serial.print("i: "); Serial.println(i);
			
			for(int j=0; j<=i; j++){
				Serial.print(message[j]);
			}
			Serial.println();
			Serial.println("*** END ***");
			return;
		}
		if(i>0)
		return;
	}
}

void lora::fillTX(){
	Serial.println("Filling TX with data");
	uint8_t lora_LLoop;
	uint8_t lora_LData;
	lora_TXEnd = 255;                                  // so a single byte packet ends up with lora_TXend of 0
	lora_TXStart = 0;

	for (lora_LLoop = 0x41; lora_LLoop <= 0x5A; lora_LLoop++)
	{
		lora_TXEnd++;
		TXBuffer[lora_TXEnd] = lora_LLoop;
	}
}

void lora::fillBuffer(char message2[], uint8_t msgLength){
	for(uint8_t i=0; i<msgLength; i++)
		TXBuffer[i] = message2[i];
	
//	#if (DEBUG_MODE)
//		for(int j=0; i>)
	
}