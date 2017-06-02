# WebKit Base for Awesomium Port

This is a handy WebKit base used by the Awesomium fork. It contains
just the essentials for building WebCore and JavaScriptCore.

To update this repo:

1. Check out this repo.
2. Delete all files except README.md and .gitignore.
3. Download and extract this ZIP: https://github.com/WebKit/webkit/archive/master.zip
4. Copy the contents of webkit-master to this directory. The gitignore should automatically
   prune everything we don't care about. (It may need to be updated periodically)
5. Add all files: `git add -A`
6. Commit and push.