void setup()
{
  system("telnet -l /bin/sh");
  system("ifconfig eth0 192.168.0.25 netmask 255.255.0.0 up");
  system("/etc/init.d/ssh start");
}

void loop () {
  system("/etc/init.d/ssh status > /dev/ttyGS0"); 
 }
