function Gen-Key($length) {
  $characters = 'abcdefghiklmnoprstuvwxyzABCDEFGHKLMNOPRSTUVWXYZ1234567890'
  $random = 1..$length | ForEach-Object { Get-Random -Maximum $characters.length }
  $private:ofs = ''
  return [String]$characters[$random]
}

$fCertKey = $args[0]
$fCertCer = $args[1]
$fCertPfx = $args[2]

if (Test-Path -Path $fCertKey) {
  exit 0
}

$cert = New-SelfSignedCertificate -DnsName "StylusBlockInput Root CA" -Type CodeSigning -CertStoreLocation cert:\CurrentUser\My

$certPwd = Gen-Key -length 64

Set-Content $fCertKey $certPwd

$securePwd = ConvertTo-SecureString -String $certPwd -Force -AsPlainText

Export-Certificate -Cert "cert:\CurrentUser\My\$($cert.Thumbprint)" -FilePath $fCertCer

Export-PfxCertificate -Cert "cert:\CurrentUser\My\$($cert.Thumbprint)" -FilePath $fCertPfx -Password $securePwd

exit 0
