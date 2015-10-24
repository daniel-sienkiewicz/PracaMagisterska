void setup()
{
  system("telnet -l /bin/sh");
  system("ifconfig eth0 192.168.0.69 netmask 255.255.0.0 up");
}

void loop () {
 }
