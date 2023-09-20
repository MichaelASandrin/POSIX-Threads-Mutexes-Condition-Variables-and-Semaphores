1. There are two possible ways to running the program listed below:

1.1 The file can be ran by using the Terminal to compile the file, and running the "a.out" where you can provide your own user inputs

	Step 1: Open the Terminal and enter the directory that contains "New_Alarm_Cond.c" and the rest of Assignment 3
	
	Step 2: In the Terminal, input the following line to create the executable (a.out)

		cc New_Alarm_Cond.c -D_POSIX_PTHREAD_SEMANTICS -lpthread

	Step 3: In the Terminal, input the following line to run the executable (a.out)

		a.out

	Now the program is running and your input can be typed into the Terminal itself



1.2 The file can also be ran by using the makefile provided to run a premade sequence of inputs for the user.

	Step 1: Ensure the makefile is in the same directory as "New_Alarm_Cond.c" and the rest of Assignment 3

	Step 2: Open the Terminal and enter the directory that contains "New_Alarm_Cond.c" and the makefile

	Step 3: Input the following into the Terminal

		make



1.3 NOTE to terminate the program using either method of running the program (1.1 & 1.2), press both CTRL + C keys on your keyboard



2. If the program successfully compiles, the Terminal will display "ALARM>". With this, the following format can be inputted 
   in the following arrangement:

2.1 To create an Alarm, the user must input the following respectively in the Terminal:

	Alarm> <Time> Message(<Message Number>): <Message>

	Where: 
		- Time: Seconds for the timer to elapse
		- Message Number: Positive Integer that acts as the identification for the Alarm
		- Message: String that acts as a "message" to the user

	Example:  Alarm> 5 Message(2) Buy groceries at Metro

     After, the following line is displayed in the Terminal:

	Successfully added



2.2 To Cancel an existing Alarm, the user must input the following respectively in the Terminal:

	Alarm> Cancel: Message(<Message Number>) <Message>

	Where:
		- Message Number: Positive Integer that acts as the identification for the Alarm
		- Message: String that acts as a "message" to the user

	Example:  Cancel: Message(2) 



3. The Outputs given to the user via the Terminal is shown below:

3.1 When the alarm is finished (time has fully elapsed) the following is displayed in the Terminal:
	
	Example: After 5 minutes has elapsed (Based on the example from 2.1)

		Message (2) Buy groceries at Metro



3.2 When the Cancel input has been typed into the Terminal, instantaneously the following occurs but no outputs are present

	Example: The user uses the line from example 2.2 to cancel Message (2) and ends the respective alarm thread process
		 


3.3 When an alarm is added (2.1) and the message is greater than 128 characters long, the message is truncated and removes any
    characters after the 128th character and results in the following output:

	Example Input: Alarm> 15 Message(4) Buy groceries at Sobeyssssssssssssssssssssssssssssssssssssssssssssssssssssssssssss
		       sssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssss
		       sssssssssssssssssssssssssssssssssssssss

		Output: Successfully added
			Bad command

 *after 15 seconds*     Message(4) Buy groceries at Sobeysssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssss
			ssssssssssssssssssssssssss  



3.4 When multiple threads are added, the same outputs will occur as shown in 3.1, but if four threads exist, attempting to add
    another alarm thread will result in no alarm being created and the following output:

	Example Input: Alarm> 22 Message(5) Buy groceries at Food Basics

		Output: Buffer is full



		



 