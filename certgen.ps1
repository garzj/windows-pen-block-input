$fCertKey = $args[0]
$fCertCer = $args[1]
$fCertPfx = $args[2]

if (Test-Path -Path $fCertKey) {
  exit 0
}

$cert = New-SelfSignedCertificate -DnsName www.github.com -Type CodeSigning -CertStoreLocation cert:\CurrentUser\My

$certPwd = -join (((48..57)+(65..90)+(97..122)) * 80 |Get-Random -Count 20 |%{[char]$_})

Set-Content $fCertKey $certPwd

$securePwd = ConvertTo-SecureString -String $certPwd -Force -AsPlainText

Export-Certificate -Cert "cert:\CurrentUser\My\$($cert.Thumbprint)" -FilePath $fCertCer

Export-PfxCertificate -Cert "cert:\CurrentUser\My\$($cert.Thumbprint)" -FilePath $fCertPfx -Password $securePwd

exit 0
