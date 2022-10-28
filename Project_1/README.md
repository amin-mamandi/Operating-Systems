# Quash Project

Name: Amin Mamanidpoor 

EECS678-Fall22


## Summary:
Quash is a command-line interface similar to bash. It is equivalent to say Quash as a parsing system in that
it attempts to parse the inbound string from its terminal or a file. The approach that is followed in developing
the Quash is to get the user input, determine if pipes are required, tokenize the input based on the defined
delimiters (for instance <,>, “ ”), determine the command, and call the respective function. I used C to
implement this project.

## Building Instructions

```
$ make
//to fire-up the quash
$ ./quash  
```

### Execution instruction for command
Built-in command:
#### Command cd:
```
cd .. -- to navigate to the previous folder in the path
cd (args) -- to navigate to the desired folder in the pathCommand 
```
#### Command pwd:
```
pwd --retuns the current directory
```
#### Command export:
```
export PATH=[path]-- will set the path to PATH variable
```
#### Command exit and quit:
```
exit -- to termiante the Quash
quit -- to terminate the Quash
```
#### Command echo:
```
echo $HOME -- will print the HOME env variable
echo  String_1 "String_2"  -- will print String_1 String_2
```
#### File redirections:
```
Reading commands from file
./prog < [textfilename]-- will read the commands, executes and exits
e.g ./prog < a.txt
quash < [textfilename] --will read the command, executes and quash stays interactive
e.g quash < a.txt
Writing to a file
[command] > [fiename] -- will redirect the out put to a textfile in name as provided by the user
e.g ls > output.txt
```

#### Pipes:
```
Runnig pipes 
e.g cat quash.c | grep Welcome -- will print out the line contains "Welcome"
```
#### Command jobs 
```
jobs-- prints the running background processes details (JobID, ProcessID, and Command
associated with the job)
```
#### Background execution
```
[command] & -- will run the job in the background (note no space between the command and &)
```
#### Command kill
```
kill SIGNUM PID -- will send the signal to the process with the given ProcessID
```
