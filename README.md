IOS Streaming Browser
=============

Welcome to IOS-Streaming Browser!This project provides you (the developer) with an embedded HTTP server which steams the web browser's view as screen captures to remote users and devices.  This is a port of the CocoaHTTPServer, and it designed primarily for students, teachers, and learning environments.  The intent is to allow students to develop content and briefings, and then broadcast the presentation to a main projector.It can also be used in small group environments, to steam the content directly to other mobile devices.Why is this needed, and why use this application       
-------------
This application is needed for two reasons.  1.  Schools have designed their network to filter content at the network level, however, as they attempt to integrate mobile devices, they will need to ensure content is filtered on whatever network the devices is connected to.  For example, if the students are allowed to take the device home, a school may still be responsible for the content which the students access on the device.   2.  The second reason this application is needed, is to have the ability to view a students content if needed.  This feature is available via the embedded HTTP server installed within the application.  This allows teachers to view a single iPad/iTouch, or multiple devices at the single time.  This feature provides an added layer of protection on top of the browser filtering capability, and can also be utilized to allow students to broadcast a presentation to other students or a teacher's projector.

Status of development
-------------
5/14/11 -   Redid the core of the project to match the current CocoaHTTPServer version.  The previous version was based on an old CocoaHTTPServer version which had memory leaks.5/16/11 - Restricted the editing of the address bar, and added controls to utilize Google strict filtering.  This is a simple filter to prevent users from viewing inappropriate content.  Note: Additional filters need to be added.5/17/11 - Added feature to restrict Google and UTube searches to 'safe' mode.  
5/25/11 - Continuing to comment the Grand Central Dispatch Asychronous Socket files

6/5/11 - Continuing commenting files and added doxygen documentation
6/9/11 - Added auto rotation for the screen views
6/12/11 - Added configuration for user preferences