# Opearting Systems 537
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

