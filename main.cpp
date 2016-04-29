#include <iostream>
#include <fstream>
#include <windows.h>
#include "wiimote.h"
#include <mmsystem.h>
#include <WINDOWS.H>
#include <math.h>

#define PI 3.14159265

using namespace std;

#include "tactor_cHeader.h"

string portName = "COM6";


void on_state_change (wiimote			  &remote,
					  state_change_flags  changed,
					  const wiimote_state &new_state)
	{
	// we use this callback to set report types etc. to respond to key events
	//  (like the wiimote connecting or extensions (dis)connecting).
	
	// NOTE: don't access the public state from the 'remote' object here, as it will
	//		  be out-of-date (it's only updated via RefreshState() calls, and these
	//		  are reserved for the main application so it can be sure the values
	//		  stay consistent between calls).  Instead query 'new_state' only.

	// the wiimote just connected
	if(changed & CONNECTED)
		{
		// ask the wiimote to report everything (using the 'non-continous updates'
		//  default mode - updates will be frequent anyway due to the acceleration/IR
		//  values changing):

		// note1: you don't need to set a report type for Balance Boards - the
		//		   library does it automatically.
		
		// note2: for wiimotes, the report mode that includes the extension data
		//		   unfortunately only reports the 'BASIC' IR info (ie. no dot sizes),
		//		   so let's choose the best mode based on the extension status:
		if(new_state.ExtensionType != wiimote::BALANCE_BOARD)
			{
			if(new_state.bExtension)
				remote.SetReportType(wiimote::IN_BUTTONS_ACCEL_IR_EXT); // no IR dots
			else
				remote.SetReportType(wiimote::IN_BUTTONS_ACCEL_IR);		//    IR dots
			}
		}
	// a MotionPlus was detected
	if(changed & MOTIONPLUS_DETECTED)
		{
		// enable it if there isn't a normal extension plugged into it
		// (MotionPlus devices don't report like normal extensions until
		//  enabled - and then, other extensions attached to it will no longer be
		//  reported (so disable the M+ when you want to access them again).
		if(remote.ExtensionType == wiimote_state::NONE) {
			bool res = remote.EnableMotionPlus();
			_ASSERT(res);
			}
		}
	// an extension is connected to the MotionPlus
	else if(changed & MOTIONPLUS_EXTENSION_CONNECTED)
		{
		// We can't read it if the MotionPlus is currently enabled, so disable it:
		if(remote.MotionPlusEnabled())
			remote.DisableMotionPlus();
		}
	// an extension disconnected from the MotionPlus
	else if(changed & MOTIONPLUS_EXTENSION_DISCONNECTED)
		{
		// enable the MotionPlus data again:
		if(remote.MotionPlusConnected())
			remote.EnableMotionPlus();
		}
	// another extension was just connected:
	else if(changed & EXTENSION_CONNECTED)
		{
#ifdef USE_BEEPS_AND_DELAYS
		Beep(1000, 200);
#endif
		// switch to a report mode that includes the extension data (we will
		//  loose the IR dot sizes)
		// note: there is no need to set report types for a Balance Board.
		if(!remote.IsBalanceBoard())
			remote.SetReportType(wiimote::IN_BUTTONS_ACCEL_IR_EXT);
		}
	// extension was just disconnected:
	else if(changed & EXTENSION_DISCONNECTED)
		{
#ifdef USE_BEEPS_AND_DELAYS
		Beep(200, 300);
#endif
		// use a non-extension report mode (this gives us back the IR dot sizes)
		remote.SetReportType(wiimote::IN_BUTTONS_ACCEL_IR);
		}
	}
//----------------------------Define Constants-----------------------------//
int THRESH1 = 20;
int THRESH2 = 45;
int TSIG1 = 2500;
int TSIG2 = 2450;
int rmbloop = 0;
int loopbreak = 2;
int tactime = 150;
int threshtag = 0;
int SleepPulse = 200; //250?
//------------------------End Constant Definition-------------------------//

void rmblplse(int length, int gap, wiimote &remote)
{
	int time=timeGetTime();
	while(timeGetTime()-time <= length) remote.SetRumble(1);
	time = timeGetTime();
	while(timeGetTime()-time <= gap) remote.SetRumble(0);
}

int main()
{
	cout << "Load DLLS and Functions into Memory\n";
	
	if(InitTactorDLL() < 0)
	{
		cout << "ERROR DURING INIT\n";
		return -1;
	}

	SetConsoleTitle(_T("Orientation Signalling - HF&CS Lab"));
	HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);

	// create a wiimote object
	wiimote remote;
	// we use a state-change callback to get notified of
	//  extension-related events, and polling for everything else
	remote.ChangedCallback		= on_state_change;
	//  notify us only when the wiimote connected sucessfully, or something
	//   related to extensions changes
	remote.CallbackTriggerFlags = (state_change_flags)(CONNECTED |
													   EXTENSION_CHANGED |
													   MOTIONPLUS_CHANGED);
reconnect:
	COORD pos = { 0, 6 };
	SetConsoleCursorPosition(console, pos);

	// try to connect the first available wiimote in the system
	//  (available means 'installed, and currently Bluetooth-connected'):
	static const TCHAR* wait_str[] = { _T(".  "), _T(".. "), _T("...") };
	unsigned count = 0;
	while(!remote.Connect(wiimote::FIRST_AVAILABLE)) {
		_tprintf(_T("\b\b\b\b%s "), wait_str[count%3]);
		count++;
#ifdef USE_BEEPS_AND_DELAYS
		Beep(500, 30); Sleep(1000);
#endif
		}


#ifdef USE_BEEPS_AND_DELAYS
	Beep(1000, 300); Sleep(2000);
#endif

	COORD cursor_pos = { 0, 6 };
	MessageBox(NULL, "WiiMote Connected!", "Wiimote", MB_OK);

		
	//---------------Establish Connection with Tactors -------------------------//		
	ConnectDirect("COM9", Serial); 
			
	SetSinFreq_Fine1(0,0,2500,false);
	SetSinFreq_Fine2(0,0,2400,false);
	SetSigSrc(0,0,3,3,false);
	SetGain(0,0,3,61,false);
	

	//Connect second tactor set for mirroring
	/*
	ConnectDirect("COM4", Serial);
	SetSinFreq_Fine1(1,0,2500,false);
	SetSinFreq_Fine2(1,0,2505,false);
	SetGain(1,0,4,63,false);
	MessageBox(NULL, "Tactors Connected!","Tactors", MB_OK);
	*/
	//----------------------Tactor Connection Established ----------------------//


	
	//--------------------Begin Wiimote Feedback Console -----------------------//
	
	//-------------------------Variable Definitions ----------------------------//
	//Define mag variable for Acceleration Magnitude, define MAX for magnitude tuning
	
	
	//Vector Signaling Declarations
		
	/*ofstream logfile;
	logfile.open("logfile.txt", ios::out | ios::app);  //Opens logfile.txt in output mode and appends all incoming data.
	*/
	//Logfile should be formatted to work with Excel's import function.

	//logfile << "\n\nWiiMote Tracking Data: "<< timeGetTime() << "\n";
	//logfile << " Time Elapsed, Magnitude,  X Accel,  Z Accel, \n";
	//logfile << "X; Y; Z; Time\n";
	//----------------------End Variable Definitions ---------------------------//


	

	//Everything from this point on defines the interaction protocol


	// display the wiimote state data until 'Home' is pressed:
	while(!remote.Button.Home())// && !GetAsyncKeyState(VK_ESCAPE))
		{

		// IMPORTANT: the wiimote state needs to be refreshed each pass
		while(remote.RefreshState() == NO_CHANGE)
			Sleep(1); // // don't hog the CPU if nothing changed

		cursor_pos.Y = 0;
		SetConsoleCursorPosition(console, cursor_pos);

		// did we lose the connection?
		if(remote.ConnectionLost())
			{
			_tprintf(
				_T("   *** connection lost! ***                                          \n")
				);
			Beep(100, 1000);
			Sleep(2000);
			COORD pos = { 0, 6 };
			SetConsoleCursorPosition(console, pos);
			goto reconnect;
			}

		// Battery level:
		_tprintf(_T("  Battery: "));
		// (the green/yellow colour ranges are rough guesses - my wiimote
		//  with rechargeable battery pack fails at around 15%)
		(remote.bBatteryDrained	    );
		(remote.BatteryPercent >= 30);
		_tprintf(_T("%3u%%   "), remote.BatteryPercent);

		DWORD current_time = timeGetTime();


		// Rumble
		// Output method:
	    _tprintf( _T("        using %s\n"), (remote.IsUsingHIDwrites()?
											   _T("HID writes") : _T("WriteFile()")));
		
		// 'Unique' IDs (not guaranteed to be unique, check the variable
		//  defintion for details)
		_tprintf(_T("       ID: "));
		_tprintf(_T("%I64u")  , remote.UniqueID);
#ifdef ID2_FROM_DEVICEPATH		// (see comments in header)
		_tprintf(_T("   ID2: "));
		_tprintf(_T("%I64u\n"), remote.UniqueID2);
#else
		_tprintf(_T("\n"));
#endif

		// Acceleration Display:
		_tprintf(_T("    Accel:"));
		_tprintf(_T("  X %+2.3f  Y %+2.3f  Z %+2.3f  \n"),
					remote.Acceleration.X,
					remote.Acceleration.Y,
					remote.Acceleration.Z);
		

		
	// Orientation estimate (shown red if last valid update is aging):
		_tprintf(_T("   Orient:"));
		_tprintf(_T("  UpdateAge %3u  "), remote.Acceleration.Orientation.UpdateAge);


		_tprintf("\n\nStandard Wii Orientations\n");
		_tprintf(_T("Pitch:%4ddeg  Roll:%4ddeg  \n"),
				 (int)remote.Acceleration.Orientation.Pitch,
				 (int)remote.Acceleration.Orientation.Roll );

		//Motion Plus Feedback Section
		_tprintf("\n\nMotion Plus Functions");
		_tprintf("\n");
		_tprintf(_T("      Raw: "));
		_tprintf(_T("Yaw: %04hx  ")   , remote.MotionPlus.Raw.Yaw);
		_tprintf(_T("Pitch: %04hx  ") , remote.MotionPlus.Raw.Pitch);
		_tprintf(_T("Roll: %04hx  \n"), remote.MotionPlus.Raw.Roll);
		_tprintf(_T("    Float: "));
		_tprintf(_T(" %8.3fdeg")     , remote.MotionPlus.Speed.Yaw);
		_tprintf(_T("  %8.3fdeg")   , remote.MotionPlus.Speed.Pitch);
		_tprintf(_T(" %8.3fdeg \n")   , remote.MotionPlus.Speed.Roll);

	//----------------End Wiimote Feedback Console----------------------//
		
	

	//----------------Begin Orientation Signaling-----------------------//
	//Code is initially setup to send two different signals based on the roll
	//levels between Thresh1 and Thresh 2.
	//It has since been modified to work off of Thresh1 and a combination of 
	//the A & B buttons.
		if(remote.Button.A()){
			if(remote.Acceleration.Orientation.Roll >= THRESH1) //Right
			{
				//if(remote.Acceleration.Orientation.Roll >= THRESH2){
				// Uncomment above to re-enable Two-Threshold signal method
				if(remote.Button.B()){						
					/*if(threshtag != 2){
						SetSinFreq_Fine2(0,0,TSIG2,false);
						SetSigSrc(0,0,3,3,false);
						threshtag = 2;
						}*/
				TacOnTime(0,0,0x88,60,true);
				remote.SetRumble(1);
			}
			else{
				// Signal based on remote orientation
				/*if(threshtag != 1){
						SetSinFreq_Fine2(0,0,TSIG1,false);
						SetSigSrc(0,0,3,3,false);
						threshtag = 1;
						}*/
				
				TacOnTime(0,0,0x70,tactime,true);
				//remote.SetRumble(1);
				rmblplse(125,15,remote);
				//Sleep(SleepPulse);
				}
			}
		
		
			else{
			if(remote.Acceleration.Orientation.Roll <= -THRESH1) //Left
			{
				//if(remote.Acceleration.Orientation.Roll <= -THRESH2){
				//Uncomment the above to reenable Two-Threshold Signalling
				if(remote.Button.B()){
					/*if(threshtag != 2){
						SetSinFreq_Fine2(0,0,TSIG2,false);
						SetSigSrc(0,0,3,3,false);
						threshtag = 2;
						}*/
					
					TacOnTime(0,0,0x88,tactime,true);
					remote.SetRumble(1);
				}
				else{
					/*if(threshtag != 1){
						SetSinFreq_Fine2(0,0,TSIG1,false);
						SetSigSrc(0,0,3,3,false);
						threshtag = 1;
						}*/
					
					TacOnTime(0,0,0x07,tactime,true);
					//remote.SetRumble(1);
					rmblplse(125,15,remote);
					//Sleep(SleepPulse);
					}
			}
		else {
			remote.SetRumble(0); // Turn off Rumble if not past Threshold
			threshtag = 0;
			}
		}
		}
		else remote.SetRumble(0); // Turn off Rumble if A is not pressed

		if(remote.Button.B()){
					/*if(threshtag != 2){
						SetSinFreq_Fine2(0,0,TSIG2,false);
						SetSigSrc(0,0,3,3,false);
						threshtag = 2;
						}*/
					
					TacOnTime(0,0,0x88,tactime,true);
					remote.SetRumble(1);
		}
	//-----------------------End Orientation Signaling---------------------//
		
		
	//---------------------- End Home Button Loop -------------------------//
		//Everything from here on down is disconnect and program shutdown protocols

	}		

	// disconnect (auto-happens on wiimote destruction anyway, but let's play nice)
	remote.Disconnect();
	Beep(1000, 200);

	// for automatic 'press any key to continue' msg

	CloseHandle(console);




	/*Tutorial 1 Tactor Test Functions*/
	/*ConnectDirect("COM9", Serial);

	cout << "Fire Example Commands\n";
	TacOnTime(0, 0, Tac1, 0xFA, false);
	Sleep(500);
	TacOnTime(0, 0, Tac2, 0xFA, false);
	Sleep(500);
	TacOnTime(0, 0, Tac3, 0xFA, false);
	Sleep(500);
	TacOnTime(0, 0, Tac4, 0xFA, false);
	Sleep(500);
	TacOnTime(0, 0, Tac5, 0xFA, false);
	Sleep(500);
	TacOnTime(0, 0, Tac6, 0xFA, false);
	Sleep(500);
	TacOnTime(0, 0, Tac7, 0xFA, false);
	Sleep(500);
	TacOnTime(0, 0, Tac8, 0xFA, false);
	cout << "Complete\n";

	Sleep(1000);*/
	

	

	//Close out tactor functions
	
	//TurnAllOff(0,0,false);
	KillDLL();
	

	return 0;
}
