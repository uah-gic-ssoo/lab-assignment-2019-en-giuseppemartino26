#include <stdio.h>

#include <os.h>
#include <sched.h>

/** Pointer to the Ready Task Queue */
extern TaskQueue_t * readyQueue;

/** Pointer to the Hard Disk Waiting Task Queue */
extern TaskQueue_t * hardDiskWaitingQueue;

/** Pointer to the Keyboard Waiting Task Queue */
extern TaskQueue_t * keyboardWaitingQueue;

#define TIMESLICE 2

PCB_t * schedule() {

    // Return the first element of the ready queue
    return extractFirst(readyQueue);

}

void startTask(PCB_t * pcb) {

    // TODO: Complete the function
    // runningTask = task that is currently being executed or NULL if no task
    // is currently running on the CPU
    PCB_t * runningTask = getRunningTask();

    // Check if there was already a task in execution

    if (runningTask != NULL) {

        // If there was a task already running, put the new one on ready
        // state and append it to the end of the ready queue
        setState(pcb, READY);
        appendPCB(readyQueue, pcb);

    } else {

        // If there was no task previously running, set the new one to running
        // state and dispatch it to the CPU
        setState(pcb, RUNNING);
	setTimeslice(pcb, TIMESLICE);
        dispatch(pcb);

    }

}


void exitTask(PCB_t * pcb) {

    // TODO: Complete the function
    PCB_t * nextToRun = NULL;

    // Set the exit task to finished state
    setState(pcb, FINISHED);

    // Get the next task to run
    nextToRun = schedule();

    // Check if there was a candidate to running state
    if (nextToRun != NULL) {

        // If there is a candidate, set it to running state and dispatch it to
        // the CPU
        setState(nextToRun, RUNNING);

	setTimeslice(nextToRun, TIMESLICE);

        dispatch(nextToRun);

    }


}



void clockTick(PCB_t * pcb) {

    PCB_t * nextToRun = NULL;

    // Check if there is a task currently running on the CPU
    if (pcb != NULL) {

        // If there is a task currently running, its timeslice must be updated
        unsigned int timeslice = getTimeslice(pcb);

        timeslice = timeslice - 1;

        setTimeslice(pcb, timeslice);

        // Check if its quantum has expired
        if (timeslice == 0) {

            // If its quantum has expired, put the task to ready state and
            // append it to the end of the ready queue
            setState(pcb, READY);
            appendPCB(readyQueue, pcb);

            // Check if there is a candidate for running on the CPU
            nextToRun = schedule();

            if (nextToRun != NULL) {

                // If there is a candidate, then set it to running state,
                // initialize its timeslice and dispatch it to the CPU
                setState(nextToRun, RUNNING);
                setTimeslice(nextToRun, TIMESLICE);

                dispatch(nextToRun);

            }

        }

    }

    return;

}

void yieldHardDisk(PCB_t * pcb) {

    // TODO: Complete the function
    PCB_t * nextToRun = NULL;

    // Set the task to waiting state
    setState(pcb, WAITING);

    // Check if there was already a task using the hard disk
    if (getHardDiskWaitingTask() != NULL) {

        // If there was a task, enqueue the current one on the hard disk
        // waiting queue
        appendPCB(hardDiskWaitingQueue, pcb);

    } else {

        // There was not any task using the hard disk, so make the current
        // task to use it
        programHardDisk(pcb);

}
	nextToRun = schedule();

    // Check if there is a candidate to running state
    if (nextToRun != NULL) {

        // If there is a candidate, set it to running state and dispatch it to
        // the CPU
        setState(nextToRun, RUNNING);

	setTimeslice(nextToRun, TIMESLICE);

        dispatch(nextToRun);


}

}

void ioHardDiskIRQ(PCB_t * pcb) {

    // TODO: Complete the function

	// An IO operation on the hard disk has finished. First, we need to
    // check if there were tasks waiting for the previous operation to
    // finish
    PCB_t * waiting = extractFirst(hardDiskWaitingQueue);
    PCB_t * runningTask = getRunningTask();

    if (waiting != NULL) {

        // If there were a task waiting for the previous operation to
        // finish, then program it to use the hard disk
        programHardDisk(waiting);

    }

    // Check if there was already a task in execution
    if (runningTask != NULL) {

        // If there was a task already running, put this one on ready
        // state and append it to the end of the ready queue
        setState(pcb, READY);
        appendPCB(readyQueue, pcb);

    } else {

        // If there was no task previously running, set this one to running
        // state and dispatch it to the CPU
        setState(pcb, RUNNING);

        setTimeslice(pcb, TIMESLICE);

        dispatch(pcb);


    }


}

void yieldKeyboard(PCB_t * pcb) {

    // TODO: Complete the function
    
    PCB_t * nextToRun = NULL;

    // Set the task to waiting state
    setState(pcb, WAITING);

    // Check if there was already a task waiting for the keyboard
    if (getKeyboardWaitingTask() != NULL) {

        // If there was a task, enqueue the current one on the keyboard
        // waiting queue
        appendPCB(keyboardWaitingQueue, pcb);

    } else {

        // There was not any task waiting for the keyboard, so make the
        // current task to wait for it
        programKeyboard(pcb);

    }

    // Since the task has abandoned the CPU, we need to select another one to
    // run
    nextToRun = schedule();

    // Check if there is a candidate to running state
    if (nextToRun != NULL) {

        // If there is a candidate, set it to running state and dispatch it to
        // the CPU
        setState(nextToRun, RUNNING);


        setTimeslice(nextToRun, TIMESLICE);

	dispatch(nextToRun);

    }

}

void ioKeyboardIRQ(PCB_t * pcb) {

    // TODO: Complete the function
    // An IO operation on the keyboard has finished. First, we need to
    // check if there were tasks waiting for the previous operation to
    // finish
    PCB_t * waiting = extractFirst(keyboardWaitingQueue);
    PCB_t * runningTask = getRunningTask();

    if (waiting != NULL) {

        // If there were a task waiting for the previous operation to
        // finish, then program it to wait for the keyboard
        programKeyboard(waiting);

    }

    // Check if there was already a task in execution
    if (runningTask != NULL) {

        // If there was a task already running, put this one on ready
        // state and append it to the end of the ready queue
        setState(pcb, READY);
        appendPCB(readyQueue, pcb);

    } else {

        // If there was no task previously running, set this one to running
        // state and dispatch it to the CPU
        setState(pcb, RUNNING);
	setTimeslice(pcb, TIMESLICE);
        dispatch(pcb);

    }

}
