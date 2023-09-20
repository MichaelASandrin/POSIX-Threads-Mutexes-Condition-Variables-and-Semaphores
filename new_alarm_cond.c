/*
 * EECS 4421 Section E
 * Instructor: Jia Xu
 * Date of Completion: Tuesday, December 6th, 2022
 * Assignment 3
 * Group Members:
 * Victor Buica
 * Michael Sandrin
 * Alain Ballen
 * Brandon La
 *
 *
 * new_alarm_cond.c
 *
 * This is an enhancement to the alarm_mutex.c program, which
 * used only a mutex to synchronize access to the shared alarm
 * list. This version adds a condition variable. The alarm
 * thread waits on this condition variable, with a timeout that
 * corresponds to the earliest timer request. If the main thread
 * enters an earlier timeout, it signals the condition variable
 * so that the alarm thread will wake up and process the earlier
 * timeout first, requeueing the later request.
 */
#include <pthread.h>
#include <time.h>
#include "errors.h"
#define BUFFERSIZE 4

/*
 * The "alarm" structure now contains the time_t (time since the
 * Epoch, in seconds) for each alarm, so that they can be
 * sorted. Storing the requested number of seconds would not be
 * enough, since the "alarm thread" cannot tell how long it has
 * been on the list.
 */
typedef struct alarm_tag {
    struct alarm_tag    *link;
    int                 seconds;
    time_t              time;   /* seconds from EPOCH */
    char                message[128]; // Changed from 64 to 128
    int                 messageNumber; // Message Number for two new alarm requests
    char                cancel[128]; // Keyword to cancel
} alarm_t;

//Display Thread Method
typedef struct display_thread {
int messageNumber;
int seconds;
char message[128];
pthread_t thread;
} thread_struct;

//Creation of all Threads
pthread_mutex_t alarm_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t alarm_cond = PTHREAD_COND_INITIALIZER;
pthread_cond_t nalarm_cond = PTHREAD_COND_INITIALIZER;

//Values need for alarms and their respective list
alarm_t *alarm_list = NULL;
time_t current_alarm = 0;
int current_number = 0;
int current_seconds = 0;
int cancelled = 0;
int counter = 0;
alarm_t alarm_listTest[100];
thread_struct thread_list[100];
void alarm_cancel (alarm_t *alarm);
void alarm_insert (alarm_t *alarm);
void *alarm_thread (void *arg);
void *pd_thread (alarm_t *alarm);

//Values based for the Circular Buffer
alarm_t buffer[4];
int head = 0;
int tail = 0;
int fullFlag = 0;

/*
 * Insert alarm entry on list, in order.
 */
void alarm_insert (alarm_t *alarm){
    int status;
    alarm_t **last, *next;
    alarm_listTest[counter] = *alarm;
    counter++;
    /*
     * LOCKING PROTOCOL:
     *
     * This routine requires that the caller have locked the
     * alarm_mutex!
     */
    last = &alarm_list;
    next = *last;
    while (next != NULL) {
        if (next->time >= alarm->time) {
            alarm->link = next;
            *last = alarm;
            break;
        }
        last = &next->link;
        next = next->link;
    }
    /*
     * If we reached the end of the list, insert the new alarm
     * there.  ("next" is NULL, and "last" points to the link
     * field of the last item, or to the list header.)
     */
    if (next == NULL) {
        *last = alarm;
        alarm->link = NULL;
    }
#ifdef DEBUG
    printf ("[list: ");
    for (next = alarm_list; next != NULL; next = next->link)
        printf ("%d(%d)[\"%s\"] ", next->time,
            next->time - time (NULL), next->message);
    printf ("]\n");
#endif
    /*
     * Wake the alarm thread if it is not busy (that is, if
     * current_alarm is 0, signifying that it's waiting for
     * work), or if the new alarm comes before the one on
     * which the alarm thread is waiting.
     */
    if (current_alarm == 0 || alarm->time < current_alarm) {
        current_alarm = alarm->time;
        status = pthread_cond_signal (&alarm_cond);
        if (status != 0)
            err_abort (status, "Signal cond");
    }
}

/*
 * The alarm thread's start routine.
 */
void *alarm_thread (void *arg){
    alarm_t *alarm;
    struct timespec cond_time;
    time_t now;
    int status, expired;

    /*
     * Loop forever, processing commands. The alarm thread will
     * be disintegrated when the process exits. Lock the mutex
     * at the start -- it will be unlocked during condition
     * waits, so the main thread can insert alarms.
     */
    status = pthread_mutex_lock (&alarm_mutex);
    if (status != 0)
        err_abort (status, "Lock mutex");
    while (1) {
        /*
         * If the alarm list is empty, wait until an alarm is
         * added. Setting current_alarm to 0 informs the insert
         * routine that the thread is not busy.
         */
        current_alarm = 0;
        while (alarm_list == NULL) {
            status = pthread_cond_wait (&alarm_cond, &alarm_mutex);
            if (status != 0)
                err_abort (status, "Wait on cond");
            }
        alarm = alarm_list;
        alarm_list = alarm->link;
        now = time (NULL);
        expired = 0;
        if (alarm->time > now) {
#ifdef DEBUG
            printf ("[waiting: %d(%d)\"%s\"]\n", alarm->time,
                alarm->time - time (NULL), alarm->message);
#endif
            cond_time.tv_sec = alarm->time;
            cond_time.tv_nsec = 0;
            current_alarm = alarm->time;
            while (current_alarm == alarm->time) {
                status = pthread_cond_timedwait (
                    &alarm_cond, &alarm_mutex, &cond_time);
                if (status == ETIMEDOUT) {
                    expired = 1;
                    break;
                }
                if (status != 0)
                    err_abort (status, "Cond timedwait");
            }
            if (!expired)
                alarm_insert (alarm);
        } else
            expired = 1;
        if (expired) {
            printf ("Message(%d) %s\n", alarm->messageNumber, alarm->message);
            free (alarm);
        }
    }
}

//Thread to Cancel the Alarm Thread
void alarm_cancel (alarm_t *alarm){

	//Values for the alarms and their respective index in the list
	int status;
	alarm_t **last, *next, *temp, *temp2;
	last = &alarm_list;
	next = *last;
	temp = next;
	int j;

	//For Loop to search through the alarm lists
	for( j = 0; j < counter; j++){
       //printf("Enter loop check with thread list value %d...\n", alarm_listTest[j].messageNumber);


	//Locates the alarm from the List
	if (alarm_listTest[j].messageNumber == alarm->messageNumber){

            //printf("In if statement...\n");

			//Calls on the cancel function to cancel the alarm thread called on
			pthread_cancel(thread_list[j].thread);

			//Sets the value in the list to be vacant
			thread_list[j].messageNumber = 0;
			//printf("Display thread exiting at %ld: %d Message(%d) %s\n", time(NULL), thread_list[j].seconds, alarm->messageNumber, alarm_listTest[j].message);
			break;

		}
	}

	while (next != NULL) {

		//Changes the pointers to the respective variables if the list and the alarm share the same message number (essentially share the same identifications)
		if (next->messageNumber == alarm->messageNumber){

			if(next->link != NULL) {

				temp->link = next->link;
				next = NULL;
				*last = temp->link;

			}

			//Changes the pointers to the respective variables if the temporary list saved register and the alarm do not share the same message number
			else if (temp->messageNumber != alarm->messageNumber){

				next = NULL;
				temp->link = NULL;
			}

			//If they do not share any of the same message numbers, make the entire alarm list vacant
			else {

				alarm_list = NULL;

			}

			break;

		}

		last = &next->link;
		temp = next;
		next = next->link;
	}

	//If the current number is equal to the current alarm's message number, the thread is unblocked and its current status shows that
	if(current_number == alarm->messageNumber){

		status = pthread_cond_signal (&alarm_cond);

		if (status != 0)
			err_abort (status, "Signal cond");

	}
}

//Initial buffer for the circular buffer which initializes the buffer to work properly
void initBuffer(int *buffer) {
    buffer = (alarm_t*) malloc(sizeof(alarm_t) * BUFFERSIZE);
}

//Circular Buffer Thread method
int circularBuffer_add(alarm_t* request) {
    alarm_t temp = *request;

    if(fullFlag == 1)
        return -1; //buffer is full

    buffer[head] = temp;
    head = (head + 1) % 4;

    if(head == tail)
        fullFlag = 1;

    return 0; //alarm request successfully added
}

//Consumer Thread method
void consumer_thread(alarm_t *alarm){

	//Tenorary operation to print to the user if there is any space in the buffer or if there isnt, add the new alarm to the buffer
	int status_circular = circularBuffer_add(&alarm);
            status_circular == 0 ?
                printf("Successfully added\n") :
                printf("Buffer is full\n");

}

//Main Method
int main (int argc, char *argv[]){
    int status;
    char line[128];
    alarm_t *alarm;
    pthread_t thread;

    //initialize buffer
    initBuffer(&buffer);

    status = pthread_create (
        &thread, NULL, alarm_thread, NULL);
    if (status != 0)
        err_abort (status, "Create alarm thread");
    while (1) {
        printf ("Alarm> ");
        if (fgets (line, sizeof (line), stdin) == NULL) exit (0);
        if (strlen (line) <= 1) continue;
        alarm = (alarm_t*)malloc (sizeof (alarm_t));
        if (alarm == NULL)
            errno_abort ("Allocate alarm");

        //If the user inputs the respective cancel message, the function alarm_cancel is called
        if (sscanf (line, "Cancel: Message(%d)", &alarm->messageNumber) == 1){


		//fprintf (stderr,"Message(%d) has been Cancelled [TEST MESSAGE]\n", alarm->messageNumber);
		status = pthread_mutex_lock (&alarm_mutex);

		//Calls to cancel the current alarm
		alarm_cancel(alarm);


	// Checks to see if there are any errors or if the message is greater than 128 characters long
        } else if (sscanf (line, "%d %*[^0123456789]%d) %128[^\n]",
            &alarm->seconds, &alarm->messageNumber, alarm->message) < 3){
            fprintf (stderr, "Bad command\n");
            free (alarm);

       } else {
            status = pthread_mutex_lock (&alarm_mutex);
            if (status != 0)
                err_abort (status, "Lock mutex");
            alarm->time = time (NULL) + alarm->seconds;
            /*
             * Insert the new alarm into the list of alarms,
             * sorted by expiration time.
             */
            alarm_insert (alarm);

		//Calls on consumer thread to run circular buffer
		consumer_thread(alarm);

            status = pthread_mutex_unlock (&alarm_mutex);
            if (status != 0)
                err_abort (status, "Unlock mutex");
        }
    }
}
