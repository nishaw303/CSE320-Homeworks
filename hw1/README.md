# HW #1

You are given some code. The main purpose of code handout is to show you expected output but feel free to ask on Piazza in case of concerns, however, it should be more or less clear from the homework document.

Using GCC is not complicated but we slowly will start to use Makefiles. By the link you can find a tutorial
http://mrbook.org/blog/tutorials/make/
I recommend start to learn how to use Makefiles as starting from the next homework you will need to write your own.

Most likely 'make' is not yet installed in your VM so please install it
```
sudo apt-get install make
```

In your new repository you will have hidden file for Travis CI. Please don't modify it unless we specifically ask. You also will have a `main.c` file. You will use it for development (feel free to use more *.h files and *.c if needed). You also may see Makefile.config. Please put your data into that file in the appropriate fields. In case you will be doing any of the extra credit options please change corresponding value in this file from zero to one.

You also can see Makefile file. It helps to build you programin by issuing a simple command
```
make
```
And that's it. Please modify it according to your needs but we still should be able just to run `make` and then everything should work. Most likely you will not need to change it. There is also this README file in the repository.

Below you can find few examples of different output. First two, are when there is some error:

- ADD 4 john doe 3.43 AMS
- ADD 4 mary jane 3.87 ART

You program is supposed to printf `ID NOT UNIQUE`.

 - ADD 3 john doe 3.43 AMS
 - ADD 5 mary jane 3.87 ART
 - DELETE 4
 
Now you should see `STUDENT  RECORD  CANNOT  BE  DELETED  NOR  UPDATED`.

Examples of correct input

 - ADD 3 john doe 3.43 AMS
 - ADD 5 mary jane 3.87 ART

Then imagine we are trying to print everyone in the database. We should see following results
```
3 John Doe 3.43 AMS
5 Mary Jane 3.87 ART
```

 - ADD 3 john doe 3.43 AMS
 - ADD 5 mary jane 3.87 ART
 - UPDATE 3 john Wonka 3.41 AMS

Then imagine we are trying to print everyone in the database. We should see following results
```
3 John Wonka 3.41 AMS
5 Mary Jane 3.87 ART
```
