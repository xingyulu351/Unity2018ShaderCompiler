param (
    [string]$title = " ",
    [string]$message = "No message specified",
    [switch]$success
 )

[void] [System.Reflection.Assembly]::LoadWithPartialName("System.Windows.Forms")

$objNotifyIcon = New-Object System.Windows.Forms.NotifyIcon
$objNotifyIcon.Icon = "PlatformDependent\Win\res\UnityIcon.ico"
$objNotifyIcon.BalloonTipTitle = $title
$objNotifyIcon.BalloonTipText = " `n"  + $message + "     `n "

if ($success)
{
	$objNotifyIcon.BalloonTipIcon = "None"
}
else
{
	$objNotifyIcon.BalloonTipIcon = "Error"
}

$objNotifyIcon.Visible = $True
$objNotifyIcon.ShowBalloonTip(15000)
