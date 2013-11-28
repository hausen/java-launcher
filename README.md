java-launcher
=============

A simple launcher for JAR files that creates linux executables. It is
possible to embed the JAR file in the executable.

The generated executable searches for a suitable JRE installed in the
system. If one JRE cannot run the file (which may happen for many
reasons, such as: the classes were compiled with a newer JDK, the
default JRE installed in the system is headless, etc.) the launcher
tries every other JRE it may find on the system, first via the JAVA_HOME 
environment variable, second via the PATH variable, then it searches
/usr/lib/jvm, and at last it tries to find a suitable JVM via
update-alternatives (if it is available).

Building
--------

Edit Makefile and set JARFILE accordingly. If you wish to embed
the JAR file in the executable, read the instructions in the makefile.
