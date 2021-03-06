Developer Workflow
==================

STADIC was originally developed using a minimal number of branches (master plus branches for big changes).
This is easy with a couple of developers with no releases to worry about, but once releases need to be
tracked it is worthwile to adopt a procedure. To that end, we're adopting Vincent Driessen's branching
model (described fully here: http://nvie.com/posts/a-successful-git-branching-model/).

This workflow makes use of two infinite-lifetime branches:

* master - code at a production-ready state
* develop - code that reflects the current state of development

The original does a very good job of describing the workflow, so it is worth the time to read the document.
Rather than rehash that, this document will describe what steps to take to use the workflow with our GitHub
repo.

Add a New Feature or Fix a Bug
------------------------------

Each new feature or bug fix should be associated with a GitHub issue or milestone. If there isn't an issue and the
proposed change is small, submit one and get yourself assigned to it. For larger changes, a milestone may be
appropriate (discuss this with the developers). Then:

1. Create a new branch from develop. Make sure that the branch is of develop and not of master and give the
branch a name like "issue-68", where 68 is the issue number. If your "u", "e", and "-" keys are defective,
then "iss68" is OK. It is also OK to add the name of the feature or a description to the branch name (e.g. 
"issue-68-fix-that-thing"). For milestones, name the the branch after the feature being developed (e.g. 
"milestone-add-that-feature"). The git commands to use are:

        $> git checkout develop
        $> git branch issue-68
        $> git checkout issue-68
    
2. Write code.

3. Create a pull request to merge this branch with develop. Once your code has been reviewed and merged, delete
the branch.

Releases
--------

The release procedure uses GitHub milestones. 

1. Create a milestone associated with a release. The milestone should be named after the release (e.g. "Version 1.0.0").

2. Create branch of develop named after the release (e.g. "release-1.0.0"):

        $> git checkout develop
        $> git branch release-1.0.0
        $> git checkout release-1.0.0

3. Write code.

4. Create a pull request to merge the release branch with develop and master.
