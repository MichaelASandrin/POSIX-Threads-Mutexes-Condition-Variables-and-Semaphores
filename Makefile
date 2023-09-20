
output: new_alarm_cond.o #creates executable for every changes to object file
	cc new_alarm_cond.c -D_POSIX_PTHREAD_SEMANTICS -lpthread -o out

My_Alarm.o: new_alarm_cond.c #recompile object file without creating executable
	cc -c new_alarm_cond.c

clean: #remove all .out files
	rm *.out