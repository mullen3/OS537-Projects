# Operating Systems 537
Here is a collection of all my classwork for OS537, taken in Fall 2011 with Remzi Arpaci-Dusseau
http://pages.cs.wisc.edu/~remzi/Classes/537/Fall2011/

Since it has been 3 years since I've touched most of this, it's a mess... I probably won't fix it.

My partner was Brian Anderson for all of these projects.

## Projects
### 1a - FastSort
My first real introduction to C. There wasn't really much work on this, I just read in a file and used qsort with a pointer to my comparator. Link to the project description for those interested: http://pages.cs.wisc.edu/~remzi/Classes/537/Fall2011/Projects/p1a.html
### 1b - XV6 Intro
This project was just to introduce us to our XV6 hacking environment (a little unix-like kernel). We simply implemented a syscall that returned the number of system calls that had been made. http://pages.cs.wisc.edu/~remzi/Classes/537/Fall2011/Projects/p1b.html
### 2a - Shell
In this project, we implemented a simple unix shell. The primary purpose of this project was to familiarize ourselves with process creation, distruction, and management. http://pages.cs.wisc.edu/~remzi/Classes/537/Fall2011/Projects/p2a.html
### 2b - Scheduling
Now the fun really begins! In this project, we built a stride scheduler (in case you're interested: http://www.cs.wisc.edu/~remzi/OSFEP/cpu-sched-lottery.pdf). This was also when I really started becoming familiar with GDB. http://pages.cs.wisc.edu/~remzi/Classes/537/Fall2011/Projects/p2b.html
### 3a - Malloc and Free
This project was particularly difficult. We were tasked with building our own shared performance-efficient library for memory allocation... Awesome. We used a worst-fit algorithm to manage the free list of memory. http://pages.cs.wisc.edu/~remzi/Classes/537/Fall2011/Projects/p3a.html
### 3b - The Null Pointer
The XV6 kernel we were given had no Null Pointer errors, so we added one! In all honesty, my partner did a lot of the work on this project as I was in debug hell with the Malloc project. This project required an intimate understanding of how XV6's two-level page table worked. http://pages.cs.wisc.edu/~remzi/Classes/537/Fall2011/Projects/p3b.html
### 4 - Threads!
This project was awesome and a beast. We implemented kernel threads in our XV6 hacking environment. We created a system call 'clone' to build our threading library ontop of. Then created a library with functions thread_create(), thread_join(), lock_acquire(), and lock_release(). You can imagine how many hours I spent in the lab on this one. One of my favorite projects of CS. http://pages.cs.wisc.edu/~remzi/Classes/537/Fall2011/Projects/p4.html
### 5 - Extent Based Files
Stupid XV6 file system! The original implementation could only create really simple files with data blocks and an indirect block for larger files. We fixed this by implementing extent-based files. Pretty much, we filled the direct blocks with pointer, length pairs for the location of the data blocks. This was a fun project, and sadly our last in the XV6 environment. You will be missed! http://pages.cs.wisc.edu/~remzi/Classes/537/Fall2011/Projects/p5.html
### 6 - Distributed File Server
Let's play with sockets! We were given some bare bones UDP code and asked to build a distributed file server with it. We implemented a (virtual) log-structured file system for the server and some small client code for reading. I mainly worked on building the server side (filesystems are hard). Fun stuff! http://pages.cs.wisc.edu/~remzi/Classes/537/Fall2011/Projects/p6.html
