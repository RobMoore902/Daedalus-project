#!/usr/bin/perl -wT

#Set the environment path for `mysqldump`
$ENV{'PATH'} = '/usr/bin';

$Month = (localtime(time))[4]+1;
$Year = (localtime(time))[5]+1900;
$Day = (localtime(time))[3];

$dt = sprintf("%10s%04d-%02d-%02d", "db_backup_", $Year, $Month, $Day, );

exec "mysqldump -u daedalus -pu8i9o0p- daedalus_dev > /var/www/html/daedalus_dev/sites/all/modules/custom/daedalus/db_backup/$dt.sql";
