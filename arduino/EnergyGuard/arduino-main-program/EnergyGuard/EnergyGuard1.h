
// ------------------------------------------------------------------------------------------------------------------

int CalculatePowerForHeater (long PowerL, int Power_Reg_L,  type_CalibData LmaxPower) 			// PowerL and PWM_L in Watt
{
  int P_L;

  P_L = (int) PowerL;

  if (P_L < 0)                                 //  ------------ Production of energy is surplus (exported) ------------
  {
    P_L = P_L * (-1);

    if (P_L > POWER_MARGIN)
    {

      Power_Reg_L = Power_Reg_L + P_L;         // Increase power on heater

      if (Power_Reg_L > LmaxPower.power)
        Power_Reg_L = LmaxPower.power;
    }
  }
  else                                         // ------------  Energy is imported  ------------
  {
    if (P_L < Power_Reg_L)
    {
      Power_Reg_L = Power_Reg_L - P_L - (POWER_MARGIN / 2);         // Decrease power on heater

      if (Power_Reg_L <= 0)
        Power_Reg_L = 0;
    }
    else
      Power_Reg_L = 0;                         // 0% of power on heater (off)
  }

  return Power_Reg_L;
}

// ------------------------------------------------------------------------------------------------------------------

int PowerMappingToPWMDuty(int Power_Reg_L, type_CalibData CalibData[16])

{
  int i;

  for ( i = 1; i < 16; i++)
  {
    if (CalibData[i - 1].power <= Power_Reg_L)
      if (CalibData[i].power > Power_Reg_L)
        return CalibData[i - 1].pwm;
  }
  if (CalibData[15].power <= Power_Reg_L)
    return CalibData[15].pwm;


  return 0; 			// 0 % of power (off)
}

// ------------------------------------------------------------------------------------------------------------------

void RunPowerRegulator (int PinNumber, int PWM_L_duty)
{
  analogWrite(PinNumber, PWM_L_duty);             // phase L for given pin number

}

// ------------------------------------------------------------------------------------------------------------------

int RunPomp500 (int PinNumber, int temp300, int temp500top, int PUMP_500)
{
  // Decision about running pomp for heating system.

  if (temp500top > (temp300 + 4))
    PUMP_500 = 1;

  if (temp500top < temp300)
    PUMP_500 = 0;

  if (temp500top > 50)            // 500 l pomp to prevent overheating
    PUMP_500 = 1;


  if (PUMP_500 == 1)
    digitalWrite(PinNumber, HIGH);
  else
    digitalWrite(PinNumber, LOW);

  return PUMP_500;

}

// ------------------------------------------------------------------------------------------------------------------

int RunPompCWU (int PinNumber, int tempPCoutCWU, int tempRedTop, int Pump_CWU)
{
  // Decision about running pomp for heating system.

  if (tempRedTop > (tempPCoutCWU + 4))
    Pump_CWU = 1;

  if (tempRedTop < tempPCoutCWU)
    Pump_CWU = 0;

  if (tempRedTop > 50)            // CWU l pomp to prevent overheating
    Pump_CWU = 1;


  if (Pump_CWU == 1)
    digitalWrite(PinNumber, HIGH);
  else
    digitalWrite(PinNumber, LOW);

  return Pump_CWU;

}

// ------------------------------------------------------------------------------------------------------------------

int RunPomp (int PinNumber, int isNoon)
{
  // Decision about running pomp for heating system.
  int PUMP_500L;

  if (temp500top > (temp300 + 4))
    PUMP_500L = 1;

  if (temp500top < temp300)
    PUMP_500L = 0;

  if (temp500top > 50)            // 500 l pomp to prevent overheating
    PUMP_500L = 1;


  if (PUMP_500L == 1)
    digitalWrite(PinNumber, HIGH);
  else
    digitalWrite(PinNumber, LOW);

  return PUMP_500L;

}


// ------------------------------------------------------------------------------------------------------------------


int RunSSR3p (int Power_Reg_L1, int Power_Reg_L2, int Power_Reg_L3, int SSR_3p)
{

  if (SSR_3p == 0)
    if (Power_Reg_L1 + Power_Reg_L2 + Power_Reg_L3 > 6100 )
      SSR_3p = 1;                  // switch on 3-phase SSR



  if (Power_Reg_L1 == 0)
    SSR_3p = 0;                        // switch off 3-phase SSR

  if (Power_Reg_L2 == 0)
    SSR_3p = 0;

  if (Power_Reg_L3 == 0)
    SSR_3p = 0;


  if (SSR_3p == 1)
    digitalWrite(HEATER_4_2KW, HIGH);
  else
    digitalWrite(HEATER_4_2KW, LOW);

  return SSR_3p;
}

