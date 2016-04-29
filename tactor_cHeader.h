#ifndef TACTOR_H_
#define TACTOR_H_

using namespace std;

#include <windows.h>


// Enumerating some basic definitions to clarify function calls

enum Tactor
{
	None = 0x00,
	Tac1 = 0x01,
	Tac2 = 0x02,
	Tac3 = 0x04,
	Tac4 = 0x08,
	Tac5 = 0x10,
	Tac6 = 0x20,
	Tac7 = 0x40,
	Tac8 = 0x80,
	All = 0xFF
};
enum SequenceNum
{
	Seq1 = 0, Seq2 = 1, Seq3 = 2, Seq4 = 3, Seq5 = 4,
	Seq6 = 5, Seq7 = 6, Seq8 = 7, Seq9 = 8, Seq10 = 9,
	Seq11 = 10, Seq12 = 11, Seq13 = 12, Seq14 = 13, Seq15 = 14,
	Seq16 = 15
};
enum Address
{
	Master = 0x00,
	Slave1 = 0x01,
	Slave2 = 0x02,
	Slave3 = 0x03,
	Slave4 = 0x04,
	Slave5 = 0x05,
	Slave6 = 0x06,
	Slave7 = 0x07,
	ADDRESSAll = 0xFF
};
enum Gain
{
	One = 0x00,
	Two = 0x40,
	Three = 0x80,
	Four = 0xc0
};
enum SigSrc
{
	NoSignal = 0,
	SignalOne = 1,
	SignalTwo = 2,
	SummedSignal = 3
};
enum ConnectionType
{
	Serial = 0x01,
	USBADV = 0x02,
	Bluetooth = 0x04,
	BluetoothCache = 0x08,
	USBEVAL = 0x010,
	SerialALL = 0x20,
};



//Defining type

typedef int (*ConnectDirectPtr)(char*name, int type);
typedef int (*TacOnTimePtr)(int cidx, int board, int tacNum, int durMilli, bool returnifprocessing);
typedef int (*SetFreqPtr)(int cidx, int board, int freq, bool returnifprocessing);

//define SetFreqFine Pointer
typedef int (*SetFreq_FinePtr)(int cidx, int board, int freq, bool returnifprocessing);
typedef int (*SeqWaitPtr)(int cidx, int board, int waitTime, bool returnifprocessing);

typedef int (*SetGainPtr)(int cidx, int board, int gain, int attenuation, bool returnifprocessing);
typedef int (*SetGainPtr)(int cidx, int board, int gain, int attenuation, bool returnifprocessing);
typedef int (*OpenPortPtr)(char * com);
typedef int (*DiscoverDevicesPtr)(int byte);
typedef int (*GetLastEAIErrorPtr)();
typedef char*( *GetDevicePtr)(int index, int*size, int*Type);
typedef int (*ConnectPtr)(int index,int type);
typedef int (*DisconnectPtr)(int index);
typedef int (*KillDLLptr)();
typedef int (*ClsPortptr)(int cidx);
typedef int (*SetSigSrcptr)(int cindx,int board,int bank1, int bank2, bool returnifprocessing);
typedef int (*ReadFWVptr)(int cindx,int board, bool returnifprocessing);
typedef void ( *ShowDebugInfoPTR)(bool tf);
typedef int (*ReadDataPTR)(const int cindx, unsigned char* buf,const int bufsize,  int* bufsizee);


//Establish pointers of the form declared in typedef section
KillDLLptr KillDLL;
OpenPortPtr OpenPort;
SetGainPtr SetGain;
SetFreqPtr SetSinFreq1;
SetFreqPtr SetSinFreq2;
//Attempting to mimic above approach
SetFreq_FinePtr SetSinFreq_Fine1;
SetFreq_FinePtr SetSinFreq_Fine2;
//SeqWait Pointer
SeqWaitPtr SeqWait;

SetSigSrcptr SetSigSrc;
ReadFWVptr ReadFWVer;
TacOnTimePtr TacOnTime;
ConnectDirectPtr ConnectDirect;
TacOnTimePtr TacOnTimeForTAction;
ClsPortptr ClosePort;
DiscoverDevicesPtr DiscoverDevices;
GetLastEAIErrorPtr GetLastEAIError;
GetDevicePtr GetDevice;
ConnectPtr Connect;
DisconnectPtr Disconnect;
ShowDebugInfoPTR ShowDebugInfo;



//Establish connection with .dll
HINSTANCE tactorhandle = NULL;

inline int InitTactorDLL()
{
	tactorhandle = LoadLibrary("Tactor_DLL.dll");
	if (tactorhandle == 0)
		return -1;

	//Establish function in relation to pointer type
	//Pull reference from .dll
	ConnectDirect = (ConnectDirectPtr)GetProcAddress(tactorhandle, "ConnectDirect");
	TacOnTime = (TacOnTimePtr)GetProcAddress(tactorhandle, "TacOnTime");
	SetSinFreq1 = (SetFreqPtr)GetProcAddress(tactorhandle, "SetSinFreq1");
	SetSinFreq2 = (SetFreqPtr)GetProcAddress(tactorhandle, "SetSinFreq2");
	
	//Custom code for fine frequency
	SetSinFreq_Fine1 = (SetFreq_FinePtr)GetProcAddress(tactorhandle, "SetSinFreq1_Fine");  // Attempt to call SetSinFreq1_Fine
	SetSinFreq_Fine2 = (SetFreq_FinePtr)GetProcAddress(tactorhandle, "SetSinFreq2_Fine");  // Note: F must be capitalized, as in Tactor_Dll.h
	
	//Custom code for SeqWait
	SeqWait = (SeqWaitPtr)GetProcAddress(tactorhandle, "SeqWait");

	SetGain = (SetGainPtr)GetProcAddress(tactorhandle, "SetGain");
	KillDLL = (KillDLLptr)GetProcAddress(tactorhandle, "KillDLL");
	ClosePort =(ClsPortptr) GetProcAddress(tactorhandle, "ClosePort");
	SetSigSrc = (SetSigSrcptr)GetProcAddress(tactorhandle, "SetSigSrc");
	ReadFWVer = (ReadFWVptr)GetProcAddress(tactorhandle, "ReadFwVer");
	GetLastEAIError = (GetLastEAIErrorPtr)GetProcAddress(tactorhandle, "GetLastEAIError");
	GetDevice = (GetDevicePtr)GetProcAddress(tactorhandle, "GetDevice");
	Connect = (ConnectPtr)GetProcAddress(tactorhandle, "Connect");

	return 0;
}



#endif
