@ECHO OFF

REM ==================================
REM Calls the Fabric Environment .bat
REM ==================================

CALL %cd%\..\..\..\environment.bat

REM ==================================
REM Adds the parent directory to the
REM PYTHONPATH environment variable.
REM ==================================

set PYTHONPATH=%PYTHONPATH%;%cd%\..;

REM ==================================
REM Launches the Python application.
REM ==================================

call cmd /k "python alembic_viewer.py"

PAUSE