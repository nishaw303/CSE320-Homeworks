
# hw0
In this part of the assignment you will learn some basics of git and will start to use terminal.

First, you will need to create a directory that you will use for the course. In your VM type the following:
```bash
mkdir cse320
```
This will create a directory named "cse320" in your current directory. Hint: to see what is the current directory you can type: `pwd`, which will show full path and you always can use full path, while navigating through folders.
Then you will need to enter this directory by typing the following command in the terminal:
```bash
cd cse320
```
Now we are ready to clone our repository. To do that you need to type the following:
```bash
git clone https://github.com/CSE320Spring2018/hw0-<your GitHub username>
```
It may ask you for your username and password for the GitHub so please provide one. Now, as you successfully have cloned your repository you need to enter it with already familiar command:
```bash
cd hw0-<your GitHub username>
```
Now, let's get familiar with Travis CI interface that is used for providing feedback in our course. Type the following:
```bash
mv hello.a hello.b
```
This is will rename `hello.a` file into `hello.b`. As we made a change to our repository so version control (git in our case) should notice that! Let's check that by typing the following command:
```bash
git status
```
You can see that one file is deleted and one file is added. We are fine with that as this is what we just had done. So let's say to the git that we want these changes to be staged:
```bash
git add .
```
This '.' means that we want to add all changes that were done. Now we need to commit these changes, i.e. say to git that we want to create a check point at this state:
```bash
git commit -m "My first commit"
```
"-m" flag indicates that you include the message for commit right after that flag. Most likely, git will issue an error saying that you should configure it with your credentials such as name and email. Let's do that by typing the following commands (I will use my credentials but you should use yours) and then try again to commit our changes:
```bash
git config user.email "smadaminov@cs.stonybrook.edu"
git config user.name "Sergey Madaminov"
git commit -m "My first commit"
```
Now, it should work and we are ready to push our changes to the GitHub server. This is important to do as those changes are stored locally in your VM. If anything will happen to it (chances are high that it may happen around the submission deadline) then you may loose all your work! So please commit and push your changes often rather then rarely! Anyway, let's finally push our changes:
```bash
git push
```
Now, let's go to see results. Open the link https://github.com/CSE320Spring2018/hw0-<your GitHub username\> and click on the link to see your commits (it usually has name "X commits", where X is some number and there is timer icon just left to it). You should see list of commits in your repository. There will be more than one, which is fine as first few are made by us during creation of this assignment. On top you should see yours. But, wait...there is a red cross icon indicating that something went wrong. This is a notification from Travis CI saying that something went wrong. Let's click on it to see what exactly. NB: be aware that it may take some time to generate a page with report so if you will get any error such as that repository/owner cannot be found just be patient and try again in a few minutes.
After some time link should lead you to the report and you can scroll down to see something like that:
```bash
$ gcc hello.c -o hello
gcc: error: hello.c: No such file or directory
gcc: fatal error: no input files
compilation terminated.
```
Oh no! We did a mistake during renaming our file. Let's quickly fix it:
```bash
mv hello.b hello.c
git add .
git commit -m "rename fix"
git push
```
Now, let's check again our commit and we should see a green tick that indicates that now our submission passed pre-defined test. If we will click on the tick we should be able to see a report that will contain no errors. Finally, we are done!

Congratulations! That concludes first home work.
