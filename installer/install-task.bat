powershell -c ^
  $action = New-ScheduledTaskAction -Execute 'cmd.exe' -Argument '/c start """""""" ""%1""'; ^
  $trigger = New-ScheduledTaskTrigger -AtLogon -RandomDelay 00:00:15; ^
  $settings = New-ScheduledTaskSettingsSet -AllowStartIfOnBatteries -DontStopIfGoingOnBatteries -Compatibility Win8; ^
  $principal = New-ScheduledTaskPrincipal -GroupId 'BUILTIN\Administrators' -RunLevel Highest; ^
  $definition = New-ScheduledTask -Action $action -Principal $principal -Trigger $trigger -Settings $settings -Description 'Run Stylus Block Input at startup.'; ^
  Register-ScheduledTask -TaskName 'Stylus Block Input' -InputObject $definition
