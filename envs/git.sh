
#!/bin/sh

command -v git > /dev/null
if [ $? -eq 0 ]
    then
    git_ver=$(git log --max-count=1 | head -n 1 | sed 's/commit //g')
    sed "s/COMMIT_VER/$git_ver/g" git.tmp > git.hpp
else
    cp git.tmp git.hpp
fi

