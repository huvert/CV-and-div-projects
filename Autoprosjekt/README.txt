This was one of 4 subjects one semester.

Theres alot more documentation to this project. Including the code itself.
For more documentation, send me an email at:
	haavard.bonde@hotmail.com

or call me at 
	+47 901 54 532
	
	
===========================================================
==========	PID Controller Description 	===========
===========================================================
The PID-controller is created with the help of discrete mathematics. See report.
It has the following inputs:

- ENABLE 		Enables block
- SP 			Setpoint
- PV 			Process Value
- Kp 			Propotional Gain
- Ti 			Integration time
- Td 			Derivation time
- n_filter 		Adjusting the filter constant 	tau = Td / n
- SampleTime 		Sampletime / cycletime. VERY important that the SampleTime is predictable!
- FeedForward 		Input from FeedForward controller (ex: LeadLag). 0 if deactive.
- u_0 			Nominal "Pådrag".
- Inverse 		Inverts output (0 ... 100) --> (100 ... 0)
- Tracking 		Tracking input. Used when using more than 1 controller in paralell.

The PID Controller accounts for Antiwindup from I-part of controller.
The PID Controller avoids derivate-kick.
The PID Controller can be hooked up to a FF-controller.
The PID Controller has a tracking-input which enables smooth transitions between controllers. 
This can also be used to smooth transition between Manual --> Auto, Auto --> Manual, or changing of paramter-inputs such as Kp, Ti, Td ... 
