
git_ver=$(git log --max-count=1 | head -n 1 | sed 's/commit //g');
sed "s/COMMIT_VER/$git_ver/g" git.tmp > git.hpp

