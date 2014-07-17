
setlocal
set git_ver=
for /f %%x in ('git.exe log --max-count=1 ^| head -n 1 ^| sed "s/commit //g"') do set git_ver=%%x
if "%git_ver%"=="" (
    copy git.tmp git.hpp
) else (
    sed "s/COMMIT_VER/%git_ver%/g" git.tmp > git.hpp
)
endlocal

