            -=================================-
			RAT
            -=================================-

                      Version 0.1
              README updated on 04-20-2010

   Copyright © bendre1/sharma30. 2010 All Rights Reserved.
       



| Table of Contents
+===================-

   1. Introduction
   2. System Requirements
   3. Installation
   4. Usage
   5. Feedback & Support
   6. Credits


| 1. Introduction
+=================-

   RAT - Relational Algebra Translator

   RAT allows you to convert nearly all relational algebra queries in one step to . Why would you want to do that? Well, the DivX Media Format can change the way you play,create and share digital video.

   Relational algebra is a query language that is being used to explain basic relational operations and their principles. Many books and articles are concerned with the theory of relational algebra; however, there are few available tools that allow the students to have hands on experience with the relational algebra queries. Most of the currently used relational database management systems work with SQL queries. This work therefore describes and implements a tool that transforms relational algebra expressions into SQL queries, allowing the expressions to be evaluated in standard databases. This also allows the user to integrate the queries to various DBMS engines and have direct interaction with the database using relational algebra queries.



| 2. System Requirements
+========================-

   PIV 1Ghz or better
   Linux OS 2.6 and higher
   20MB free space for application
   qt runtime env
   qt-sdk (if compiling from sources)
   qt runtime (if using precompiled bianry)
   qt db drivers (for required dbs)


| 3. Installation
+===================================-
   
 Using sources:
 1. Go to application folder.
 2. Run qmake command (you will need QT installed in your machine for this).
 3. Run make.
 4. Run the generated rat binary file to start the application.
 
 
 Using binaries:
 A. For MS Windows
 	1. Download the self extrating archive from https://github.com/forward-uiuc/rasql/releases/download/0.1/rasql.exe
 	2. Run the downloaded file to extact the application.
	3. Specify a directory to extrat the files. 
	4. Run rqsql.exe from the extrated files.
 
 B. For Mac OS
 	1. Download the disk image from https://github.com/forward-uiuc/rasql/releases/download/0.1/rat.dmg
	2. Mount the image by double cliking on it.
	3. Drag the Rat application from the image to the Applications folder. 
	4. Run the Rat application.



| 4. Usage
+===================================-
   
   The RAT Overview document contains details about the usage of RAT application. Please refer that document for details.
   




| 5. Feedback & Support
+=======================-

   Be sure to read the RAT Overview and FAQ as it covers all aspects of the RAT application and should answer most of your questions. Please post all feedback, questions, problems, etc to our IDs.

   This is a beta version of the software. We will like to improve on this and want to create a full-proof software that will make the DBMS students' life a lot easier. :-) 
   Feel free to contact us at Mangesh Bendre <bendre1@illinois.edu>, Nilam Sharma <sharma30@illinois.edu> whenever you will find any bug (You may find many :-( ).

   Mangesh Bendre <bendre1@illinois.edu>, 
   Nilam Sharma <sharma30@illinois.edu>


| 6. Credits
+============-

   RAT Parser uses GOLD Parser application to create the DFA grammer needed for parsing.

   RAT also uses the QT Framework for development of the FE application.


   Thank you for using RAT.

   Mangesh Bendre
   Nilam Sharma
   2 Idiots
