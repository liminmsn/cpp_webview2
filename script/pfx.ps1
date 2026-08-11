$pfxPath = "C:\dev\MysqlRedisCert.pfx"
$pfxPasswordPlain = "123456"
$pfxPassword = ConvertTo-SecureString -String $pfxPasswordPlain -AsPlainText -Force

Export-PfxCertificate -Cert (Get-ChildItem Cert:\CurrentUser\My\7E4AED3BD9B8FDFA5F78EB6635DC49F47D7F3E83) `
    -FilePath $pfxPath -Password $pfxPassword -Force

Import-PfxCertificate -FilePath $pfxPath `
    -CertStoreLocation "Cert:\LocalMachine\TrustedPeople" `
    -Password $pfxPassword -Exportable


signtool sign /fd sha256 `
    /sha1 7E4AED3BD9B8FDFA5F78EB6635DC49F47D7F3E83 `
    /tr http://timestamp.sectigo.com /td sha256 `
    E:\source\cpp\cpp_webview2\script\MysqlRedis.msix

signtool verify /pa /v E:\source\cpp\cpp_webview2\script\MysqlRedis.msix
