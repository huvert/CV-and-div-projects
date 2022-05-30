(*
  THIS IS A PART OF THE CODE INSIDE GX WORKS 2, AND IS ONLY A SMALL PART OF THE FULL PROGRAM.
*)

(*
Had to rename variables in GX-works in order to avoid errors.

Inputs:
	SP 		- 		reference / Setpoint
	PV 		- 		Process Value
	Kp  		- 		Propotional Gain
	Ti 		- 		Integration time
	Td  		- 		Derivation time
	n_filter	- 		Adjusting the filter constant 	tau = Td / n
	Tracking 	- 		Tracking input. Used when using more than 1 controller in paralell.

Outputs:
	MV 		- 		Modulated Value

Equations:
	MV	= P_gain  +  I_gain  +  D_gain
	P_gain	= Kp * e[k]
	I_gain	= KpT/(Ti*2)	* (e[k] + e[k-1]) 	+	i[k-1] 	
	D_gain	= 2KpTd/(2) 	* (y[k] -  y[k-1]) 	+	(2tau - T)/(2tau + T) * d[k-1]

Tracking:
	I_gain on the controller can be deactivated by setting Ti = 0.0.
	In order for Tracking to work - Ti needs to be higher than 0.0.
*)

IF ENABLE = 0 THEN
	MV 		:= 0;
	I_gain 		:= 0.0;
	I_gain_prev 	:= 0.0;
	Dev_prev 	:= Dev;
	PV_prev 	:= PV_float;
	D_gain_prev 	:= D_gain; 
	tracking_flag 	:= 0;
ELSE 		
	IF Kp = 0.0 THEN 	(* PID-controller is Deactivated -> Write LeadLag directly to output *)
		MV := FeedForward;
	ELSE
		(* Saturate input *)
		Aa := SP;  		(* Store to temporary variable for 2 reasons: 	1. Cant write new value to an input. *) 		
		FLT(1, PV, PV_float);	(*			          		2. Store space BY limiting number OF local variable definitions. *)
		IF Aa < MIN_INPUT THEN
			Aa := MIN_INPUT;
		ELSIF Aa > MAX_INPUT THEN
			Aa := MAX_INPUT;
		END_IF;
		
		IF PV_float < MIN_INPUT THEN
			PV_float := MIN_INPUT;
		ELSIF PV_float > MAX_INPUT THEN
			PV_float := MAX_INPUT;
		END_IF;
	
		(* Calculate deviation (avvik)	*)					
		Dev := Aa - PV_float;
		
		(* Calculate D 	*)
		IF Td = 0.0 THEN
			D_gain := 0.0;
		ELSE
			Aa 	:= 2.0*Kp*Td / (2.0*(Td/n_filter)+ SampleTime);
			Bb 	:= (2.0*(Td/n_filter)- SampleTime) / (2.0*(Td/n_filter)+ SampleTime);
			D_gain 	:= Aa * ( PV_float - PV_prev ) + Bb * D_gain_prev;
		END_IF;
		
		(* Calculate P 	*)
		P_gain 	:= Dev * Kp;
		
		(* Calculate I	*)
		IF Ti = 0.0 THEN 		(* I_gain can be deactivated by setting Ti to 0. *)
			I_gain := 0.0;
		ELSE 
			IF MV = Tracking THEN 	(* Note that this MV is from the last cycle and has not been updated YET *)
				Aa 	:= Kp * SampleTime / (2.0 * Ti);
				I_gain 	:= Aa * (Dev + Dev_prev) + I_gain_prev;
			ELSE
				(*This controller is not in control. Activate Tracking *)
				tracking_flag := 1;
				FLT(1, Tracking - FeedForward, Aa);
				I_gain := Aa - P_gain - D_gain ;	
			END_IF;
		END_IF;
		
		(* Write to output*)
		INT(1, (P_gain + I_gain + D_gain), PID_Padrag);

		IF NOT Inverse THEN
			MV := PID_Padrag+FeedForward+u_0; 	(* Normal output *)
		ELSE
			MV := -1*(PID_padrag+FeedForward+u_0); 	(* Inverse output *)
		END_IF;

	END_IF;
	
	(* Saturate output and check for windup *)
	IF MV <= MIN_OUTPUT THEN 		(* Negative Saturation *)
		IF Dev < 0.0 THEN 		(* Antiwindup *)
			I_gain := I_gain_prev;
		END_IF;
		MV := MIN_OUTPUT;
	ELSIF MV >= MAX_OUTPUT THEN 		(* Positive Saturation *)
		IF Dev > 0.0 THEN 		(* Antiwindup *)
			I_gain := I_gain_prev;
		END_IF;
		MV := MAX_OUTPUT;
	END_IF;
	
	(* Update previous values *)
	Dev_prev 	:= Dev;
	PV_prev 	:= PV_float;
	I_gain_prev 	:= I_gain;
	D_gain_prev 	:= D_gain; 
	tracking_flag := 0;
	
END_IF;
