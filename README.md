# Concurrent Programming and Synchronization Mechanisms in C

This repository hosts an assignment project which implements concurrent programming and synchronization mechanisms in C. The project simulates a news broadcasting scenario where different types of stories are produced, sorted and displayed to the public.
<img src="https://github.com/ArielAsh1/News-Broadcasting/assets/112930532/e150ce3b-093b-4fbf-b968-c942c19da56e" width="400">

## Scenario
We simulate different types of 'news stories' using simple strings which are displayed to the screen in the order they arrive. There are four types of active actors in this simulation:

1. Producers
2. Dispatcher
3. Co-Editors
4. Screen-manager

#### Producers
A producer creates a number of strings in a specific format and passes its information to the Dispatcher via its own private queue. 

#### Dispatcher
The Dispatcher continuously accepts messages from the Producers queues. It then sorts each message and inserts it into one of the Dispatcher queues which includes strings of a single type.

#### Co-Editors
Each type of possible message has a Co-Editor that receives the message through the Dispatcher's queue. The Co-Editor "edits" it, and passes it to the screen manager via a single shared queue.

#### Screen-manager
The Screen-manager displays the strings it receives via the Co-Editors queue to the screen (standard output). 

#### System Design
Three producers communicate with the dispatcher via their Producer queues. The Dispatcher communicates with the Co-Editors via three queues corresponding to the three types of messages. The Co-Editors communicate with the Screen-Manager via a single shared queue, and the Screen manager displays the system's output.
