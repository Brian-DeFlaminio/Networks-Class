Hi!
So in order to run the code you can just use make in the terminal, if for some reason that doesn't work
gcc -o p2 student2.c project2.c will work

My makefile calls the executeable p2 so ./p2 < input.txt will save some typing for you.
I made input.txt have 100 messages, .5 on each corruption modifier, 10000 time, and randomization is on, so that one is worst case scenario from what prof described in the video.

simpleInput.txt is 100 messages with no randomization or corruption (still behaves weirdly when I run it)

and the high_loss, high_corruption, high_out_of_order text files just use .5 for the modifier named, 100 messages and randomization is on

outputExample.txt is the output I saved from a better run of input.txt, 25 messages were wrong but 75 were correct. I also included it as a PDF as the deliverables requirement says, but it may be more readable as a txt file, idk what the 

If when you run the code it has a lot of them wrong, I'd ask that you run it a few more times and take an average, I wasn't able to make it consistently good so I figured an average would be fair as some inputs are perfectly randomized to lose a ton of messages.