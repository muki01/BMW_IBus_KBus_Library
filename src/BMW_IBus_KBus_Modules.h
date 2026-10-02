/* -----------------------------------------------------------------------------
BMW_IBus_KBus_Modules.h - module addresses used on the BMW I-Bus, K-Bus and D-Bus

Optional header. Include it when you want to use names instead of raw
addresses, for example:

    if (source == M_MFL && destination == M_RAD) { ... }
-----------------------------------------------------------------------------*/

#ifndef BMW_IBus_KBus_Modules_h
#define BMW_IBus_KBus_Modules_h

#include "Arduino.h"

// -----------------------------------------------------------------------------
// K/I-Bus module ID's
static const uint8_t M_GM5     = 0x00;  // General Body Electronics (ZKE 3/4/5)
static const uint8_t M_SHD     = 0x08;  // Tilt/Slide Sunroof (SHD)
static const uint8_t M_CDC     = 0x18;  // CD Changer (CDC)
static const uint8_t M_HKM     = 0x24;  // Trunk Lid Module (HKM) [E38]
static const uint8_t M_RCC     = 0x28;  // Radio Clock Control (RCC) [E38]
static const uint8_t M_EDC     = 0x2E;  // Electronic Damper Control (EDC)
static const uint8_t M_CCM     = 0x30;  // Check Control Module (CCM) [E38]
static const uint8_t M_GT      = 0x3B;  // Graphics Stage (GT)
static const uint8_t M_DIA     = 0x3F;  // Diagnostics (via gateway)
static const uint8_t M_FBZV    = 0x40;  // Remote Control for Central Locking (FBZV) [E31]
static const uint8_t M_GT_R    = 0x43;  // Unconfirmed: Rear Graphics Stage (GT) [E38]
static const uint8_t M_EWS     = 0x44;  // Drive Away Protection System (EWS)
static const uint8_t M_DWA     = 0x45;  // Anti-Theft System (DWA)
static const uint8_t M_CID     = 0x46;  // Central Information Display (CID) [E83, E85]
static const uint8_t M_FONT    = 0x47;  // Rear Control Panel (FONT_BT) [E38]
static const uint8_t M_TEL_J   = 0x48;  // Telephone (Japan)
static const uint8_t M_MFL     = 0x50;  // Multifunction Steering Wheel (MFL)
static const uint8_t M_MIR_P   = 0x51;  // Mirror Memory: Passenger [E46]
static const uint8_t M_MULTI   = 0x53;  // Unconfirmed: Multicast
static const uint8_t M_IHKA    = 0x5B;  // Automatic Heating/Air Conditioning (IHKA)
static const uint8_t M_PDC     = 0x60;  // Park Distance Control (PDC)
static const uint8_t M_ALC     = 0x66;  // Active Light Control (ALC)
static const uint8_t M_RAD     = 0x68;  // Radio
static const uint8_t M_EKM     = 0x69;  // Electronic Body Module (EKM) [E31]
static const uint8_t M_DSP     = 0x6A;  // Digital Sound Processor (DSP)
static const uint8_t M_WEB     = 0x6B;  // Auxiliary Heater "Webasto"
static const uint8_t M_RDC     = 0x70;  // Tire Pressure Control (RDC) / DWS
static const uint8_t M_SEAT_D1 = 0x71;  // Seat Memory: Driver [E31, E34]
static const uint8_t M_SEAT_D2 = 0x72;  // Seat Memory: Driver [E46, E53]
static const uint8_t M_CD      = 0x76;  // CD Player (Business)
static const uint8_t M_NAV     = 0x7F;  // Navigation
static const uint8_t M_IKE     = 0x80;  // Instrument Cluster (IKE/KOMBI)
static const uint8_t M_LWR     = 0x9A;  // Automatic Headlight Vertical Aim Control (LWR)
static const uint8_t M_MIR_D   = 0x9B;  // Mirror Memory: Driver [E46], CVM [E36]
static const uint8_t M_CVM     = 0x9C;  // Convertible Soft Top Module (CVM) [E46]
static const uint8_t M_ETS     = 0x9D;  // Electronic disconnecting switch (ETS) [E38]
static const uint8_t M_MID_R   = 0xA0;  // Rear Multi-functional Display (MID) [E38]
static const uint8_t M_MRS     = 0xA4;  // Multiple Restraint System (MRS)
static const uint8_t M_HVAC_R  = 0xA7;  // Rear Compartment Heating/Air Conditioning
static const uint8_t M_EHC     = 0xAC;  // Electronic Height Control (EHC)
static const uint8_t M_SES     = 0xB0;  // Speech Input System (SES)
static const uint8_t M_RC      = 0xB9;  // Compact Remote Control (RF/IR)
static const uint8_t M_NAV_J   = 0xBB;  // Navigation (Japan)
static const uint8_t M_ALL     = 0xBF;  // Broadcast
static const uint8_t M_MID     = 0xC0;  // Multi-functional Display (MID)
static const uint8_t M_TEL     = 0xC8;  // Telephone
static const uint8_t M_OBC     = 0xCD;  // On-Board Computer (OBC) [E31]
static const uint8_t M_LCM     = 0xD0;  // Lamp Check Module (LCM), Light Switch Center (LSZ)
static const uint8_t M_SEAT_P  = 0xDA;  // Seat Memory: Passenger [E46]
static const uint8_t M_IRIS    = 0xE0;  // Integrated Radio and Information System (IRIS)
static const uint8_t M_DISP    = 0xE7;  // Multicast: Displays
static const uint8_t M_RLS     = 0xE8;  // Rain/Driving Light Sensor (RLS)
static const uint8_t M_DSP_C   = 0xEA;  // DSP Controller [E38]
static const uint8_t M_VID     = 0xED;  // Video Module
static const uint8_t M_BMBT    = 0xF0;  // On-board Computer Control Panel (BMBT)
static const uint8_t M_SZM     = 0xF5;  // Center Console Switch Center (SZM), LKM2 [E31]
static const uint8_t M_BCAST   = 0xFF;  // Broadcast

// D-Bus module ID's
static const uint8_t M_ENG1   = 0x10;  // Engine Management
static const uint8_t M_ZKE12  = 0x11;  // Central Body Electronics (ZKE 1/2) [E31, E34]
static const uint8_t M_ENG2   = 0x12;  // Engine Management
static const uint8_t M_ENG3   = 0x13;  // Engine Management
static const uint8_t M_ENG4   = 0x14;  // Engine Management
static const uint8_t M_DDSHD  = 0x15;  // Double Sunroof (DDSHD) [E34]
static const uint8_t M_OIL    = 0x16;  // Thermal Level Oil Sensor [E36]
static const uint8_t M_RR     = 0x19;  // *Range Rover*
static const uint8_t M_EML70  = 0x20;  // Electronic Engine Power Control (EML) [M70]
static const uint8_t M_ZV     = 0x21;  // Central locking module [E34, E36]
static const uint8_t M_EML73  = 0x22;  // Electronic Engine Power Control (EML) [M73]
static const uint8_t M_MINI1  = 0x31;  // *MINI*
static const uint8_t M_EGS1   = 0x32;  // Gearbox Control
static const uint8_t M_LSM    = 0x35;  // Steering Column Memory (LSM) [E31/E32/E34]
static const uint8_t M_ABS1   = 0x36;  // ABS/ASC (Concept 1/2)
static const uint8_t M_ABS2   = 0x56;  // ABS/ASC/DSC (DS2)
static const uint8_t M_LWS    = 0x57;  // Steering Angle Sensor (LWS)
static const uint8_t M_IHKA_D = 0x59;  // Automatic Heating/Air Conditioning (IHKA) [E31, E36]
static const uint8_t M_ELV    = 0x5A;  // Electric Steering Lock (ELV) [E52]
static const uint8_t M_EKP    = 0x65;  // Fuel Pump (EKP)
static const uint8_t M_EGS2   = 0x6C;  // Gearbox Control
static const uint8_t M_OC3    = 0x74;  // Seat Occupation Detection US (OC3) [E83, E85]
static const uint8_t M_MINI2  = 0x81;  // *MINI*
static const uint8_t M_AHK    = 0x86;  // Active Rear Axle Kinematics (AHK) [E31]
static const uint8_t M_ROLL   = 0x9E;  // Rollover Sensor [E36]
static const uint8_t M_CC     = 0xA6;  // Cruise Control
static const uint8_t M_SVT    = 0xC2;  // Servotronic (SVT)
static const uint8_t M_OC     = 0xCE;  // Seat Occupancy Detection

#endif
