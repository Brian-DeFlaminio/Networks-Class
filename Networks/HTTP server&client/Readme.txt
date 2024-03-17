Hi!

So in order to compile this I just used "gcc HTTPServer.c -o HTTPServer" & "gcc HTTPClient.c -o HTTPClient" 
I tried to use a makefile but it comes up with errors that gcc doesn't and it still works as expected. 
I asked the professor if that was ok to have a faulty makefile and he said that's fine. Lmk if you want to see the email about it.

When you run the two files with "./HTTPServer {port}" and "./HTTPClient -p {address} {port}" respectively the client will return the response in the terminal and a "Response" file.

Examples of inputs that I tested are:
./HTTPServer 6667
./HTTPClient -p www.google.com 80
./HTTPClient -p linux.wpi.edu 6667
./HTTPClient -p linux.wpi.edu/TMDG.html 6667

Sometimes if the program exits with an error the port doesn't close so just try a different number if the bind() fails after a mistype or something.

-Brian DeFlaminio
bmdeflaminio@wpi.edu