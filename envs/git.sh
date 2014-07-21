
#!/bin/sh

command -v git > /dev/null
if [ $? -eq 0 ]
then
    git_ver=$(git log --max-count=1 | head -n 1 | sed 's/commit //g')
    sed "s/COMMIT_VER/$git_ver/g" git.tmp > git.tmp2
else
    cp git.tmp git.tmp2
fi

diff git.hpp git.tmp2 > /dev/null
if [ $? -ne 0 ]
then
    mv git.tmp2 git.hpp
else
    rm git.tmp2
fi

